cmake_minimum_required(VERSION 4.2)
project()

set(CMAKE_CXX_STANDARD 20)

target_include_directories()

add_executable(main.cpp
        src/midievents.hpp
        src/midievents.cpp
        src/midi_parser.hpp)
