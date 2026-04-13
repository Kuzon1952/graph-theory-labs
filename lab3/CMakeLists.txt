add_executable(lab3
    main.cpp
        #../lab2/dfs.cpp
        # ford_fulkerson.cpp   <- uncomment when you add it
    # min_cost_flow.cpp    <- uncomment when you add it
)

target_include_directories(lab3 PRIVATE
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(lab3 PRIVATE
    ${CMAKE_SOURCE_DIR}/shared/graph.cpp
    ${CMAKE_SOURCE_DIR}/shared/distribution.cpp
    ${CMAKE_SOURCE_DIR}/shared/dag_generator.cpp
)

if(MSVC)
    target_compile_options(lab3 PRIVATE /W4)
else()
    target_compile_options(lab3 PRIVATE -Wall -Wextra -pedantic)
endif()
