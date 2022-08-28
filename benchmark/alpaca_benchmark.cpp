#include <alpaca/alpaca.h>
#include <benchmark/benchmark.h>
#include <cassert>
#include <chrono>
#include <functional>
#include <random>
#include <type_traits>

// simple implementations of uniform distribution functions to generate same
// test data on different platforms.
template <typename TValue> class UniformIntDistribution {
public:
  static_assert(std::is_integral<TValue>::value, "");
  UniformIntDistribution(TValue a,
                         TValue b = std::numeric_limits<TValue>::max())
      : _a{a}, _b{b} {
    assert(a < b);
  }

  template <typename Engine> TValue operator()(Engine &eng) {
    auto urange = _b - _a;
    auto erange = eng.max() - eng.min();
    assert(erange > static_cast<decltype(erange)>(urange));
    auto scale = erange / urange;
    while (true) {
      auto res = eng() - eng.min();
      if (res >= scale * urange)
        continue;
      return res / scale + _a;
    }
  }

private:
  TValue _a;
  TValue _b;
};

template <typename TValue> class UniformRealDistribution {
public:
  static_assert(std::is_floating_point<TValue>::value, "");
  UniformRealDistribution(TValue a, TValue b) : _a{a}, _b{b} { assert(a < b); }

  template <typename Engine> TValue operator()(Engine &eng) {
    auto urange = _b - _a;
    auto erange = eng.max() - eng.min();
    return static_cast<TValue>(eng()) / erange * (urange) + _a;
  }

private:
  TValue _a;
  TValue _b;
};

static UniformIntDistribution<int16_t> rand_char((int16_t)'A', (int16_t)'Z');
static UniformIntDistribution<int> rand_len(1, 10);
static UniformIntDistribution<int16_t> rand_nr(0);
static UniformRealDistribution<float> rand_float(-1.0f, 1.0f);

typedef std::mersenne_twister_engine<
    uint_fast32_t, 32, 624, 397, 31, 0x9908b0dfUL, 11, 0xffffffffUL, 7,
    0x9d2c5680UL, 15, 0xefc60000UL, 18, 1812433253UL>
    engine;

using namespace alpaca;

enum Color : uint8_t { Red, Green, Blue };

struct Vec3 {
  float x;
  float y;
  float z;
};

struct Weapon {
  std::string name;
  int16_t damage;
};

struct Monster {
  Vec3 pos;
  int16_t mana;
  int16_t hp;
  std::string name;
  std::vector<uint8_t> inventory;
  Color color;
  std::vector<Weapon> weapons;
  Weapon equipped;
  std::vector<Vec3> path;
};

static Weapon createRandomWeapon(engine &e) {
  Weapon res;
  res.damage = rand_nr(e);
  std::generate_n(std::back_inserter(res.name), rand_len(e),
                  std::bind(rand_char, std::ref(e)));
  return res;
}

Monster createRandomMonster(engine &e) {
  Monster res{};
  std::generate_n(std::back_inserter(res.name), rand_len(e),
                  std::bind(rand_char, std::ref(e)));
  res.pos.x = rand_float(e);
  res.pos.y = rand_float(e);
  res.pos.z = rand_float(e);
  res.color = static_cast<Color>(rand_len(e) % static_cast<int>(3));
  res.hp = rand_nr(e) % 1000;
  res.mana = std::abs(rand_nr(e) % 500);
  static_assert(std::is_copy_constructible<engine>::value, "");
  std::generate_n(std::back_inserter(res.inventory), rand_len(e),
                  std::bind(rand_len, std::ref(e)));
  std::generate_n(std::back_inserter(res.path), rand_len(e), [&]() {
    return Vec3{rand_float(e), rand_float(e), rand_float(e)};
  });
  res.equipped = createRandomWeapon(e);
  std::generate_n(std::back_inserter(res.weapons), rand_len(e),
                  std::bind(createRandomWeapon, std::ref(e)));
  return res;
}

std::vector<Monster> createMonsters(size_t count) {
  std::vector<Monster> res{};
  // always the same seed
  std::seed_seq seed{1, 2, 3};
  engine e{seed};

  std::generate_n(std::back_inserter(res), count,
                  std::bind(createRandomMonster, std::ref(e)));
  return res;
}

static void BM_alpaca_serialize(benchmark::State &state) {
  {
    struct my_struct {
      std::vector<Monster> values;
    };

    std::size_t data_size = 0;
    my_struct s{createMonsters(state.range(0))};

    for (auto _ : state) {
      // This code gets timed
      // serialize
      auto bytes = serialize(s);

      data_size = bytes.size();
    }

    state.counters["BytesOutput"] = data_size;
  }
}

static void BM_alpaca_serialize_with_checksum(benchmark::State &state) {
  {
    struct my_struct {
      std::vector<Monster> values;
    };

    std::size_t data_size = 0;
    my_struct s{createMonsters(state.range(0))};

    for (auto _ : state) {
      // This code gets timed
      // serialize
      auto bytes = serialize<my_struct, options::with_checksum>(s);

      data_size = bytes.size();
    }

    state.counters["BytesOutput"] = data_size;
  }
}

