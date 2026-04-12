add_executable(lab2
    main.cpp
    # dfs.cpp          <- uncomment when you add it
    # floyd_warshall.cpp  <- uncomment when you add it
)

target_include_directories(lab2 PRIVATE
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(lab2 PRIVATE
    ${CMAKE_SOURCE_DIR}/shared/graph.cpp
    ${CMAKE_SOURCE_DIR}/shared/distribution.cpp
    ${CMAKE_SOURCE_DIR}/shared/dag_generator.cpp
)

if(MSVC)
    target_compile_options(lab2 PRIVATE /W4)
else()
    target_compile_options(lab2 PRIVATE -Wall -Wextra -pedantic)
endif()
