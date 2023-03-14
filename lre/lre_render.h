#ifndef lre_render_h
#define lre_render_h

#include "lre_device.h"
#include "lre_swapchain.h"
#include "lre_pipeline.h"
#include "lre_object.h"

#include "pch.h"

// static const int MAX_FRAMES_IN_FLIGHT = 2;

typedef struct LreFrameBuffer {
    VkFramebuffer* frame;
    size_t count;    
} LreFrameBuffer;

typedef struct LreDrawInfo {
    VkBuffer* vertexBuffers;
    VkDeviceSize* bufferOffsets;
    uint32_t bufferStartIndex;
    uint32_t bufferCount;

    VkBuffer indexBuffer;
    uint32_t indexCount;
    VkIndexType indexType;
    uint32_t indexOffset;

    VkDescriptorSet* descriptorSets;
    uint32_t descriptorStartSet;
    uint32_t descriptorCount;
} LreDrawInfo;

LreFrameBuffer lreCreateFrameBuffer(VkDevice device,LreSwapChain lreSwapChain,LreSwapChainImages* lreSwapChainImages,VkRenderPass renderPass);
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

typedef struct LreSynchronization {
    VkSemaphore* imageAvailableSemaphores;
    VkSemaphore* renderFinishedSemaphores;
    VkFence* inFlightFences;
} LreSynchronization;

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

//#include "lre_object.h"

typedef struct lreVulkanObject {
    LreWindow window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    LreSwapChain lreSwapChain;
    LreSwapChainImages lreSwapChainImages;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    LreFrameBuffer frameBuffer;
    VkCommandPool commandPool;
    VkCommandBuffer* commandBuffer;
    LreSynchronization syncObjects;
} lreVulkanObject;

void lreDrawFrame(lreVulkanObject *vulkanObject,uint32_t currentFrame,LreDrawInfo* drawInfo);

void lreRecordDrawCommandBuffer(lreVulkanObject* vulkanObject,uint32_t imageIndex,uint32_t currentFrame,LreDrawInfo* drawInfo);

void lreReCreateSwapChain(lreVulkanObject *vulkanObject);

#endif