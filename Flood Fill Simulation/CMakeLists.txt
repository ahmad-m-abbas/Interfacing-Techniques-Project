cmake_minimum_required(VERSION 3.20)
project(Flood_Fill_Simulation)

set(CMAKE_CXX_STANDARD 11)

add_executable(Flood_Fill_Simulation main.cpp Cell.h Coordinates.h Maze.h API.cpp API.h MazeGraph.h)
