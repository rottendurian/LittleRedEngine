#ifndef lre_object_h
#define lre_object_h

#include "lre_window.h"
#include "lre_swapchain.h"
#include "lre_render.h"
#include "lre_instance.h"
#include "lre_device.h"
#include "lre_pipeline.h"
#include "lre_buffer.h"



//setup functions
#define createWindow(width,height,name) (lreCreateWindow)(width,height,name)
#define createInstance(engine_name,engine_version,app_name,app_version) lreCreateInstance(engine_name,app_name,engine_version,app_version)
#define createDebugMessenger(vulkanObject) (lreCreateDebugMessenger)(vulkanObject.instance)
#define createSurface(vulkanObject) (lreCreateSurface)(vulkanObject.instance,&vulkanObject.window)
#define pickPhysicalDevice(vulkanObject) (lrePickPhysicalDevice)(vulkanObject.instance,vulkanObject.surface)
#define createLogicalDevice(vulkanObject) (lreCreateLogicalDevice)(vulkanObject.physicalDevice,vulkanObject.surface)
#define getGraphicsQueue(vulkanObject) (lreGetGraphicsQueue)(vulkanObject.physicalDevice,vulkanObject.device,vulkanObject.surface)
#define getPresentQueue(vulkanObject) (lreGetPresentQueue)(vulkanObject.physicalDevice,vulkanObject.device,vulkanObject.surface)
#define createSwapChain(vulkanObject) (lreCreateSwapChain)(&vulkanObject.window,vulkanObject.surface,vulkanObject.device,vulkanObject.physicalDevice)
#define createImageViews(vulkanObject) (lreCreateSwapchainImageViews)(vulkanObject.device,&vulkanObject.lreSwapChain)
#define createRenderPass(vulkanObject) (lreCreateRenderPass)(vulkanObject.device,vulkanObject.physicalDevice,vulkanObject.lreSwapChain.surfaceFormat.format)
#define createGraphicsPipelineLayout(vulkanObject) (lreCreateGraphicsPipelineLayout)(vulkanObject.device,NULL)
#define createGraphicsPipeline(vulkanObject) (lreCreateGraphicsPipeline)(vulkanObject.device,vulkanObject.renderPass,vulkanObject.pipelineLayout)
#define createFrameBuffer(vulkanObject) (lreCreateFrameBuffer)(vulkanObject.device,vulkanObject.lreSwapChain,&vulkanObject.lreSwapChainImages,vulkanObject.renderPass)
#define createCommandPool(vulkanObject) (lreCreateCommandPool)(vulkanObject.physicalDevice,vulkanObject.device,vulkanObject.surface)
#define createCommandBuffer(vulkanObject) (lreCreateDrawCommandBuffer)(vulkanObject.device,vulkanObject.commandPool)
#define createSyncObjects(vulkanObject) (lreCreateSyncObjects)(vulkanObject.device)

//swapchain recreation


//cleanup functions
#define destroySyncObjects(vulkanObject) (lreDestroySyncObjects)(vulkanObject.device,vulkanObject.syncObjects)
#define destroyCommandBuffers(vulkanObject) (lreDestroyCommandBuffers)(vulkanObject.commandBuffer)
#define destroyCommandPool(vulkanObject) (lreDestroyCommandPool)(vulkanObject.device,vulkanObject.commandPool)
#define destroyFrameBuffer(vulkanObject) (lreDestroyFrameBuffer)(vulkanObject.device,vulkanObject.frameBuffer)
#define destroyGraphicsPipeline(vulkanObject) (lreDestroyGraphicsPipeline)(vulkanObject.device,vulkanObject.graphicsPipeline)
#define destroyGraphicsPipelineLayout(vulkanObject) (lreDestroyGraphicsPipelineLayout)(vulkanObject.device,vulkanObject.pipelineLayout)
#define destroyRenderPass(vulkanObject) (lreDestroyRenderPass)(vulkanObject.device,vulkanObject.renderPass)
#define destroyImageViews(vulkanObject) (lreDestroySwapchainImageViews)(vulkanObject.device,&vulkanObject.lreSwapChainImages)
#define destroySwapChain(vulkanObject) (lreDestroySwapChain)(vulkanObject.device,&vulkanObject.lreSwapChain)
#define destroySurface(vulkanObject) (lreDestroySurface)(vulkanObject.instance,vulkanObject.surface)
#define destroyDevice(vulkanObject) (lreDestroyDevice)(vulkanObject.device)
#define destroyDebugMessenger(vulkanObject) (lreDestroyDebugMessenger)(vulkanObject.instance,vulkanObject.debugger)
#define destroyInstance(vulkanObject) (lreDestroyInstance)(vulkanObject.instance)
#define destroyWindow(vulkanObject) (lreDestroyWindow)(&vulkanObject.window)

#endif