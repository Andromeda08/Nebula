cmake_minimum_required (VERSION 3.23)
set (CMAKE_CXX_STANDARD 20)

add_executable (ring_array_test ring_array_test.cpp ../nebula/common/RingArray.hpp)
target_include_directories (ring_array_test PRIVATE ..)