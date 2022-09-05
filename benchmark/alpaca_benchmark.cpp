#include <alpaca/alpaca.h>
#include <benchmark/benchmark.h>
#include "log.h"
#include "mesh.h"
#include "minecraft_save.h"
#include <random>

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_real_distribution<float> real_distr(-1000.0f, 1000.0f);

static void BM_serialize_log_10k(benchmark::State &state) {
  {
    auto logs = alpaca::benchmark::generate_logs(eng);

    std::array<uint8_t, 1000000> bytes;
    std::size_t data_size = 0;

    for (auto _ : state) {
      // This code gets timed
      // serialize
      data_size = alpaca::serialize(logs, bytes);
    }

    std::error_code ec;
    auto logs_recovered = alpaca::deserialize<alpaca::benchmark::Logs>(bytes, ec);
    state.counters["Success"] = ((bool)ec == false);
    state.counters["BytesOutput"] = data_size;
    state.counters["Logs"] = logs_recovered.logs.size();
    state.counters["DataRate"] = benchmark::Counter(data_size, benchmark::Counter::kIsRate);
  }
}

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

static void BM_serialize_mesh_125k(benchmark::State &state) {
  {
    alpaca::benchmark::Mesh m;
    alpaca::benchmark::generate_mesh<125000>(m, eng, real_distr);

    std::array<uint8_t, 6000024> bytes;
    std::size_t data_size = 0;

    for (auto _ : state) {
      // This code gets timed
      // serialize
      data_size = alpaca::serialize(m, bytes);
    }

    std::error_code ec;
    auto m_recovered = alpaca::deserialize<alpaca::benchmark::Mesh>(bytes, ec);
    state.counters["Success"] = ((bool)ec == false);
    state.counters["BytesOutput"] = data_size;
    state.counters["Triangles"] = m_recovered.triangles.size();
    state.counters["DataRate"] = benchmark::Counter(data_size, benchmark::Counter::kIsRate);
  }
}

static void BM_deserialize_mesh_125k(benchmark::State &state) {
  {
    alpaca::benchmark::Mesh m;
    alpaca::benchmark::generate_mesh<125000>(m, eng, real_distr);

    std::array<uint8_t, 6000024> bytes;
    auto data_size = alpaca::serialize(m, bytes);

    std::error_code ec;
    alpaca::benchmark::Mesh m_recovered;

    for (auto _ : state) {
      // This code gets timed
      m_recovered = alpaca::deserialize<alpaca::benchmark::Mesh>(bytes, ec);
    }

    state.counters["Success"] = ((bool)ec == false);
    state.counters["BytesOutput"] = data_size;
    state.counters["Triangles"] = m_recovered.triangles.size();
    state.counters["DataRate"] = benchmark::Counter(data_size, benchmark::Counter::kIsRate);
  }
}

static void BM_serialize_minecraft_players_50(benchmark::State &state) {
  {
    alpaca::benchmark::Players m;

    for (std::size_t i = 0; i < 50; ++i) {
      m.players.push_back(alpaca::benchmark::generate_player(eng));
    }

    std::array<uint8_t, 150000> bytes;
    std::size_t data_size = 0;

    constexpr auto OPTIONS = alpaca::options::fixed_length_encoding;

    for (auto _ : state) {
      // This code gets timed
      // serialize
      data_size = alpaca::serialize<OPTIONS>(m, bytes);
    }

    std::error_code ec;
    auto m_recovered = alpaca::deserialize<OPTIONS, alpaca::benchmark::Players>(bytes, ec);
    state.counters["Success"] = ((bool)ec == false);
    state.counters["BytesOutput"] = data_size;
    state.counters["Players"] = m_recovered.players.size();
    state.counters["DataRate"] = benchmark::Counter(data_size, benchmark::Counter::kIsRate);
  }
}

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

BENCHMARK(BM_serialize_log_10k);
BENCHMARK(BM_deserialize_log_10k);
BENCHMARK(BM_serialize_mesh_125k);
BENCHMARK(BM_deserialize_mesh_125k);
BENCHMARK(BM_serialize_minecraft_players_50);
BENCHMARK(BM_deserialize_minecraft_players_50);

BENCHMARK_MAIN();
