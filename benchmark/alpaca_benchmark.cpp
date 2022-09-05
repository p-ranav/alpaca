#include <alpaca/alpaca.h>
#include <benchmark/benchmark.h>
#include "mesh.h"
#include <random>

static void BM_alpaca_serialize_mesh_125k(benchmark::State &state) {
  {
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<float> distr(-1000.0f, 1000.0f);

    alpaca::benchmark::Mesh m;
    alpaca::benchmark::generate_mesh<125000>(m, eng, distr);

    std::vector<uint8_t> bytes;
    bytes.reserve(6000024);
    std::size_t data_size = 0;

    for (auto _ : state) {
      // This code gets timed
      // serialize
      data_size = alpaca::serialize(m, bytes);
    }

    state.counters["BytesOutput"] = data_size;
  }
}

BENCHMARK(BM_alpaca_serialize_mesh_125k);

BENCHMARK_MAIN();