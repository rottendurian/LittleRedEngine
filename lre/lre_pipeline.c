#ifndef lre_pipeline_c
#define lre_pipeline_c

#include "lre_pipeline.h"

typedef struct ShaderCode {
    char* code;
    size_t size;
} ShaderCode;

static inline ShaderCode readShader(const char* name) {
    FILE* file = fopen(name,"rb");
    if (file == NULL) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Could not find shader with the name of %s",name);
        // fprintf(stderr, "Could not find shader with the name of %s",name);
        // exit(EXIT_FAILURE);
    }
    fseek(file,0,SEEK_END);
    long size = ftell(file);
    fseek(file,0,SEEK_SET);

    char* buffer = (char*)malloc(size*sizeof(char));

    size_t read = fread(buffer,sizeof(char),size,file);
    // printf("Read %zu bytes\n",read);
    
    return (ShaderCode){buffer,read};
}

static inline VkShaderModule createShaderModule(VkDevice device,ShaderCode shader) {
    VkShaderModuleCreateInfo createInfo; memset(&createInfo,0,sizeof(createInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shader.size;
    createInfo.pCode = (const uint32_t*)(shader.code);
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create shader module");
        // fprintf(stderr,"Failed to create shader module");
        // exit(EXIT_FAILURE);
    }

    return shaderModule;
}

VkDescriptorSetLayout lreCreateDescriptorSetLayout(VkDevice device,VkDescriptorSetLayoutBinding* bindings,uint32_t bindingCount) {
    
    VkDescriptorSetLayoutCreateInfo layoutInfo; memset(&layoutInfo,0,sizeof(layoutInfo));
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindingCount;
    layoutInfo.pBindings = bindings;

    VkDescriptorSetLayout descriptorSetLayout;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &descriptorSetLayout) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create descriptor set layout!\n");
        // fprintf(stderr,"Failed to create descriptor set layout!\n");
        // exit(EXIT_FAILURE);
    }

    return descriptorSetLayout;
}

VkDescriptorPool _lreCreateDescriptorPool(VkDevice device,uint32_t descriptorCount,uint32_t argCount,...) {
    va_list args;
    va_start(args,argCount);
    
    VkDescriptorPoolSize* poolSize = (VkDescriptorPoolSize*)alloca(argCount*sizeof(VkDescriptorPoolSize)); memset(poolSize,0,sizeof(poolSize));
    for (int i = 0; i < argCount; i++) {
        poolSize[i].type = va_arg(args,VkDescriptorType);
        poolSize[i].descriptorCount = descriptorCount;
    }

    va_end(args);

    VkDescriptorPoolCreateInfo poolInfo; memset(&poolInfo,0,sizeof(poolInfo));
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = argCount;
    poolInfo.pPoolSizes = poolSize;
    poolInfo.maxSets = descriptorCount;

    VkDescriptorPool descriptorPool;
    if (vkCreateDescriptorPool(device, &poolInfo, NULL, &descriptorPool) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create descriptor pool!\n");
        // fprintf(stderr,"Failed to create descriptor pool!\n");
        // exit(EXIT_FAILURE);
    }
    return descriptorPool;
}

VkDescriptorSet* _lreCreateDescriptorSets(VkDevice device,VkDescriptorPool descriptorPool,VkDescriptorSetLayout descriptorSetLayout,uint32_t descriptorCount) {
    VkDescriptorSetLayout* layouts = (VkDescriptorSetLayout*)alloca(descriptorCount*sizeof(VkDescriptorSetLayout));
    for (int i = 0; i < descriptorCount; i++) {
        layouts[i] = descriptorSetLayout;
    }

    VkDescriptorSetAllocateInfo allocInfo; memset(&allocInfo,0,sizeof(allocInfo));
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = descriptorCount;
    allocInfo.pSetLayouts = layouts;

    VkDescriptorSet* descriptorSets = (VkDescriptorSet*)malloc(descriptorCount*sizeof(VkDescriptorSet));

    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"failed to allocate descriptor sets!");
        // fprintf(stderr,"failed to allocate descriptor sets!");
        // exit(EXIT_FAILURE);
    }



    return descriptorSets;
}

