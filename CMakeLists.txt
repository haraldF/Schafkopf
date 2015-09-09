project(Schaf)

cmake_minimum_required(VERSION 3.1.0)

find_package(Boost REQUIRED)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

add_executable(schaf main.cpp Schafkopf.h)
target_include_directories(schaf PRIVATE ${Boost_INCLUDE_DIRS})

add_executable(schaftest test.cpp)
target_include_directories(schaftest PRIVATE ${Boost_INCLUDE_DIRS})

set_property(TARGET schaf PROPERTY CXX_STANDARD 14)
