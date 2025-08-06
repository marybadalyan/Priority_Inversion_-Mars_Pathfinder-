cmake_minimum_required(VERSION 3.10)

project(ThreadSafeCounter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(w_priority_inheritence w_priority_inheritence.cpp)
add_executable(W_priority_fault w_priority_fault.cpp )

add_executable(l_priority_inheritence l_priority_inheritence.cpp)
add_executable(l_priority_fault l_priority_fault.cpp )
