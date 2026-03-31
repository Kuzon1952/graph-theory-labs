cmake_minimum_required(VERSION 3.14)
project(GraphLab11 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(graph_lab
    main.cpp
    graph.cpp
    distribution.cpp
    tree_generator.cpp
    eccentricity.cpp
    shimbell.cpp
    path_counter.cpp
)

if(MSVC)
    target_compile_options(graph_lab PRIVATE /W4)
else()
    target_compile_options(graph_lab PRIVATE -Wall -Wextra)
endif()
