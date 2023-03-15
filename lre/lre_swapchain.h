#ifndef lre_swapchain_h
#define lre_swapchain_h

#include "lre_window.h"
#include "lre_device.h"

#include "pch.h"

static const uint32_t lreDeviceExtensionsSize = 1;
static const char* lreDeviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);

SwapChainSupportDetails lreQuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

LreSwapChain lreCreateSwapChain(LreWindow* window,VkSurfaceKHR surface,VkDevice device,VkPhysicalDevice physicalDevice);
//void lreReCreateSwapChain();
static inline void lreDestroySwapChain(VkDevice device,LreSwapChain *swapChain) {vkDestroySwapchainKHR(device, swapChain->swapChain, NULL);}



//LreSwapChainImages lreGetSwapChainImages(VkDevice device,VkSwapchainKHR swapChain);

LreSwapChainImages lreCreateSwapchainImageViews(VkDevice device,LreSwapChain* swapChain);
static inline void lreDestroySwapchainImageViews(VkDevice device,LreSwapChainImages* images) {
    for (int i = 0; i < images->count; i++) {
        vkDestroyImageView(device, images->imageViews[i], NULL);
    } free(images->imageViews); free(images->images);
}

#endif