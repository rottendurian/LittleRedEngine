#ifndef lre_main_c
#define lre_main_c

#include "lre_main.h"

LreVulkanContext* LreGenerateVulkanContext(uint32_t windowWidth, uint32_t windowHeight, const char* windowName) {
    LreVulkanContext *vulkanContext = (LreVulkanContext*)malloc(sizeof(LreVulkanContext));
    
    vulkanContext->vulkanObject.window = createWindow(windowWidth,windowHeight,windowName);     
    vulkanContext->vulkanObject.instance = createInstance("[LRE] Little Red Engine",VK_MAKE_VERSION(1,0,0),"application",VK_MAKE_VERSION(1,0,0));
    vulkanContext->vulkanObject.debugger = createDebugMessenger(vulkanObject);
    
    vulkanContext->vulkanObject.surface = createSurface(vulkanObject);
    vulkanContext->vulkanObject.physicalDevice = pickPhysicalDevice(vulkanObject);

    vulkanContext->vulkanObject.device = createLogicalDevice(vulkanObject);
    vulkanContext->vulkanObject.graphicsQueue = getGraphicsQueue(vulkanObject);
    vulkanContext->vulkanObject.presentQueue = getPresentQueue(vulkanObject);
    
    vulkanContext->vulkanObject.lreSwapChain = createSwapChain(vulkanObject);
    vulkanContext->vulkanObject.lreSwapChainImages = createImageViews(vulkanObject);
    vulkanContext->vulkanObject.renderPass = createRenderPass(vulkanObject);

    return vulkanContext;

}



#endif