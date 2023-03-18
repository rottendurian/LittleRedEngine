#ifndef lre_swapchain_c
#define lre_swapchain_c

#include "lre_swapchain.h"

bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, NULL);

    VkExtensionProperties* availableExtensions = (VkExtensionProperties*)alloca(extensionCount*sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(physicalDevice, NULL, &extensionCount, availableExtensions);

    for (int i = 0; i < extensionCount; i++) {
        if (strcmp(lreDeviceExtensions[0],availableExtensions[i].extensionName) != false) {
            return true;
        } 
    }

    return false;
}

static inline VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR* availableFormats,uint32_t formatCount) {
    for(int i = 0; i < formatCount; i++) {
        VkSurfaceFormatKHR availableFormat = availableFormats[i];
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }
    
    return availableFormats[0];
}

static inline VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR* availablePresentModes,uint32_t presentModeCount) {
    for (int i = 0; i < presentModeCount; i++) {
        if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentModes[i];
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

static inline uint32_t clamp(uint32_t value,uint32_t min,uint32_t max) {
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}

static inline VkExtent2D chooseSwapExtent(LreWindow *window,const VkSurfaceCapabilitiesKHR* capabilities) {
    if (capabilities->currentExtent.width != UINT32_MAX) {
        return capabilities->currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window->window, &width, &height);

        VkExtent2D actualExtent = {
            (uint32_t)(width),
            (uint32_t)(height)
        };
        
        actualExtent.width = clamp(actualExtent.width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
        actualExtent.height = clamp(actualExtent.height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);

        return actualExtent;
    }
}

SwapChainSupportDetails lreQuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
    SwapChainSupportDetails details; memset(&details,0,sizeof(details));

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, NULL);

    if (formatCount != 0) {
        details.formats = (VkSurfaceFormatKHR*)malloc(formatCount*sizeof(VkSurfaceFormatKHR));
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats);
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, NULL);

    if (presentModeCount != 0) {
        details.presentModes = (VkPresentModeKHR*)malloc(presentModeCount*sizeof(VkPresentModeKHR));
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes);
    }

    details.formatCount = formatCount;
    details.presentModeCount = presentModeCount;

    return details;
}

LreSwapChain lreCreateSwapChain(LreWindow* window,VkSurfaceKHR surface,VkDevice device,VkPhysicalDevice physicalDevice) {
    SwapChainSupportDetails swapChainSupport = lreQuerySwapChainSupport(physicalDevice,surface);
    
    LreSwapChainDetails swapChain;

    swapChain.surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats,swapChainSupport.formatCount);
    swapChain.presentMode = chooseSwapPresentMode(swapChainSupport.presentModes,swapChainSupport.presentModeCount);
    swapChain.extent = chooseSwapExtent(window,&swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo; memset(&createInfo,0,sizeof(createInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = swapChain.surfaceFormat.format;
    createInfo.imageColorSpace = swapChain.surfaceFormat.colorSpace;
    createInfo.imageExtent = swapChain.extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = lreFindQueueFamilies(physicalDevice,surface);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value, indices.presentFamily.value};

    if (indices.graphicsFamily.value != indices.presentFamily.value) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = NULL; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = swapChain.presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapChainKHR;

    if (vkCreateSwapchainKHR(device, &createInfo, NULL, &swapChainKHR) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create swap chain");
        // fprintf(stderr,"Failed to create swap chain");
        // exit(EXIT_FAILURE);
    }

    free(swapChainSupport.formats);
    free(swapChainSupport.presentModes);

    LreSwapChain lreSwapChain = {swapChainKHR,swapChain.surfaceFormat,swapChain.extent};

    return lreSwapChain;
}

#include "lre_buffer.h"
#include "lre_render.h"

static inline void lreCleanUpSwapChain(LreVulkanObject* vulkanObject) {
    lreDestroyFrameBuffer(vulkanObject->device,vulkanObject->frameBuffer);
    lreDestroySwapchainImageViews(vulkanObject->device,&vulkanObject->lreSwapChainImages);
    lreDestroySwapChain(vulkanObject->device,&vulkanObject->lreSwapChain);
    lreDestroyTexture(vulkanObject->device,vulkanObject->depthImage);
}

void lreReCreateSwapChain(LreVulkanObject *vulkanObject) {
    //printf("resizing");
    int width = 0, height = 0;
    glfwGetFramebufferSize(vulkanObject->window.window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(vulkanObject->window.window, &width, &height);
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(vulkanObject->device);
    lreCleanUpSwapChain(vulkanObject);

    vulkanObject->lreSwapChain = lreCreateSwapChain(&vulkanObject->window,vulkanObject->surface,vulkanObject->device,vulkanObject->physicalDevice);
    vulkanObject->lreSwapChainImages = lreCreateSwapchainImageViews(vulkanObject->device,&vulkanObject->lreSwapChain);
    vulkanObject->depthImage = lreCreateDepthResources(vulkanObject->device,vulkanObject->physicalDevice,vulkanObject->lreSwapChain);
    vulkanObject->frameBuffer = lreCreateFrameBuffer(vulkanObject->device,vulkanObject->lreSwapChain,&vulkanObject->lreSwapChainImages,vulkanObject->renderPass,vulkanObject->depthImage);
}

static inline LreSwapChainImages lreGetSwapChainImages(VkDevice device,VkSwapchainKHR swapChain) {
    LreSwapChainImages swapChainImages;

    vkGetSwapchainImagesKHR(device, swapChain, &swapChainImages.count, NULL);
    swapChainImages.images = (VkImage*)malloc(swapChainImages.count*sizeof(VkImage));
    vkGetSwapchainImagesKHR(device, swapChain, &swapChainImages.count, swapChainImages.images);

    return swapChainImages;
}

LreSwapChainImages lreCreateSwapchainImageViews(VkDevice device,LreSwapChain* swapChain) {
    LreSwapChainImages images = lreGetSwapChainImages(device,swapChain->swapChain);

    images.imageViews = (VkImageView*)malloc(images.count*sizeof(VkImageView));
    //VkImageView* imageViews = (VkImageView*)malloc(images->count*sizeof(VkImageView));

    for (int i = 0; i < images.count; i++) {

        images.imageViews[i] = lreCreateImageView2D(device,images.images[i],swapChain->surfaceFormat.format,1);

        // VkImageViewCreateInfo createInfo; memset(&createInfo,0,sizeof(createInfo));
        // createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        // createInfo.image = images.images[i];

        // createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        // createInfo.format = swapChain->surfaceFormat.format;

        // createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        // createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        // createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        // createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        // createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        // createInfo.subresourceRange.baseMipLevel = 0;
        // createInfo.subresourceRange.levelCount = 1;
        // createInfo.subresourceRange.baseArrayLayer = 0;
        // createInfo.subresourceRange.layerCount = 1;

        // if (vkCreateImageView(device, &createInfo, NULL, &images.imageViews[i]) != VK_SUCCESS) {
        //     LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create image views!");
        //     // exit(EXIT_FAILURE);
        // }
    }
    
    return images;
}




#endif