#ifndef lre_instance_h
#define lre_instance_h

#include "pch.h"

#ifdef NDEBUG
    static const bool enableValidationLayers = false;
#else
    static const bool enableValidationLayers = true;
#endif



static const int validationLayersSize = 1;
static const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};

VkInstance lreCreateInstance(const char *engineName,const char* appName,unsigned int engineVersion,unsigned int appVersion);
void lreDestroyInstance(VkInstance instance);

VkDebugUtilsMessengerEXT lreCreateDebugMessenger(VkInstance instance);
void lreDestroyDebugMessenger(VkInstance instance,VkDebugUtilsMessengerEXT debugMessenger);

#endif

