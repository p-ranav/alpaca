#include <alpaca/alpaca.h>
#include <benchmark/benchmark.h>
#include "log.h"
#include <random>

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_real_distribution<float> real_distr(-1000.0f, 1000.0f);

static void BM_deserialize_log_10k(benchmark::State &state) {
  {
    auto logs = alpaca::benchmark::generate_logs(eng);

    std::array<uint8_t, 1000000> bytes;

    // serialize
    auto data_size = alpaca::serialize(logs, bytes);

    std::error_code ec;
    alpaca::benchmark::Logs logs_recovered;

    for (auto _ : state) {
      // This code gets timed
      logs_recovered = alpaca::deserialize<alpaca::benchmark::Logs>(bytes, ec);
    }

    state.counters["Success"] = ((bool)ec == false);
    state.counters["BytesOutput"] = data_size;
    state.counters["Logs"] = logs_recovered.logs.size();
    state.counters["DataRate"] = benchmark::Counter(data_size, benchmark::Counter::kIsRate);
  }
}

BENCHMARK(BM_deserialize_log_10k);

BENCHMARK_MAIN();