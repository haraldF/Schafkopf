aux_source_directory(../gtest GTEST_SOURCES)

add_executable(schaftest test.cpp ${GTEST_SOURCES})
target_include_directories(schaftest PRIVATE ../gtest)
target_link_libraries(schaftest schafkopf)

add_test(NAME schaftest COMMAND schaftest)

set_property(TARGET schaftest PROPERTY CXX_STANDARD 14)
