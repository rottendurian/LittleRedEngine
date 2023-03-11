#ifndef lre_pipeline_h
#define lre_pipeline_h

#include "pch.h"


typedef struct LreFixedFunctions {
    VkPipelineDynamicStateCreateInfo dynamicState;
    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    VkPipelineMultisampleStateCreateInfo multisampling;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlending;
} LreFixedFunctions;

VkDescriptorSetLayout lreCreateDescriptorSetLayout(VkDevice device,VkDescriptorSetLayoutBinding* bindings,uint32_t bindingCount);
VkDescriptorPool lreCreateDescriptorPool(VkDevice device,uint32_t descriptorCount,uint32_t size,...);
VkDescriptorSet* lreCreateDescriptorSets(VkDevice device,VkDescriptorSetLayout descriptorSetLayout,uint32_t descriptorCount,VkDescriptorPool descriptorPool);

#include "lre_buffer.h"
void lreUpdateDescriptorSets(VkDevice device,VkDescriptorSet* descriptorSets,uint32_t descriptorSetsCount,LreUniformBufferObject* buffers,uint32_t bufferSize);

static inline void lreDestroyDescriptorSetLayout(VkDevice device,VkDescriptorSetLayout layout) {
    vkDestroyDescriptorSetLayout(device,layout,NULL);
}
static inline void lreDestroyDescriptorPool(VkDevice device,VkDescriptorPool pool,VkDescriptorSet* descriptorSets) {
    vkDestroyDescriptorPool(device,pool,NULL);
    free(descriptorSets);
}

VkRenderPass lreCreateRenderPass(VkDevice device,VkFormat swapChainImageFormat);
static inline void lreDestroyRenderPass(VkDevice device,VkRenderPass renderPass) {vkDestroyRenderPass(device, renderPass, NULL);}

VkPipelineLayout lreCreateGraphicsPipelineLayout(VkDevice device,VkDescriptorSetLayout* descriptorSetLayout,uint32_t descriptorSetLayoutCount);
static inline void lreDestroyGraphicsPipelineLayout(VkDevice device,VkPipelineLayout pipelineLayout) {vkDestroyPipelineLayout(device,pipelineLayout,NULL);}

VkPipeline lreCreateGraphicsPipeline(VkDevice device,VkRenderPass renderPass,VkPipelineLayout pipelineLayout);
static inline void lreDestroyGraphicsPipeline(VkDevice device,VkPipeline graphicsPipeline) {vkDestroyPipeline(device,graphicsPipeline,NULL);}


#endif