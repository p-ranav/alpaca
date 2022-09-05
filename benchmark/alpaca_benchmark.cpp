#include <alpaca/alpaca.h>
#include <benchmark/benchmark.h>
#include "mesh.h"
#include <random>

static void BM_alpaca_serialize_mesh_125k_array(benchmark::State &state) {
  {
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<float> distr(-1000.0f, 1000.0f);

    alpaca::benchmark::Mesh m;
    alpaca::benchmark::generate_mesh<125000>(m, eng, distr);

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
  }
}

BENCHMARK(BM_alpaca_serialize_mesh_125k_array);

BENCHMARK_MAIN();
