#ifndef lre_buffer_c
#define lre_buffer_c

#include "lre_buffer.h"
#include "lre_render.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

static inline uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties,VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    LOGTOFILE(LOG_LEVEL_ERROR,"[lreBuffer] unable to find suitable memory");
    // fprintf(stderr,"[lreBuffer] unable to find suitable memory");
    exit(EXIT_FAILURE);
}

LreUniformBufferObject lreCreateUniformBuffer(VkDevice device,VkPhysicalDevice physicalDevice,VkDeviceSize size) {
    LreBufferObject buffer = lreCreateBuffer(device,physicalDevice,size,VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    LreUniformBufferObject uniform;
    uniform.buffer = buffer.buffer;
    uniform.memory = buffer.memory;
    uniform.size = buffer.size;
#ifdef NDEBUG
    vkMapMemory(device,uniform.memory,0,size,0,&uniform.map);
#else
    VkResult result = vkMapMemory(device,uniform.memory,0,size,0,&uniform.map);
    if (result != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to map uniform buffer object");
        // fprintf(stderr,"Failed to map uniform buffer object");
        // exit(EXIT_FAILURE);
    }
#endif
    return uniform;
}

LreBufferObject lreCreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    
    VkBufferCreateInfo bufferInfo; memset(&bufferInfo,0,sizeof(bufferInfo));
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, NULL, &buffer) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create buffer!\n");
        // fprintf(stderr,"Failed to create buffer!\n");
        // exit(EXIT_FAILURE);
    }

    VkMemoryRequirements memRequirements; memset(&memRequirements,0,sizeof(memRequirements));
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo; memset(&allocInfo,0,sizeof(allocInfo));
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties,physicalDevice);

    if (vkAllocateMemory(device, &allocInfo, NULL, &bufferMemory) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to allocate buffer memory!\n");
        // fprintf(stderr,"Failed to allocate buffer memory!\n");
        // exit(EXIT_FAILURE);
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);

    return (LreBufferObject){buffer,bufferMemory,size};
}

void lreCopyBuffer(VkDevice device,VkCommandPool commandPool,VkQueue graphicsQueue,VkBuffer src, VkBuffer dst,VkDeviceSize size) {

    VkCommandBuffer commandBuffer = lreBeginSingleTimeCommands(device,commandPool);

    VkBufferCopy copyRegion; memset(&copyRegion,0,sizeof(copyRegion));
    copyRegion.srcOffset = 0; 
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

    lreEndSingleTimeCommands(device,graphicsQueue,commandPool,commandBuffer);

}

LreBufferObject lreCreateBufferStaged(VkDevice device, VkPhysicalDevice physicalDevice,VkCommandPool commandPool,VkQueue graphicsQueue,const void* data,VkDeviceSize size,VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    LreBufferObject stagingBuffer = lreCreateBuffer(device,physicalDevice,size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    lreMapMemory(device,stagingBuffer.memory,data,size);

    LreBufferObject buffer = lreCreateBuffer(device,physicalDevice,size,
        usage,properties);
    //copy
    lreCopyBuffer(device,commandPool,graphicsQueue,stagingBuffer.buffer,buffer.buffer,size);
    //cleanup stage buffer
    lreDestroyBuffer(device,stagingBuffer);

    return buffer;
}

LreTextureImageObject lreCreateTextureImage(VkDevice device,VkPhysicalDevice physicalDevice,VkImageCreateInfo imageInfo) {
    LreTextureImageObject textureImage;

    if (vkCreateImage(device, &imageInfo, NULL, &textureImage.image) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"failed to create image!");
        // fprintf(stderr,"failed to create image!");
        // exit(EXIT_FAILURE);
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, textureImage.image, &memRequirements);

    VkMemoryAllocateInfo allocInfo; memset(&allocInfo,0,sizeof(allocInfo));
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,physicalDevice);

    if (vkAllocateMemory(device, &allocInfo, NULL, &textureImage.memory) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"failed to allocate image memory!");
        // fprintf(stderr,"failed to allocate image memory!");
        // exit(EXIT_FAILURE);
    }

    vkBindImageMemory(device, textureImage.image, textureImage.memory, 0);

    return textureImage;

}

void lreTransitionImageLayout(VkDevice device,VkCommandPool commandPool,VkQueue graphicsQueue,VkImage image,VkFormat format,VkImageLayout oldLayout,VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = lreBeginSingleTimeCommands(device,commandPool);

    VkImageMemoryBarrier barrier; memset(&barrier,0,sizeof(barrier));
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;


    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        LOGTOFILE(LOG_LEVEL_ERROR,"unsupported layout transition!");
    }


    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, NULL,
        0, NULL,
        1, &barrier
    );


    lreEndSingleTimeCommands(device,graphicsQueue,commandPool,commandBuffer);
}

