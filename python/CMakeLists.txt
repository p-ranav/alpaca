cmake_minimum_required(VERSION 3.15)

project(pyalpaca CXX)

include(../cmake/project-is-top-level.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(alpaca REQUIRED)
endif()

if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DNDEBUG)
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Set default compile flags for GCC
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "-Wall -Wextra")
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif(CMAKE_COMPILER_IS_GNUCXX)

# Prefer an installed pybind11 (`pip install pybind11`, then
# -Dpybind11_DIR=$(python -m pybind11 --cmakedir) if it is not on the prefix
# path); fall back to the vendored submodule. PYBIND11_FINDPYTHON makes both
# use CMake's FindPython, so the interpreter first on PATH (or
# -DPython_EXECUTABLE=...) is the one the module is built for.
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG QUIET)
if(NOT pybind11_FOUND)
  if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pybind11/CMakeLists.txt")
    message(FATAL_ERROR "pybind11 not found: pip install pybind11, or git submodule update --init python/pybind11")
  endif()
  add_subdirectory(pybind11)
endif()

pybind11_add_module(pyalpaca pyalpaca.cpp)
target_compile_features(pyalpaca PRIVATE cxx_std_20)
target_link_libraries(pyalpaca PRIVATE alpaca::alpaca)
