#include "lre_window.h"

#ifndef lre_window_c
#define lre_window_c

static void frameBufferResizeCallBack(GLFWwindow* window, int width, int height) {
    LreWindow* lreWindow = (LreWindow*)glfwGetWindowUserPointer(window);
    //printf("Memory Address %lu",lreWindow);
    lreWindow->frameBufferResized = 1;
}

static void mouseMoveCallBack(GLFWwindow* window,double xPosIn,double yPosIn) {
    LreWindow* lreWindow = (LreWindow*)glfwGetWindowUserPointer(window);

    float xPos = (float)(xPosIn);
    float yPos = (float)(yPosIn);

    if (lreWindow->mouseOffsets.firstMouse)
    {
        lreWindow->mouseOffsets.lastX = xPos;
        lreWindow->mouseOffsets.lastY = yPos;
        lreWindow->mouseOffsets.offsetX = 0;
        lreWindow->mouseOffsets.offsetY = 0;
        lreWindow->mouseOffsets.firstMouse = false;
    }

    lreWindow->mouseOffsets.offsetX += xPos - lreWindow->mouseOffsets.lastX;
    lreWindow->mouseOffsets.offsetY += lreWindow->mouseOffsets.lastY - yPos;

    lreWindow->mouseOffsets.lastX = xPos;
    lreWindow->mouseOffsets.lastY = yPos;

}

static void scrollWheelCallBack(GLFWwindow* window, double xPos, double yPos) {
    LreWindow* lreWindow = (LreWindow*)glfwGetWindowUserPointer(window);

    lreWindow->scrollOffsets.offsetY += yPos;

}

LreWindow lreCreateWindow(int width, int height, const char* name) {
    LreWindow lreWindow; memset(&lreWindow,0,sizeof(lreWindow));
    lreWindow.width = width;
    lreWindow.height = height;
    lreWindow.name = name;
    lreWindow.frameBufferResized = 0;
    lreWindow.mouseOffsets.firstMouse = true;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API,GLFW_NO_API);
    lreWindow.window = glfwCreateWindow(width,height,name,NULL,NULL);

    glfwSetFramebufferSizeCallback(lreWindow.window,frameBufferResizeCallBack);
    glfwSetCursorPosCallback(lreWindow.window,mouseMoveCallBack);
    glfwSetScrollCallback(lreWindow.window,scrollWheelCallBack);

    glfwSetInputMode(lreWindow.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

void lreWindowGetMouseOffsets(LreWindow* window,float* offsetX,float* offsetY) {
    *offsetX = window->mouseOffsets.offsetX;
    *offsetY = window->mouseOffsets.offsetY;

    window->mouseOffsets.offsetX = 0.0f;
    window->mouseOffsets.offsetY = 0.0f;
}

void lreWindowGetScrollOffsets(LreWindow* window, float* offsetY) {
    *offsetY = window->scrollOffsets.offsetY;
    window->scrollOffsets.offsetY = 0.0f;
}

void lreDestroyWindow(LreWindow* window) {
    glfwDestroyWindow(window->window);
}

#endif