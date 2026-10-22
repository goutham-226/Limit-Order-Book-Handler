<h1 align="center">Limit Order Book Handler</h1>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-23-00599C?logo=cplusplus&logoColor=white" alt="C++23">
  <img src="https://img.shields.io/badge/CMake-3.28%2B-064F8C?logo=cmake&logoColor=white" alt="CMake 3.28+">
  <img src="https://img.shields.io/badge/tests-GoogleTest-4285F4?logo=google&logoColor=white" alt="GoogleTest">
  <img src="https://img.shields.io/badge/bench-Google%20Benchmark-34A853" alt="Google Benchmark">
  <img src="https://img.shields.io/badge/platform-Linux-FCC624?logo=linux&logoColor=black" alt="Linux">
  <img src="https://img.shields.io/github/last-commit/goutham-226/Limit-Order-Book-Handler" alt="Last commit">
</p>

A low-latency limit order book written in modern C++, built module by module with benchmarks and tests at each step.

## Progress

- **Module 0 — Rig:** benchmark harness (Google Benchmark) and CPU setup script (`scripts/rig.sh`) for stable measurements.
- **Module 1 — Pool allocator:** fixed-size, stack-based slot pool (`include/lob/core/pool.hpp`) with O(1) add/delete and free-slot reuse.

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

Sanitizer builds (for testing only, never benchmarking): `-DLOB_ASAN=ON` or `-DLOB_TSAN=ON`.

## Test & Benchmark

```bash
ctest --test-dir build --output-on-failure
./scripts/rig.sh on && ./build/bench; ./scripts/rig.sh off
```

## Layout

```
include/lob/   header-only library
tests/         GoogleTest unit tests
bench/         Google Benchmark suites
scripts/       machine setup for benchmarking
results/       saved benchmark results
```
