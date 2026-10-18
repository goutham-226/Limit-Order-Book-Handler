#include<benchmark/benchmark.h>
#include<cstddef>
#include<cstdint>
#include<vector>

/**
 * @brief Measures the cost of an empty benchmark loop (the noise floor).
 *
 * The body does no real work, so differences between repetitions come from
 * the machine (frequency scaling, scheduling, interrupts), not the code.
 * Run with --benchmark_repetitions=10; the Module 0 gate is CV < 2%.
 *
 * @param state Google Benchmark run state; drives the timing loop and
 *              supplies the iteration counter fed to DoNotOptimize.
 */
static void BM_Empty(benchmark::State& state)
{
    for(auto _: state)
    {
        benchmark::DoNotOptimize(state.iterations());
    }
}

BENCHMARK(BM_Empty);

/**
 * @brief Sums a uint64_t array sequentially to show throughput per cache level.
 *
 * Array size grows from 8 KB to 32 MB; bytes/second steps down as the working
 * set leaves L1, L2 and L3. Baseline for the pointer-chase probe in Module 2.
 *
 * @param state Google Benchmark run state. state.range(0) is the number of
 *              elements; SetBytesProcessed reports throughput in bytes/second.
 */

static void BM_SumArray(benchmark::State& state)
{
    const auto n = static_cast<std::size_t>(state.range(0));
    std::vector<std::uint64_t> v(n,1);
    for(auto _: state)
    {
        std::uint64_t sum = 0;
        for(auto j: v)
        {
            sum += j;
        }
        benchmark::DoNotOptimize(sum);
    }
    state.SetBytesProcessed(state.iterations() * state.range(0)
                            * static_cast<std::int64_t>(sizeof(std::uint64_t)));
}

BENCHMARK(BM_SumArray)->RangeMultiplier(4)->Range(1 << 10, 1 << 22);
