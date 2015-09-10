project(Schaf)

cmake_minimum_required(VERSION 3.1.0)

find_package(Boost REQUIRED)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

add_executable(schaf main.cpp Schafkopf.h)
target_include_directories(schaf PRIVATE ${Boost_INCLUDE_DIRS})

enable_testing()

aux_source_directory(gtest GTEST_SOURCES)
add_executable(schaftest test.cpp ${GTEST_SOURCES})
target_include_directories(schaftest PRIVATE ${Boost_INCLUDE_DIRS} gtest)
add_test(NAME schaftest COMMAND schaftest)

set_property(TARGET schaf PROPERTY CXX_STANDARD 14)
set_property(TARGET schaftest PROPERTY CXX_STANDARD 14)