//this function definitely needs more support in the future (binding and array element) probably worth manually typing in future tbh
void _lreUpdateDescriptorSets(VkDevice device,VkDescriptorSet* descriptorSets,uint32_t descriptorSetsCount,LreUniformBufferObject* buffers,uint32_t bufferSize,LreTextureImageObject textureImage) {
    
    for (size_t i = 0; i < descriptorSetsCount; i++) {
        VkDescriptorBufferInfo bufferInfo; memset(&bufferInfo,0,sizeof(bufferInfo));
        bufferInfo.buffer = buffers[i].buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = bufferSize;

        VkDescriptorImageInfo imageInfo; memset(&imageInfo,0,sizeof(imageInfo));
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = textureImage.view;
        imageInfo.sampler = textureImage.sampler;

        VkWriteDescriptorSet *descriptorWrites = (VkWriteDescriptorSet*)alloca(2*sizeof(VkWriteDescriptorSet));
        memset(descriptorWrites,0,sizeof(VkWriteDescriptorSet)*2);

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;
        
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(device, 2, descriptorWrites, 0, NULL);
        
    }

}
//VkDescriptorType, &(buffer or image)
LreDescriptorPool lreCreateDescriptorPool(VkDevice device,VkDescriptorSetLayout descriptorSetLayout,uint32_t descriptorCount,uint32_t argCount,...) {
    
    LreDescriptorPool lrePool;
    
    lrePool.descriptorSetLayout = descriptorSetLayout;

    va_list args;
    va_start(args,argCount);

    VkDescriptorPoolSize* poolSize = (VkDescriptorPoolSize*)alloca(argCount*sizeof(VkDescriptorPoolSize)); 
    memset(poolSize,0,argCount*sizeof(VkDescriptorPoolSize));

    //this is sort of backwards
    VkWriteDescriptorSet *descriptorWrites = (VkWriteDescriptorSet*)alloca(descriptorCount*argCount*sizeof(VkWriteDescriptorSet));
    memset(descriptorWrites,0,descriptorCount*argCount*sizeof(VkWriteDescriptorSet));

    for (uint32_t i = 0; i < argCount; i++) {
        VkDescriptorType type = va_arg(args,VkDescriptorType);
        poolSize[i].type = type;
        poolSize[i].descriptorCount = descriptorCount;
        switch (type) {
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                LreUniformBufferObject* buffer = va_arg(args,LreUniformBufferObject*);
                for (uint32_t j = 0; j < descriptorCount; j++) { //this is meant to be outer loop
                    descriptorWrites[i+j*argCount].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    VkDescriptorBufferInfo *bufferInfo = (VkDescriptorBufferInfo*)malloc(sizeof(VkDescriptorBufferInfo));
                    bufferInfo->buffer = buffer[j].buffer;
                    bufferInfo->offset = 0;
                    bufferInfo->range = buffer[j].size;
                    
                    descriptorWrites[i+j*argCount].pBufferInfo = bufferInfo;
                }
                
                break;
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                LreTextureImageObject* textureImage =  va_arg(args,LreTextureImageObject*);
                for(uint32_t j = 0; j < descriptorCount; j++) {
                    descriptorWrites[i+j*argCount].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    
                    VkDescriptorImageInfo* imageInfo = (VkDescriptorImageInfo*)malloc(sizeof(VkDescriptorImageInfo));
                    imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imageInfo->imageView = textureImage->view;
                    imageInfo->sampler = textureImage->sampler;

                    descriptorWrites[i+j*argCount].pImageInfo = imageInfo;
                }
                break;
            default:
                LOGTOFILE(LOG_LEVEL_ERROR,"Descriptor type not supported!");
                break;
        }
    }

    va_end(args);

    VkDescriptorPoolCreateInfo poolInfo; memset(&poolInfo,0,sizeof(poolInfo));
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = argCount;
    poolInfo.pPoolSizes = poolSize;
    poolInfo.maxSets = descriptorCount;

    if (vkCreateDescriptorPool(device, &poolInfo, NULL, &lrePool.descriptorPool) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create descriptor pool!\n");
    }


    //setup descriptor sets
    VkDescriptorSetLayout* layouts = (VkDescriptorSetLayout*)alloca(descriptorCount*sizeof(VkDescriptorSetLayout));
    for (int i = 0; i < descriptorCount; i++) {
        layouts[i] = lrePool.descriptorSetLayout;
    }

    VkDescriptorSetAllocateInfo allocInfo; memset(&allocInfo,0,sizeof(allocInfo));
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = lrePool.descriptorPool;
    allocInfo.descriptorSetCount = descriptorCount;
    allocInfo.pSetLayouts = layouts;

    lrePool.descriptorSets = (VkDescriptorSet*)malloc(descriptorCount*sizeof(VkDescriptorSet));

    if (vkAllocateDescriptorSets(device, &allocInfo, lrePool.descriptorSets) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"failed to allocate descriptor sets!");
    }


    for (uint32_t i = 0; i < descriptorCount; i++) {
        for (uint32_t j = 0; j < argCount; j++) {
            descriptorWrites[i*argCount+j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[i*argCount+j].dstSet = lrePool.descriptorSets[i];
            descriptorWrites[i*argCount+j].dstBinding = j;
            descriptorWrites[i*argCount+j].dstArrayElement = 0;
            descriptorWrites[i*argCount+j].descriptorCount = 1;
            descriptorWrites[i*argCount+j].pTexelBufferView = NULL;
        }
        vkUpdateDescriptorSets(device, argCount, &descriptorWrites[i*argCount], 0, NULL);

        for (uint32_t j = 0; j < argCount; j++) {
            free((void*)descriptorWrites[i*argCount+j].pBufferInfo);
            free((void*)descriptorWrites[i*argCount+j].pImageInfo);
        }
    }

    return lrePool;
    
}


