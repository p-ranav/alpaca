#pragma once
#include <vector>

namespace alpaca {

namespace benchmark {

struct Vector3 {
  float x;
  float y;
  float z;
};

struct Triangle {
  Vector3 v0;
  Vector3 v1;
  Vector3 v2;
  Vector3 normal;
};

struct Mesh {
  std::vector<Triangle> triangles;
};

template <std::size_t NumTriangles, typename Engine, typename Distr>
auto generate_mesh(Mesh& mesh, Engine& eng, Distr &distr) {
    for (std::size_t i = 0; i < NumTriangles; ++i) {
        mesh.triangles.push_back(Triangle{
            Vector3{distr(eng), distr(eng), distr(eng)},
            Vector3{distr(eng), distr(eng), distr(eng)},
            Vector3{distr(eng), distr(eng), distr(eng)},
            Vector3{distr(eng), distr(eng), distr(eng)}
        });
    }
}

}

}