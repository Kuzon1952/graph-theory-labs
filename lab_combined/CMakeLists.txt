add_executable(lab_combined
        main.cpp
        # Lab 1
        ${CMAKE_SOURCE_DIR}/lab1/eccentricity.cpp
        ${CMAKE_SOURCE_DIR}/lab1/shimbell.cpp
        ${CMAKE_SOURCE_DIR}/lab1/path_counter.cpp
        # Lab 2
        ${CMAKE_SOURCE_DIR}/lab2/dfs.cpp
        ${CMAKE_SOURCE_DIR}/lab2/floyd_warshall.cpp
        # Lab 3
        ${CMAKE_SOURCE_DIR}/lab3/ford_fulkerson.cpp
        ${CMAKE_SOURCE_DIR}/lab3/min_cost_flow.cpp
        # Lab 4
        ${CMAKE_SOURCE_DIR}/lab4/boruvka.cpp
        ${CMAKE_SOURCE_DIR}/lab4/edge_cover.cpp
        ${CMAKE_SOURCE_DIR}/lab4/kirchhoff.cpp
        ${CMAKE_SOURCE_DIR}/lab4/prufer.cpp
)

target_include_directories(lab_combined PRIVATE
        ${CMAKE_SOURCE_DIR}/shared
        ${CMAKE_SOURCE_DIR}/lab1
        ${CMAKE_SOURCE_DIR}/lab2
        ${CMAKE_SOURCE_DIR}/lab3
        ${CMAKE_SOURCE_DIR}/lab4
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(lab_combined PRIVATE
        ${CMAKE_SOURCE_DIR}/shared/graph.cpp
        ${CMAKE_SOURCE_DIR}/shared/distribution.cpp
        ${CMAKE_SOURCE_DIR}/shared/dag_generator.cpp
        ${CMAKE_SOURCE_DIR}/shared/weight_matrix.cpp
)

if(MSVC)
    target_compile_options(lab_combined PRIVATE /W4)
else()
    target_compile_options(lab_combined PRIVATE -Wall -Wextra -pedantic)
endif()
