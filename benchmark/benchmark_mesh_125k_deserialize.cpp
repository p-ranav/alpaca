#include <alpaca/alpaca.h>
#include <benchmark/benchmark.h>
#include "mesh.h"
#include <random>

std::random_device rd;
std::default_random_engine eng(rd());
std::uniform_real_distribution<float> real_distr(-1000.0f, 1000.0f);

static void BM_deserialize_mesh_125k(benchmark::State &state) {
    constexpr auto NUM_TRIANGLES = 125000;

    alpaca::benchmark::Mesh m;
    alpaca::benchmark::generate_mesh<NUM_TRIANGLES>(m, eng, real_distr);

    std::array<uint8_t, 6000024> bytes;
    std::size_t data_size = alpaca::serialize(m, bytes);

    std::error_code ec;
    alpaca::benchmark::Mesh m_recovered;

    for (auto _ : state) {
        // This code gets timed
        m_recovered = alpaca::deserialize<alpaca::benchmark::Mesh>(bytes, data_size, ec);
    }

    state.counters["Success"] = ((bool)ec == false);
    state.counters["BytesOutput"] = data_size;
    std::size_t num_triangles_verified = 0;

    // Verify result
    for (std::size_t i = 0; i < NUM_TRIANGLES; ++i) {
        if (m_recovered.triangles[i].v0.x == m.triangles[i].v0.x && 
        m_recovered.triangles[i].v0.y == m.triangles[i].v0.y &&
        m_recovered.triangles[i].v0.z == m.triangles[i].v0.z && 
        m_recovered.triangles[i].v1.x == m.triangles[i].v1.x && 
        m_recovered.triangles[i].v1.y == m.triangles[i].v1.y &&
        m_recovered.triangles[i].v1.z == m.triangles[i].v1.z &&
        m_recovered.triangles[i].v2.x == m.triangles[i].v2.x && 
        m_recovered.triangles[i].v2.y == m.triangles[i].v2.y &&
        m_recovered.triangles[i].v2.z == m.triangles[i].v2.z &&
        m_recovered.triangles[i].normal.x == m.triangles[i].normal.x && 
        m_recovered.triangles[i].normal.y == m.triangles[i].normal.y &&
        m_recovered.triangles[i].normal.z == m.triangles[i].normal.z) {
            num_triangles_verified += 1;
        }
    }

    state.counters["Triangles"] = num_triangles_verified;
    state.counters["DataRate"] = benchmark::Counter(data_size, benchmark::Counter::kIsRate);
}

BENCHMARK(BM_deserialize_mesh_125k);

BENCHMARK_MAIN();
