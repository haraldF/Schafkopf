project(Schaf)

cmake_minimum_required(VERSION 3.3.0)

find_package(Boost REQUIRED)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

### seems that emscripten doesn't handle the C++14 flag properly. Force set it.
if(EMSCRIPTEN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
endif()

enable_testing()

add_subdirectory(src)
add_subdirectory(cli)
add_subdirectory(tests)
