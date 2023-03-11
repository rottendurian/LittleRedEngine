#ifndef lre_window_h
#define lre_window_h

#include "pch.h"

typedef struct LreWindow {
    GLFWwindow* window;
    int width;
    int height;
    const char* name;
    int frameBufferResized;
} LreWindow; 

LreWindow lreCreateWindow(int width, int height, const char* name);
void lreDestroyWindow(LreWindow* LreWindow);

inline int lreWindowShouldClose(LreWindow* window) {return glfwWindowShouldClose(window->window);}

VkSurfaceKHR lreCreateSurface(VkInstance instance,LreWindow* window);
inline void lreDestroySurface(VkInstance instance,VkSurfaceKHR surface) {vkDestroySurfaceKHR(instance,surface,NULL);}

#endif