#ifndef lre_render_h
#define lre_render_h

#include "pch.h"
#include "lre_device.h"
#include "lre_swapchain.h"
#include "lre_pipeline.h"
// #include "lre_buffer.h"
// #include "lre_object.h"

LreFrameBuffer lreCreateFrameBuffer(VkDevice device,LreSwapChain lreSwapChain,LreSwapChainImages* lreSwapChainImages,VkRenderPass renderPass,LreTextureObject depthImage,LreTextureObject colorImage);
static inline void lreDestroyFrameBuffer(VkDevice device,LreFrameBuffer frameBuffers) {
    for (size_t i = 0; i < frameBuffers.count; i++) {
        vkDestroyFramebuffer(device,frameBuffers.frame[i],NULL);
    } free(frameBuffers.frame);
}

VkCommandPool lreCreateCommandPool(VkPhysicalDevice physicalDevice,VkDevice device,VkSurfaceKHR surface);
static inline void lreDestroyCommandPool(VkDevice device,VkCommandPool commandPool) {vkDestroyCommandPool(device,commandPool,NULL);}

VkCommandBuffer lreBeginSingleTimeCommands(VkDevice device,VkCommandPool commandPool);
void lreEndSingleTimeCommands(VkDevice device,VkQueue graphicsQueue,VkCommandPool commandPool,VkCommandBuffer commandBuffer);

VkCommandBuffer* lreCreateDrawCommandBuffer(VkDevice device,VkCommandPool commandPool);
static inline void lreDestroyCommandBuffers(VkCommandBuffer* buffer) {
    free(buffer);
}

LreSynchronization lreCreateSyncObjects(VkDevice device);
static inline void lreDestroySyncObjects(VkDevice device,LreSynchronization syncObjects) {
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(device, syncObjects.imageAvailableSemaphores[i], NULL);
        vkDestroySemaphore(device, syncObjects.renderFinishedSemaphores[i], NULL);
        vkDestroyFence(device, syncObjects.inFlightFences[i], NULL);
    }
    free(syncObjects.imageAvailableSemaphores);
    free(syncObjects.renderFinishedSemaphores);
    free(syncObjects.inFlightFences);
}

void lreDrawFrame(LreVulkanObject *vulkanObject,uint32_t currentFrame,LreDrawInfo* drawInfo);

void lreRecordDrawCommandBuffer(LreVulkanObject* vulkanObject,uint32_t imageIndex,uint32_t currentFrame,LreDrawInfo* drawInfo);

void lreReCreateSwapChain(LreVulkanObject *vulkanObject);

#endif