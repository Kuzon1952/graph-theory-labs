cmake_minimum_required(VERSION 3.20)
project(Lab6Dictionary)

set(CMAKE_CXX_STANDARD 17)

add_executable(Lab6Dictionary
    main.cpp
    rbtree.cpp
    rbtree.h
    hashtable.cpp
    hashtable.h
    textutils.cpp
    textutils.h
)
