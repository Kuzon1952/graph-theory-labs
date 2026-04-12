add_executable(lab1
        main.cpp
        eccentricity.cpp
        shimbell.cpp
        path_counter.cpp
)

target_include_directories(lab1 PRIVATE ${CMAKE_SOURCE_DIR}/shared)

target_sources(lab1 PRIVATE
        ${CMAKE_SOURCE_DIR}/shared/graph.cpp
        ${CMAKE_SOURCE_DIR}/shared/distribution.cpp
        ${CMAKE_SOURCE_DIR}/shared/dag_generator.cpp
)