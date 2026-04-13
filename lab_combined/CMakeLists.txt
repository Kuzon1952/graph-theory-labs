add_executable(lab_combined
    main.cpp
    # Lab 1
    ${CMAKE_SOURCE_DIR}/lab1/eccentricity.cpp
    ${CMAKE_SOURCE_DIR}/lab1/shimbell.cpp
    ${CMAKE_SOURCE_DIR}/lab1/path_counter.cpp
        ../lab2/dfs.cpp
        ../lab2/dfs.cpp
        # Lab 2 — uncomment when ready
     ${CMAKE_SOURCE_DIR}/lab2/dfs.cpp
     ${CMAKE_SOURCE_DIR}/lab2/floyd_warshall.cpp
    # Lab 3 — uncomment when ready
    # ${CMAKE_SOURCE_DIR}/lab3/ford_fulkerson.cpp
    # ${CMAKE_SOURCE_DIR}/lab3/min_cost_flow.cpp
)

target_include_directories(lab_combined PRIVATE
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/lab1
    ${CMAKE_SOURCE_DIR}/lab2
    ${CMAKE_SOURCE_DIR}/lab3
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(lab_combined PRIVATE
    ${CMAKE_SOURCE_DIR}/shared/graph.cpp
    ${CMAKE_SOURCE_DIR}/shared/distribution.cpp
    ${CMAKE_SOURCE_DIR}/shared/dag_generator.cpp
)

if(MSVC)
    target_compile_options(lab_combined PRIVATE /W4)
else()
    target_compile_options(lab_combined PRIVATE -Wall -Wextra -pedantic)
endif()