void lreCopyBufferToImage2D(VkDevice device,VkCommandPool commandPool,VkQueue graphicsQueue,VkBuffer buffer,VkImage image,uint32_t width,uint32_t height) {
    VkCommandBuffer commandBuffer = lreBeginSingleTimeCommands(device,commandPool);

    VkBufferImageCopy region; memset(&region,0,sizeof(region));
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = (VkOffset3D){0, 0, 0};
    region.imageExtent = (VkExtent3D){
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    lreEndSingleTimeCommands(device,graphicsQueue,commandPool,commandBuffer);
}

VkImageView lreCreateImageView2D(VkDevice device,VkImage image,VkFormat format) {
    VkImageViewCreateInfo viewInfo; memset(&viewInfo,0,sizeof(viewInfo));
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    VkImageView imageView;
    if (vkCreateImageView(device, &viewInfo, NULL, &imageView) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"failed to create image view");
    }
    return imageView;
}

/*@mFilter - VK_FILTER_NEAREST/VK_FILTER_LINEAR
 *@samplerAddressMode - VK_SAMPLER_ADDRESS_MODE_REPEAT/VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT/VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE/VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE/VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
*/
VkSampler lreCreateSampler2D(VkDevice device,VkPhysicalDevice physicalDevice,VkFilter mFilter,VkSamplerAddressMode samplerAddressMode) {
    VkSamplerCreateInfo samplerInfo; memset(&samplerInfo,0,sizeof(samplerInfo));
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = mFilter; //or VK_FILTER_NEAREST
    samplerInfo.minFilter = mFilter; 

    // VK_SAMPLER_ADDRESS_MODE_REPEAT: Repeat the texture when going beyond the image dimensions.
    // VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT: Like repeat, but inverts the coordinates to mirror the image when going beyond the dimensions.
    // VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE: Take the color of the edge closest to the coordinate beyond the image dimensions.
    // VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE: Like clamp to edge, but instead uses the edge opposite to the closest edge.
    // VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER: Return a solid color when sampling beyond the dimensions of the image.
    samplerInfo.addressModeU = samplerAddressMode;
    samplerInfo.addressModeV = samplerAddressMode;
    samplerInfo.addressModeW = samplerAddressMode;

    VkPhysicalDeviceProperties properties; memset(&properties,0,sizeof(properties));
    vkGetPhysicalDeviceProperties(physicalDevice, &properties); //query maxSamplerAnisotropy

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

    // beyond image clamp
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

    // field specifies which coordinate system you want to use to address texels in an image
    // VK_FALSE = 0,1
    // VK_TRUE = 0,texWidth 0,texHeight
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VkSampler sampler;
    if (vkCreateSampler(device, &samplerInfo, NULL, &sampler) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"failed to create texture sampler!");
    }
    return sampler;

}

LreTextureImageObject lreCreateTextureImage2D(VkDevice device,VkPhysicalDevice physicalDevice,VkCommandPool commandPool,VkQueue graphicsQueue,const char* filepath) {
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(filepath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        LOGTOFILE(LOG_LEVEL_ERROR,"[STB_IMAGE] failed to load texture image!");
    }

    LreBufferObject stagingBuffer = lreCreateBuffer(device,physicalDevice,imageSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    lreMapMemory(device,stagingBuffer.memory,pixels,(size_t)imageSize);

    stbi_image_free(pixels);

    VkImageCreateInfo imageInfo; memset(&imageInfo,0,sizeof(imageInfo));
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = (uint32_t)texWidth;
    imageInfo.extent.height = (uint32_t)texHeight;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0; // Optional
    
    LreTextureImageObject textureImage = lreCreateTextureImage(device,physicalDevice,imageInfo);
    
    lreTransitionImageLayout(device,commandPool,graphicsQueue,textureImage.image,VK_FORMAT_R8G8B8A8_SRGB,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    
    lreCopyBufferToImage2D(device,commandPool,graphicsQueue,stagingBuffer.buffer,textureImage.image,(uint32_t)texWidth,(uint32_t)texHeight);

    lreTransitionImageLayout(device,commandPool,graphicsQueue,textureImage.image,VK_FORMAT_R8G8B8A8_SRGB,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    lreDestroyBuffer(device,stagingBuffer);

    textureImage.view = lreCreateImageView2D(device,textureImage.image,VK_FORMAT_R8G8B8A8_SRGB);

    textureImage.sampler = lreCreateSampler2D(device,physicalDevice,VK_FILTER_NEAREST,VK_SAMPLER_ADDRESS_MODE_REPEAT);

    return textureImage;
}


#endif