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

typedef struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR* formats;
    uint32_t formatCount;
    VkPresentModeKHR* presentModes;
    uint32_t presentModeCount;
} SwapChainSupportDetails;

SwapChainSupportDetails lreQuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

typedef struct LreSwapChainDetails {
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
    VkExtent2D extent;
} LreSwapChainDetails;

typedef struct LreSwapChain {
    VkSwapchainKHR swapChain;
    VkSurfaceFormatKHR surfaceFormat;
    VkExtent2D extent;
} LreSwapChain;

LreSwapChain lreCreateSwapChain(LreWindow* window,VkSurfaceKHR surface,VkDevice device,VkPhysicalDevice physicalDevice);
//void lreReCreateSwapChain();
static inline void lreDestroySwapChain(VkDevice device,LreSwapChain *swapChain) {vkDestroySwapchainKHR(device, swapChain->swapChain, NULL);}

typedef struct LreSwapChainImages {
    VkImage* images;
    uint32_t count;
    VkImageView* imageViews;
} LreSwapChainImages;

//LreSwapChainImages lreGetSwapChainImages(VkDevice device,VkSwapchainKHR swapChain);

LreSwapChainImages lreCreateImageViews(VkDevice device,LreSwapChain* swapChain);
static inline void lreDestroyImageViews(VkDevice device,LreSwapChainImages* images) {
    for (int i = 0; i < images->count; i++) {
        vkDestroyImageView(device, images->imageViews[i], NULL);
    } free(images->imageViews); free(images->images);
}

#include "lre_render.h"

#endif