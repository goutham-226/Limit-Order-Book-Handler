cmake_minimum_required(VERSION 3.28)
project(lob LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # for clangd / your editor

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Release = what you benchmark. Frame pointers keep `perf record -g` usable.
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -fno-omit-frame-pointer -DNDEBUG")

# Google Benchmark built from source in Release (Ubuntu's package is a DEBUG build).
# Fetched BEFORE add_compile_options so our strict warnings don't apply to its code.
include(FetchContent)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.9.1
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(benchmark)

add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)

# Sanitizer builds: never benchmark these, only test with them.
option(LOB_TSAN "ThreadSanitizer build (Module 3)" OFF)
option(LOB_ASAN "AddressSanitizer + UBSan build" OFF)
if(LOB_TSAN AND LOB_ASAN)
  message(FATAL_ERROR "TSan and ASan cannot be combined")
endif()
if(LOB_TSAN)
  add_compile_options(-fsanitize=thread -g -O1)
  add_link_options(-fsanitize=thread)
endif()
if(LOB_ASAN)
  add_compile_options(-fsanitize=address,undefined -g -O1)
  add_link_options(-fsanitize=address,undefined)
endif()

find_package(GTest REQUIRED)
enable_testing()
include(GoogleTest)

# --- Module 0: rig check ---------------------------------------------------
add_executable(bench bench/bench.cpp)
target_link_libraries(bench PRIVATE benchmark::benchmark_main)

add_executable(test_sanity tests/test_sanity.cpp)
target_link_libraries(test_sanity PRIVATE GTest::gtest_main)
gtest_discover_tests(test_sanity)
