cmake_minimum_required(VERSION 3.28)
project(lob LANGUAGES CXX)

# lang standards.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# editor support.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# optimization flags
# avoid overwritting a Debug Build.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# specify flags.
set(CMAKE_CXX_FLAGS_RELEASE "-03 -march=native -fno-omit-frame-pointer -DNDEBUG")

#warnings
add_compile_options( -Wall -Wextra -Wpedantic -Wconversion -Wshadow)

#sanitizers
option(LOB_TSAN "ThreadSanitizer build (Module 3)" OFF)
option(LOB_ASAN "AddressSanitizer + UBSan build" OFF)

#libraries and targets
find_package(benchmark REQUIRED)
find_package(GTest REQUIRED)
enable_testing()
include(GoogleTest)

add_executable(bench_noop bench/bench_noop.cpp)
target_link_libraries(bench_noop PRIVATE benchmark::benchmark_main)

add_executable(test_sanity tests/test_sanity.cpp)
target_link_libraries(test_sanity PRIVATE GTest::gtest_main)
gtest_discover_tests(test_sanity)




















