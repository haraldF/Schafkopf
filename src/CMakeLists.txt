add_library(schafkopf STATIC RandomAi.h ObserverAi.h Schafkopf.h Schafkopf.cpp)
target_include_directories(schafkopf
    PUBLIC ${Boost_INCLUDE_DIRS}
    PUBLIC .)
set_property(TARGET schafkopf PROPERTY CXX_STANDARD 14)