VkRenderPass lreCreateRenderPass(VkDevice device,VkPhysicalDevice physicalDevice,VkFormat swapChainImageFormat) {
    VkAttachmentDescription colorAttachment; memset(&colorAttachment,0,sizeof(colorAttachment));
    colorAttachment.format = swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef; memset(&colorAttachmentRef,0,sizeof(colorAttachmentRef));
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    //depth buffer
    VkAttachmentDescription depthAttachment; memset(&depthAttachment,0,sizeof(depthAttachment));
    depthAttachment.format = lreFindSupportedDepthFormat(physicalDevice);
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef; memset(&depthAttachmentRef,0,sizeof(depthAttachmentRef));
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass; memset(&subpass,0,sizeof(subpass));
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    

    VkSubpassDependency dependency; memset(&dependency,0,sizeof(dependency));
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription attachments[2] = {colorAttachment,depthAttachment};
    VkRenderPassCreateInfo renderPassInfo; memset(&renderPassInfo,0,sizeof(renderPassInfo));
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = sizeof(attachments)/sizeof(attachments[0]);
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    

    

    VkRenderPass renderPass;

    if (vkCreateRenderPass(device, &renderPassInfo, NULL, &renderPass) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create render pass!");
    }

    return renderPass;

}

VkPipelineLayout lreCreateGraphicsPipelineLayout(VkDevice device,VkDescriptorSetLayout* descriptorSetLayout,uint32_t descriptorSetLayoutCount,VkPushConstantRange* pushConstants,uint32_t pushConstantCount) {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo; memset(&pipelineLayoutInfo,0,sizeof(pipelineLayoutInfo));
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    // pipelineLayoutInfo.setLayoutCount = 0;
    // pipelineLayoutInfo.pushConstantRangeCount = 0;

    if (pushConstants != NULL) {
        pipelineLayoutInfo.pPushConstantRanges = pushConstants;
        pipelineLayoutInfo.pushConstantRangeCount = pushConstantCount;
    }

    if (descriptorSetLayout != NULL) {
        pipelineLayoutInfo.setLayoutCount = descriptorSetLayoutCount;
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayout;
    }

    VkPipelineLayout pipelineLayout;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create pipeline layout");
        // fprintf(stderr,"Failed to create pipeline layout");
        // exit(EXIT_FAILURE);
    }
    return pipelineLayout;
}

// #include "lre_vertex.h"

VkPipeline lreCreateGraphicsPipeline(VkDevice device,VkRenderPass renderPass,VkPipelineLayout pipelineLayout,LreVertexInputDescriptions* vertexInputDescriptions,const char* vertexFile,const char* fragFile) {
    ShaderCode vertex = readShader(vertexFile); 
    ShaderCode frag = readShader(fragFile);

    VkShaderModule vertModule = createShaderModule(device,vertex);
    VkShaderModule fragModule = createShaderModule(device,frag); 
    free(vertex.code);
    free(frag.code);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo; memset(&vertShaderStageInfo,0,sizeof(vertShaderStageInfo));
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo; memset(&fragShaderStageInfo,0,sizeof(fragShaderStageInfo));
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo; memset(&vertexInputInfo,0,sizeof(vertexInputInfo));
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = vertexInputDescriptions->bindingDescriptionCount;
    vertexInputInfo.vertexAttributeDescriptionCount = vertexInputDescriptions->attributeDescriptionCount;
    vertexInputInfo.pVertexBindingDescriptions = vertexInputDescriptions->bindingDescriptions;
    vertexInputInfo.pVertexAttributeDescriptions = vertexInputDescriptions->attributeDescriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly; memset(&inputAssembly,0,sizeof(inputAssembly));
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState; memset(&viewportState,0,sizeof(viewportState));
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer; memset(&rasterizer,0,sizeof(rasterizer));
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_NONE;//VK_CULL_MODE_BACK_BIT; 
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling; memset(&multisampling,0,sizeof(multisampling));
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment; memset(&colorBlendAttachment,0,sizeof(colorBlendAttachment));
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending; memset(&colorBlending,0,sizeof(colorBlending));
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState; memset(&dynamicState,0,sizeof(dynamicState));
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = (uint32_t)2;
    dynamicState.pDynamicStates = &dynamicStates[0];

    VkPipelineDepthStencilStateCreateInfo depthStencil; memset(&depthStencil,0,sizeof(depthStencil));
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    depthStencil.front; // Optional
    depthStencil.back; // Optional

    VkGraphicsPipelineCreateInfo pipelineInfo; memset(&pipelineInfo,0,sizeof(pipelineInfo));
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = &shaderStages[0];

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;

    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = renderPass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    VkPipeline graphicsPipeline;

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &graphicsPipeline) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create graphics pipeline");
        // fprintf(stderr,"Failed to create graphics pipeline");
        // exit(EXIT_FAILURE);
    }

    vkDestroyShaderModule(device, fragModule, NULL);
    vkDestroyShaderModule(device, vertModule, NULL);

    return graphicsPipeline;
}



#endif