project(Schaf)

cmake_minimum_required(VERSION 3.1.0)

find_package(Boost REQUIRED)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

add_library(schafkopf STATIC RandomAi.h Schafkopf.h Schafkopf.cpp)
target_include_directories(schafkopf PUBLIC ${Boost_INCLUDE_DIRS})

add_executable(schafcli main.cpp)
target_link_libraries(schafcli schafkopf)

enable_testing()

aux_source_directory(gtest GTEST_SOURCES)
add_executable(schaftest test.cpp ${GTEST_SOURCES})
target_include_directories(schaftest PRIVATE gtest)
target_link_libraries(schaftest schafkopf)
add_test(NAME schaftest COMMAND schaftest)

set_property(TARGET schafkopf PROPERTY CXX_STANDARD 14)
set_property(TARGET schafcli PROPERTY CXX_STANDARD 14)
set_property(TARGET schaftest PROPERTY CXX_STANDARD 14)