// static void BM_alpaca_serialize_with_version(benchmark::State &state) {
//   {
//     struct my_struct {
//       std::vector<Monster> values;
//     };

//     std::size_t data_size = 0;
//     my_struct s{createMonsters(state.range(0))};

//     for (auto _ : state) {
//       // This code gets timed
//       // serialize
//       auto bytes = serialize<my_struct, options::with_version>(s);

//       data_size = bytes.size();
//     }

//     state.counters["BytesOutput"] = data_size;
//   }
// }

static void BM_alpaca_deserialize(benchmark::State &state) {
  {
    struct my_struct {
      std::vector<Monster> values;
    };

    std::size_t data_size = 0;
    my_struct s{createMonsters(state.range(0))};
    // serialize
    auto bytes = serialize(s);

    data_size = bytes.size();

    for (auto _ : state) {
      // This code gets timed
      // deserialize
      std::error_code ec;
      auto recovered = deserialize<my_struct>(bytes, ec);
    }

    state.counters["BytesOutput"] = data_size;
  }
}

static void BM_alpaca_deserialize_with_checksum(benchmark::State &state) {
  {
    struct my_struct {
      std::vector<Monster> values;
    };

    std::size_t data_size = 0;
    my_struct s{createMonsters(state.range(0))};
    // serialize
    auto bytes = serialize<my_struct, options::with_checksum>(s);

    data_size = bytes.size();

    for (auto _ : state) {
      // This code gets timed
      // deserialize
      std::error_code ec;
      auto recovered =
          deserialize<my_struct, options::with_checksum>(bytes, ec);
    }

    state.counters["BytesOutput"] = data_size;
  }
}

// static void BM_alpaca_deserialize_with_version(benchmark::State &state) {
//   {
//     struct my_struct {
//       std::vector<Monster> values;
//     };

//     std::size_t data_size = 0;
//     my_struct s{createMonsters(state.range(0))};
//     // serialize
//     auto bytes = serialize<my_struct, options::with_version>(s);

//     data_size = bytes.size();

//     for (auto _ : state) {
//       // This code gets timed
//       // deserialize
//       std::error_code ec;
//       auto recovered =
//           deserialize<my_struct, options::with_version>(bytes, ec);
//     }

//     state.counters["BytesOutput"] = data_size;
//   }
// }

static void BM_alpaca_serialize_then_deserialize(benchmark::State &state) {
  {
    struct my_struct {
      std::vector<Monster> values;
    };

    std::size_t data_size = 0;
    my_struct s{createMonsters(state.range(0))};

    for (auto _ : state) {
      // This code gets timed
      // serialize
      auto bytes = serialize(s);

      data_size = bytes.size();

      // deserialize
      std::error_code ec;
      auto recovered = deserialize<my_struct>(bytes, ec);
    }

    state.counters["BytesOutput"] = data_size;
  }
}

static void
BM_alpaca_serialize_then_deserialize_with_checksum(benchmark::State &state) {
  {
    struct my_struct {
      std::vector<Monster> values;
    };

    std::size_t data_size = 0;
    my_struct s{createMonsters(state.range(0))};

    for (auto _ : state) {
      // This code gets timed
      // serialize
      auto bytes = serialize<my_struct, options::with_checksum>(s);

      data_size = bytes.size();

      // deserialize
      std::error_code ec;
      auto recovered =
          deserialize<my_struct, options::with_checksum>(bytes, ec);
    }

    state.counters["BytesOutput"] = data_size;
  }
}

BENCHMARK(BM_alpaca_serialize)->Arg(50)->Arg(100)->Arg(1E3)->Arg(1E4)->Arg(1E5);
BENCHMARK(BM_alpaca_serialize_with_checksum)
    ->Arg(50)
    ->Arg(100)
    ->Arg(1E3)
    ->Arg(1E4)
    ->Arg(1E5);
// BENCHMARK(BM_alpaca_serialize_with_version)
//     ->Arg(50)
//     ->Arg(100)
//     ->Arg(1E3)
//     ->Arg(1E4)
//     ->Arg(1E5);
BENCHMARK(BM_alpaca_deserialize)
    ->Arg(50)
    ->Arg(100)
    ->Arg(1E3)
    ->Arg(1E4)
    ->Arg(1E5);
BENCHMARK(BM_alpaca_deserialize_with_checksum)
    ->Arg(50)
    ->Arg(100)
    ->Arg(1E3)
    ->Arg(1E4)
    ->Arg(1E5);
// BENCHMARK(BM_alpaca_deserialize_with_version)
//     ->Arg(50)
//     ->Arg(100)
//     ->Arg(1E3)
//     ->Arg(1E4)
//     ->Arg(1E5);

// Run the benchmark
BENCHMARK_MAIN();