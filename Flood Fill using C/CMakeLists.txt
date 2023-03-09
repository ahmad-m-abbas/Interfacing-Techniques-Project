cmake_minimum_required(VERSION 3.20)
project(Flood Fill cpp)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(ported_micromouse
        src/motion.cpp
        src/motion.h
        src/motors.cpp
        src/motors.h
        src/mouse.cpp
        src/mouse.h
        src/navigator.cpp
        src/navigator.h
        src/nullserial.h
        src/parameters.h
        src/ported.cpp
        src/queue.h
        src/sensors.cpp
        src/sensors.h
        src/streaming.h
        src/systick.cpp
        src/systick.h
        src/test.cpp
        src/test.h
        src/ui.cpp
        src/ui.h
        src/volatiles.h
        src/maze.h
        src/maze.cpp
        src/API.cpp
        src/API.h)
