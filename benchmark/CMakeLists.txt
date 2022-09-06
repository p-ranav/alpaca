cmake_minimum_required(VERSION 3.14)

project(alpacaBenchmark CXX)

include(../cmake/project-is-top-level.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(alpaca REQUIRED)
endif()

find_package(benchmark CONFIG)
if(NOT benchmark_FOUND)
    message(STATUS "Using CMake Version ${CMAKE_VERSION}")
    if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.11.0")
        # User can fetch googlebenchmark
        message(STATUS "Downloading GoogleBenchmark")
        include(FetchContent)
        set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE INTERNAL "")
        # Do not build and run googlebenchmark tests
        FetchContent_Declare(googlebenchmark GIT_REPOSITORY https://github.com/google/benchmark.git GIT_TAG v1.5.2)

        FetchContent_MakeAvailable(googlebenchmark)
    else()
        message(FATAL_ERROR "GoogleBenchmark is missing. Use CMake >= 3.11 or download it")
    endif()
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

function(add_benchmark NAME)
  add_executable("${NAME}" "${NAME}.cpp")
  target_link_libraries("${NAME}" PRIVATE alpaca::alpaca benchmark::benchmark)
  target_compile_features("${NAME}" PRIVATE cxx_std_17)
  add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
  add_dependencies("run_${NAME}" "${NAME}")
endfunction()

add_benchmark(benchmark_log_10k_serialize)
add_benchmark(benchmark_log_10k_deserialize)
add_benchmark(benchmark_mesh_125k_serialize)
add_benchmark(benchmark_mesh_125k_deserialize)
add_benchmark(benchmark_minecraft_players_50_serialize)
add_benchmark(benchmark_minecraft_players_50_deserialize)