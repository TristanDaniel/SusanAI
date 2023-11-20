cmake_minimum_required(VERSION 3.26)
project(SusanAI)

set(CMAKE_CXX_STANDARD 20)

add_executable(SusanAI "SusanAI.cpp" "SusanAI.h" "Nodes.h" "Synapses.h" "Synapses.cpp" "Nodes.cpp" "Networks.h"
        Structures.h
        Utils.h
        LemonDrop.h
        Handlers.h
        Handlers.cpp
        Structures.cpp
        LemonDrop.cpp
)
