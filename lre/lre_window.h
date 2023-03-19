#ifndef lre_window_h
#define lre_window_h

#include "pch.h"

LreWindow lreCreateWindow(int width, int height, const char* name);
void lreDestroyWindow(LreWindow* LreWindow);

inline int lreWindowShouldClose(LreWindow* window) {return glfwWindowShouldClose(window->window);}
void lreWindowGetMouseOffsets(LreWindow* window,float* offsetX,float* offsetY);
void lreWindowGetScrollOffsets(LreWindow* window, float* offsetY);

VkSurfaceKHR lreCreateSurface(VkInstance instance,LreWindow* window);
inline void lreDestroySurface(VkInstance instance,VkSurfaceKHR surface) {vkDestroySurfaceKHR(instance,surface,NULL);}

#endif