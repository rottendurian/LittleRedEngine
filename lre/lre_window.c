#include "lre_window.h"

#ifndef lre_window_c
#define lre_window_c

static void frameBufferResizeCallBack(GLFWwindow* window, int width, int height) {
    LreWindow* lreWindow = (LreWindow*)glfwGetWindowUserPointer(window);
    //printf("Memory Address %lu",lreWindow);
    lreWindow->frameBufferResized = 1;
}

LreWindow lreCreateWindow(int width, int height, const char* name) {
    LreWindow lreWindow;
    lreWindow.width = width;
    lreWindow.height = height;
    lreWindow.name = name;
    lreWindow.frameBufferResized = 0;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    lreWindow.window = glfwCreateWindow(width,height,name,NULL,NULL);

    glfwSetFramebufferSizeCallback(lreWindow.window,frameBufferResizeCallBack);
    return lreWindow;
}

VkSurfaceKHR lreCreateSurface(VkInstance instance,LreWindow* window) {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance,window->window,NULL,&surface) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create vulkan surface");
        // fprintf(stderr,"Failed to create vulkan surface");
        // exit(EXIT_FAILURE);
    }
    return surface;
}

void lreDestroyWindow(LreWindow* window) {
    glfwDestroyWindow(window->window);
}

#endif