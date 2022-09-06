#include <alpaca/alpaca.h>
#include <benchmark/benchmark.h>
#include "log.h"
#include <random>

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_real_distribution<float> real_distr(-1000.0f, 1000.0f);

static void BM_deserialize_minecraft_players_50(benchmark::State &state) {
  {
    alpaca::benchmark::Players m;

    for (std::size_t i = 0; i < 50; ++i) {
      m.players.push_back(alpaca::benchmark::generate_player(eng));
    }

    constexpr auto OPTIONS = alpaca::options::fixed_length_encoding;

    std::array<uint8_t, 150000> bytes;
    std::size_t data_size = alpaca::serialize<OPTIONS>(m, bytes);

    std::error_code ec;
    alpaca::benchmark::Players m_recovered;

    for (auto _ : state) {
      // This code gets timed
      // serialize
      m_recovered = alpaca::deserialize<OPTIONS, alpaca::benchmark::Players>(bytes, ec);
    }

    state.counters["Success"] = ((bool)ec == false);
    state.counters["BytesOutput"] = data_size;
    state.counters["Players"] = m_recovered.players.size();
    state.counters["DataRate"] = benchmark::Counter(data_size, benchmark::Counter::kIsRate);
  }
}

BENCHMARK(BM_deserialize_minecraft_players_50);

BENCHMARK_MAIN();