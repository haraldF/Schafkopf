project(Schaf)

cmake_minimum_required(VERSION 3.1.0)

find_package(Boost REQUIRED)

add_executable(schaf main.cpp)
target_include_directories(schaf PRIVATE ${Boost_INCLUDE_DIRS})

set_property(TARGET schaf PROPERTY CXX_STANDARD 14)
