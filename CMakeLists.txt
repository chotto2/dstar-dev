cmake_minimum_required(VERSION 3.16)
project(dstar-dev C)

set(CMAKE_C_STANDARD 11)

add_executable(dstar src/dstar.c)
target_include_directories(dstar PRIVATE src)
target_link_libraries(dstar gmp m)
