cmake_minimum_required(VERSION 3.22.0)

project(lreutils)

add_library(${PROJECT_NAME} STATIC)

add_subdirectory(bst)
add_subdirectory(rbtree)
add_subdirectory(hashtable)
add_subdirectory(memory)
add_subdirectory(logging)

target_link_libraries(${PROJECT_NAME}
    bst
    rbtree
    hashtable
    memory
    logging
)

target_include_directories(${PROJECT_NAME}
    PUBLIC bst
    PUBLIC rbtree
    PUBLIC hashtable
    PUBLIC memory
    PUBLIC logging
)


# Add the source files from the subdirectories to the library target
target_sources(${PROJECT_NAME} PRIVATE
    $<TARGET_OBJECTS:bst>
    $<TARGET_OBJECTS:rbtree>
    $<TARGET_OBJECTS:hashtable>
    $<TARGET_OBJECTS:memory>
    $<TARGET_OBJECTS:logging>
)
