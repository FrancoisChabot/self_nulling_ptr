cmake_minimum_required(VERSION 3.1)
project(weak_ref)

include_directories(include)

add_executable(simple examples/simple.cpp)
set_property(TARGET simple PROPERTY CXX_STANDARD 14)
