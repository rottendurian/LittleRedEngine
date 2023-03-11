
add_library(lre 
    lre_window.c
    lre_instance.c
    lre_device.c
    lre_swapchain.c
    lre_pipeline.c
    lre_render.c
    lre_buffer.c
)

target_include_directories(lre
    PUBLIC ../dep/glfw/include
    PUBLIC ../dep/glm
    PUBLIC ../dep/lreutils
    PUBLIC ../dep/cglm/include/cglm
    PUBLIC ../dep/stb
)

target_include_directories(lre 
    PUBLIC ${Vulkan_INCLUDE_DIRS}
)

