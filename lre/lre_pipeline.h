#ifndef lre_pipeline_h
#define lre_pipeline_h

#include "pch.h"
#include "lre_buffer.h"

// typedef struct LreFixedFunctions {
//     VkPipelineDynamicStateCreateInfo dynamicState;
//     VkPipelineVertexInputStateCreateInfo vertexInputInfo;
//     VkPipelineInputAssemblyStateCreateInfo inputAssembly;
//     VkViewport viewport;
//     VkRect2D scissor;
//     VkPipelineRasterizationStateCreateInfo rasterizer;
//     VkPipelineMultisampleStateCreateInfo multisampling;
//     VkPipelineColorBlendAttachmentState colorBlendAttachment;
//     VkPipelineColorBlendStateCreateInfo colorBlending;
// } LreFixedFunctions;



static inline void lreDestroyVertexInputDescriptions(LreVertexInputDescriptions* vertexInputDescriptions) {
    free(vertexInputDescriptions->bindingDescriptions);
    free(vertexInputDescriptions->attributeDescriptions);
}

VkDescriptorSetLayout lreCreateDescriptorSetLayout(VkDevice device,VkDescriptorSetLayoutBinding* bindings,uint32_t bindingCount);
// VkDescriptorPool _lreCreateDescriptorPool(VkDevice device,uint32_t descriptorCount,uint32_t argCount,...);
// VkDescriptorSet* _lreCreateDescriptorSets(VkDevice device,VkDescriptorPool descriptorPool,VkDescriptorSetLayout descriptorSetLayout,uint32_t descriptorCount);
// void lreUpdateDescriptorSets(VkDevice device,VkDescriptorSet* descriptorSets,uint32_t descriptorSetsCount,LreUniformBufferObject* buffers,uint32_t bufferSize,LreTextureImageObject textureImage);

LreDescriptorPool lreCreateDescriptorPool(VkDevice device,VkDescriptorSetLayout descriptorSetLayout,uint32_t descriptorCount,uint32_t argCount,...);


static inline void lreDestroyDescriptorSetLayout(VkDevice device,VkDescriptorSetLayout layout) {
    vkDestroyDescriptorSetLayout(device,layout,NULL);
}
static inline void lreDestroyDescriptorPool(VkDevice device,LreDescriptorPool pool) {
    vkDestroyDescriptorPool(device,pool.descriptorPool,NULL);
    free(pool.descriptorSets);
}

VkRenderPass lreCreateRenderPass(VkDevice device,VkPhysicalDevice physicalDevice,VkFormat swapChainImageFormat);
static inline void lreDestroyRenderPass(VkDevice device,VkRenderPass renderPass) {vkDestroyRenderPass(device, renderPass, NULL);}

VkPipelineLayout lreCreateGraphicsPipelineLayout(VkDevice device,VkDescriptorSetLayout* descriptorSetLayout,uint32_t descriptorSetLayoutCount,VkPushConstantRange* pushConstants,uint32_t pushConstantCount);
static inline void lreDestroyGraphicsPipelineLayout(VkDevice device,VkPipelineLayout pipelineLayout) {vkDestroyPipelineLayout(device,pipelineLayout,NULL);}

VkPipeline lreCreateGraphicsPipeline(VkDevice device,VkPhysicalDevice physicalDevice,VkRenderPass renderPass,VkPipelineLayout pipelineLayout,LreVertexInputDescriptions* vertexInputDescriptions,const char* vertexFile,const char* fragFile);
static inline void lreDestroyGraphicsPipeline(VkDevice device,VkPipeline graphicsPipeline) {vkDestroyPipeline(device,graphicsPipeline,NULL);}


#endif