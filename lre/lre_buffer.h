#ifndef lre_buffer_h
#define lre_buffer_h

#include "pch.h"

typedef struct LreBufferObject {
    VkBuffer buffer;
    VkDeviceMemory memory;
    VkDeviceSize size;
} LreBufferObject;

typedef struct LreUniformBufferObject {
    VkBuffer buffer;
    VkDeviceMemory memory;
    VkDeviceSize size;
    void* map;
} LreUniformBufferObject;

typedef struct LreTextureImageObject {
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
    VkSampler sampler;
} LreTextureImageObject;

LreUniformBufferObject lreCreateUniformBuffer(VkDevice device,VkPhysicalDevice physicalDevice,VkDeviceSize size);
static inline void lreDestroyUniformBuffer(VkDevice device,LreUniformBufferObject uniform) {
    if (uniform.map) {
        vkUnmapMemory(device, uniform.memory);
        uniform.map = NULL;
    }
    vkDestroyBuffer(device, uniform.buffer, NULL);
    vkFreeMemory(device, uniform.memory, NULL);
}


LreBufferObject lreCreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

LreBufferObject lreCreateBufferStaged(VkDevice device, VkPhysicalDevice physicalDevice,VkCommandPool commandPool,VkQueue graphicsQueue, const void* data, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

static inline void lreMapMemory(VkDevice device,VkDeviceMemory deviceMemory,const void* data,size_t size) {
    void* map;
#ifdef NDEBUG
    vkMapMemory(device,deviceMemory,0,size,0,&map);
#else
    VkResult result = vkMapMemory(device,deviceMemory,0,size,0,&map);
    if (result != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to map uniform buffer object");
        // fprintf(stderr,"Failed to map uniform buffer object");
        // exit(EXIT_FAILURE);
    }
#endif
        memcpy(map,data,size);
    vkUnmapMemory(device,deviceMemory);
}

void lreCopyBuffer(VkDevice device,VkCommandPool commandPool,VkQueue graphicsQueue,VkBuffer src, VkBuffer dst,VkDeviceSize size);

static inline void lreDestroyBuffer(VkDevice device,LreBufferObject buffer) {
    vkDestroyBuffer(device,buffer.buffer,NULL);
    vkFreeMemory(device,buffer.memory,NULL);
}

LreTextureImageObject lreCreateTextureImage(VkDevice device,VkPhysicalDevice physicalDevice,VkImageCreateInfo imageInfo);
void lreTransitionImageLayout(VkDevice device,VkCommandPool commandPool,VkQueue graphicsQueue,VkImage image,VkFormat format,VkImageLayout oldLayout,VkImageLayout newLayout);
void lreCopyBufferToImage(VkDevice device,VkCommandPool commandPool,VkQueue graphicsQueue,VkBuffer buffer,VkImage image,uint32_t width,uint32_t height);
LreTextureImageObject lreCreateTextureImage2D(VkDevice device,VkPhysicalDevice physicalDevice,VkCommandPool commandPool,VkQueue graphicsQueue,const char* filepath);

VkImageView lreCreateImageView2D(VkDevice device,VkImage image,VkFormat format);
VkSampler lreCreateSampler2D(VkDevice device,VkPhysicalDevice physicalDevice,VkFilter mFilter,VkSamplerAddressMode samplerAddressMode);

static inline void lreDestroyTextureImage(VkDevice device, LreTextureImageObject texture) {
    vkDestroySampler(device,texture.sampler,NULL);
    vkDestroyImageView(device,texture.view,NULL);
    vkDestroyImage(device,texture.image,NULL);
    vkFreeMemory(device,texture.memory,NULL);
}

#endif