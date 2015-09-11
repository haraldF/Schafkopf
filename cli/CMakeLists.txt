add_executable(schafcli main.cpp)
target_link_libraries(schafcli schafkopf)
set_property(TARGET schafcli PROPERTY CXX_STANDARD 14)
