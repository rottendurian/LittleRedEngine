cmake_minimum_required(VERSION 3.22.0)

project(vulkan)

# set(CMAKE_CXX_STANARD 17)

# if(MSVC)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
# endif(MSVC)

add_executable(${PROJECT_NAME} main.c)

target_include_directories(${PROJECT_NAME}
    PUBLIC dep/glfw/include
    PUBLIC dep/glm
    PUBLIC dep/cglm/include
    PUBLIC lre
    PUBLIC dep/lreutils
)

find_package(Vulkan REQUIRED)

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${Vulkan_INCLUDE_DIRS}
)

add_subdirectory(dep/glfw)

add_subdirectory(dep/cglm)

add_subdirectory(dep/lreutils)

add_subdirectory(lre)

target_link_libraries(${PROJECT_NAME} 
    Vulkan::Vulkan
)

target_link_libraries(${PROJECT_NAME}
    glfw
    cglm
    lre
    lreutils
)
