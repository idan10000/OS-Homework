cmake_minimum_required(VERSION 3.19)
project(HW4 C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -std=c11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
add_executable(HW4 pfind.c)
target_link_libraries(HW4 Threads::Threads)

