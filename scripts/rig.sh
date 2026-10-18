#!/usr/bin/env bash
# Module 0 machine setup for AMD Ryzen 5 5500 (Zen 3, 6 cores / 12 threads,
# base 3.6 GHz, boost 4.2 GHz, frequency driver: amd-pstate or acpi-cpufreq).
#
# Usage: ./scripts/rig.sh on | off | status
#   on     : performance governor, boost off, clock pinned to base, perf counters allowed
#   off    : restore exactly what the first `on` saved
#   status : show current state, change nothing
#
# Pin a different clock:  BASE_KHZ=3400000 ./scripts/rig.sh on
set -euo pipefail
shopt -s nullglob

CPU=${CPU_SYSFS:-/sys/devices/system/cpu}
PARANOID=${PARANOID_FILE:-/proc/sys/kernel/perf_event_paranoid}
BASE_KHZ=${BASE_KHZ:-3600000}              # Ryzen 5 5500 base clock, in kHz
STATE=${STATE_FILE:-/tmp/lob_rig_state}    # original settings, saved by `on`
SUDO=${SUDO-sudo}

POLICIES=("$CPU"/cpufreq/policy*)          # one cpufreq policy per logical CPU on Zen 3

die()  { echo "rig: $*" >&2; exit 1; }
wr()   { local v=$1; shift; printf '%s\n' "$v" | $SUDO tee "$@" > /dev/null; }   # wr VALUE FILE...

# Write VALUE to FILE inside every policy that has that file.
set_all() {
  local f=$1 v=$2 p targets=()
  for p in "${POLICIES[@]}"; do if [[ -f $p/$f ]]; then targets+=("$p/$f"); fi; done
  if (( ${#targets[@]} )); then wr "$v" "${targets[@]}"; fi
}

# "value xCount" summary of FILE across all policies.
summary() {
  local p vals=()
  for p in "${POLICIES[@]}"; do if [[ -f $p/$1 ]]; then vals+=("$(<"$p/$1")"); fi; done
  if (( ${#vals[@]} )); then
    printf '%s\n' "${vals[@]}" | sort | uniq -c | awk '{printf "%s%s x%s", (NR>1 ? "  " : ""), $2, $1} END {print ""}'
  else
    echo "n/a"
  fi
}

save_state() {
  if [[ -f $STATE ]]; then return; fi      # `on` twice keeps the ORIGINAL settings
  local p f
  {
    # Order matters for restore: boost first, then governor, then max before min.
    if [[ -f $CPU/cpufreq/boost ]]; then echo "$CPU/cpufreq/boost $(<"$CPU/cpufreq/boost")"; fi
    for p in "${POLICIES[@]}"; do
      for f in boost scaling_governor scaling_max_freq scaling_min_freq; do
        if [[ -f $p/$f ]]; then echo "$p/$f $(<"$p/$f")"; fi
      done
    done
    echo "$PARANOID $(<"$PARANOID")"
  } > "$STATE"
}

boost_off() {
  if [[ -f $CPU/cpufreq/boost ]] && wr 0 "$CPU/cpufreq/boost" 2>/dev/null; then
    echo "rig: boost disabled (global switch)"; return
  fi
  local p files=()
  for p in "${POLICIES[@]}"; do if [[ -f $p/boost ]]; then files+=("$p/boost"); fi; done
  if (( ${#files[@]} )) && wr 0 "${files[@]}" 2>/dev/null; then
    echo "rig: boost disabled (per CPU)"; return
  fi
  echo "rig: no boost switch found; the clock cap below still applies." \
       "For certainty, disable Core Performance Boost in BIOS."
}

# Cap max at BASE_KHZ, then raise min to whatever max the kernel accepted:
# min == max means the clock cannot wander.
pin_clock() {
  local p max
  for p in "${POLICIES[@]}"; do
    wr "$BASE_KHZ" "$p/scaling_max_freq"
    max=$(<"$p/scaling_max_freq")
    wr "$max" "$p/scaling_min_freq"
  done
}

status() {
  if (( ${#POLICIES[@]} == 0 )); then echo "cpufreq  : not available (VM, or cpufreq disabled)"; fi
  local first=${POLICIES[0]:-}
  echo "driver   : $( [[ -n $first && -f $first/scaling_driver ]] && cat "$first/scaling_driver" || echo n/a)"
  echo "governor : $(summary scaling_governor)"
  echo "epp      : $(summary energy_performance_preference)"
  if [[ -f $CPU/cpufreq/boost ]]; then
    echo "boost    : $(<"$CPU/cpufreq/boost") (global; 0 = off)"
  else
    echo "boost    : $(summary boost)   (per CPU; 0 = off)"
  fi
  echo "min kHz  : $(summary scaling_min_freq)"
  echo "max kHz  : $(summary scaling_max_freq)"
  echo "now kHz  : $(summary scaling_cur_freq)"
  echo "perf     : perf_event_paranoid=$(cat "$PARANOID")   (want <= 1)"
  echo "tsc      : $(grep -o -w -E 'constant_tsc|nonstop_tsc' /proc/cpuinfo | sort -u | xargs)"
  echo "smt      : $(cat "$CPU/smt/control" 2>/dev/null || echo n/a)"
  if [[ -f $CPU/cpu2/topology/thread_siblings_list ]]; then
    echo "cpu2     : shares a core with CPUs $(<"$CPU/cpu2/topology/thread_siblings_list") (keep the sibling idle when pinning to 2)"
  fi
  if [[ -f $STATE ]]; then echo "saved    : $STATE (restore with: $0 off)"; fi
}

on() {
  (( ${#POLICIES[@]} )) || die "no cpufreq policies under $CPU/cpufreq (VM, or cpufreq disabled?)"
  save_state
  set_all scaling_governor performance
  boost_off
  pin_clock
  wr 1 "$PARANOID"
  status
}

off() {
  [[ -f $STATE ]] || die "nothing to restore: $STATE not found (run '$0 on' first)"
  local path val
  while read -r path val; do
    if ! wr "$val" "$path" 2>/dev/null; then echo "rig: could not restore $path" >&2; fi
  done < "$STATE"
  rm -f "$STATE"
  status
}

case "${1:-status}" in
  on)     on ;;
  off)    off ;;
  status) status ;;
  *)      echo "usage: $0 on|off|status" >&2; exit 1 ;;
esac
