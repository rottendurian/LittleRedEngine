#ifndef lre_device_h
#define lre_device_h

#include "lre_window.h"
#include "lre_instance.h"
#include "lre_swapchain.h"

#include "pch.h"

static inline bool QueueFamilyIndices_isComplete(QueueFamilyIndices* data) {
    return data->graphicsFamily.has_value && data->presentFamily.has_value;
}

VkPhysicalDevice lrePickPhysicalDevice(VkInstance instance,VkSurfaceKHR surface);

VkDevice lreCreateLogicalDevice(VkPhysicalDevice physicalDevice,VkSurfaceKHR surface);
inline void lreDestroyDevice(VkDevice device) {vkDestroyDevice(device,NULL);}

QueueFamilyIndices lreFindQueueFamilies(VkPhysicalDevice physicalDevice,VkSurfaceKHR surface);

VkQueue lreGetGraphicsQueue(VkPhysicalDevice physicalDevice,VkDevice device,VkSurfaceKHR surface);
VkQueue lreGetPresentQueue(VkPhysicalDevice physicalDevice,VkDevice device,VkSurfaceKHR surface);

VkSampleCountFlagBits lreGetMaxUsableSampleCount(VkPhysicalDevice physicalDevice);

#endif
