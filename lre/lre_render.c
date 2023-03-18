#ifndef lre_render_c
#define lre_render_c

#include "lre_render.h"

LreFrameBuffer lreCreateFrameBuffer(VkDevice device,LreSwapChain lreSwapChain,LreSwapChainImages* lreSwapChainImages,VkRenderPass renderPass,LreTextureObject depthImage) {
    VkFramebuffer *swapChainFrameBuffers = (VkFramebuffer*)malloc(lreSwapChainImages->count*sizeof(VkFramebuffer));
    for (uint32_t i = 0; i < lreSwapChainImages->count; i++) {
        VkImageView attachments[] = {
            lreSwapChainImages->imageViews[i],
            depthImage.view
        };

        VkFramebufferCreateInfo framebufferInfo; memset(&framebufferInfo,0,sizeof(framebufferInfo));
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = sizeof(attachments)/sizeof(attachments[0]);
        framebufferInfo.pAttachments = attachments;
        
        framebufferInfo.width = lreSwapChain.extent.width;
        framebufferInfo.height = lreSwapChain.extent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, NULL, &swapChainFrameBuffers[i]) != VK_SUCCESS) {
            LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create framebuffer!");
        }
    }

    return (LreFrameBuffer){swapChainFrameBuffers,lreSwapChainImages->count};
}

VkCommandPool lreCreateCommandPool(VkPhysicalDevice physicalDevice,VkDevice device,VkSurfaceKHR surface) {
    QueueFamilyIndices queueFamilyIndices = lreFindQueueFamilies(physicalDevice,surface);

    VkCommandPoolCreateInfo poolInfo; memset(&poolInfo,0,sizeof(poolInfo)); 
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value;

    VkCommandPool commandPool;

    if (vkCreateCommandPool(device, &poolInfo, NULL, &commandPool) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create command pool!");
        // fprintf(stderr,"Failed to create command pool!");
        // exit(EXIT_FAILURE);
    }

    return commandPool;
}

VkCommandBuffer lreBeginSingleTimeCommands(VkDevice device,VkCommandPool commandPool) {
    VkCommandBufferAllocateInfo allocInfo; memset(&allocInfo,0,sizeof(allocInfo));
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo; memset(&beginInfo,0,sizeof(beginInfo));
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void lreEndSingleTimeCommands(VkDevice device,VkQueue graphicsQueue,VkCommandPool commandPool,VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo; memset(&submitInfo,0,sizeof(submitInfo));
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

VkCommandBuffer* lreCreateDrawCommandBuffer(VkDevice device,VkCommandPool commandPool) {
    VkCommandBufferAllocateInfo allocInfo; memset(&allocInfo,0,sizeof(allocInfo));
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    VkCommandBuffer* commandBuffer = (VkCommandBuffer*)malloc(MAX_FRAMES_IN_FLIGHT*sizeof(VkCommandBuffer));

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffer) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to allocate command buffers!");
        // fprintf(stderr,"Failed to allocate command buffers!");
        // exit(EXIT_FAILURE);
    }

    return commandBuffer;
}

void lreRecordDrawCommandBuffer(LreVulkanObject* vulkanObject,uint32_t imageIndex,uint32_t currentFrame,LreDrawInfo* drawInfo) {
    VkCommandBufferBeginInfo beginInfo; memset(&beginInfo,0,sizeof(beginInfo));
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = NULL; // Optional

    if (vkBeginCommandBuffer(vulkanObject->commandBuffer[currentFrame], &beginInfo) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to begin recording command buffer!");
        // fprintf(stderr,"Failed to begin recording command buffer!");
        // exit(EXIT_FAILURE);
    }

    VkRenderPassBeginInfo renderPassInfo; memset(&renderPassInfo,0,sizeof(renderPassInfo));
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = vulkanObject->renderPass;
    renderPassInfo.framebuffer = vulkanObject->frameBuffer.frame[imageIndex];

    renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
    renderPassInfo.renderArea.extent = vulkanObject->lreSwapChain.extent;

    // VkClearValue clearColor[] = {{{1.0f, 1.0f, 1.0f, 1.0f}},{{1.0f, 0}}};
    renderPassInfo.clearValueCount = drawInfo->clearColorCount;
    renderPassInfo.pClearValues = drawInfo->clearColor;

    vkCmdBeginRenderPass(vulkanObject->commandBuffer[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(vulkanObject->commandBuffer[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanObject->graphicsPipeline);

    VkViewport viewport; memset(&viewport,0,sizeof(viewport));
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)vulkanObject->lreSwapChain.extent.width;
    viewport.height = (float)vulkanObject->lreSwapChain.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(vulkanObject->commandBuffer[currentFrame], 0, 1, &viewport);

    VkRect2D scissor; memset(&scissor,0,sizeof(scissor));
    scissor.offset = (VkOffset2D){0, 0};
    scissor.extent = vulkanObject->lreSwapChain.extent; 
    vkCmdSetScissor(vulkanObject->commandBuffer[currentFrame], 0, 1, &scissor);

    
    vkCmdBindVertexBuffers(vulkanObject->commandBuffer[currentFrame], drawInfo->bufferStartIndex, drawInfo->bufferCount,drawInfo->vertexBuffers,drawInfo->bufferOffsets);
    vkCmdBindIndexBuffer(vulkanObject->commandBuffer[currentFrame],drawInfo->indexBuffer,drawInfo->indexOffset,drawInfo->indexType);
    vkCmdBindDescriptorSets(vulkanObject->commandBuffer[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanObject->pipelineLayout, drawInfo->descriptorStartSet, drawInfo->descriptorCount, drawInfo->descriptorSets, 0, NULL);
    
    if (drawInfo->pushConstant != NULL)
        vkCmdPushConstants(vulkanObject->commandBuffer[currentFrame],vulkanObject->pipelineLayout,VK_SHADER_STAGE_VERTEX_BIT,0,drawInfo->pushConstantSize,drawInfo->pushConstant);
    // VkBuffer vertexBuffers[] = {vertexBuffer};
    // VkDeviceSize offsets[] = {0};
    
    // vkCmdBindVertexBuffers(vulkanObject->commandBuffer[currentFrame], 0, 1, vertexBuffers, offsets);
    // vkCmdBindIndexBuffer(vulkanObject->commandBuffer[currentFrame],indexBuffer,0,VK_INDEX_TYPE_UINT16);
    // vkCmdBindDescriptorSets(vulkanObject->commandBuffer[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanObject->pipelineLayout, 0, 1, descriptorSets, 0, NULL);

    vkCmdDrawIndexed(vulkanObject->commandBuffer[currentFrame], drawInfo->indexCount, 1, 0, 0,0);


    vkCmdEndRenderPass(vulkanObject->commandBuffer[currentFrame]);

    if (vkEndCommandBuffer(vulkanObject->commandBuffer[currentFrame]) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to record command buffer!");
    }
}

LreSynchronization lreCreateSyncObjects(VkDevice device) {
    VkSemaphoreCreateInfo semaphoreInfo; memset(&semaphoreInfo,0,sizeof(semaphoreInfo));
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo; memset(&fenceInfo,0,sizeof(fenceInfo));
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    LreSynchronization syncObjects;
    syncObjects.imageAvailableSemaphores = (VkSemaphore*)malloc(MAX_FRAMES_IN_FLIGHT*sizeof(VkSemaphore));
    syncObjects.renderFinishedSemaphores = (VkSemaphore*)malloc(MAX_FRAMES_IN_FLIGHT*sizeof(VkSemaphore));
    syncObjects.inFlightFences = (VkFence*)malloc(MAX_FRAMES_IN_FLIGHT*sizeof(VkFence));

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(device, &semaphoreInfo, NULL, &syncObjects.imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, NULL, &syncObjects.renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, NULL, &syncObjects.inFlightFences[i]) != VK_SUCCESS) 
        {
            LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create semaphores!");
        }

    }

    return syncObjects;
}

void lreDrawFrame(LreVulkanObject *vulkanObject,uint32_t currentFrame,LreDrawInfo* drawInfo) {
    vkWaitForFences(vulkanObject->device, 1, &vulkanObject->syncObjects.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(vulkanObject->device, vulkanObject->lreSwapChain.swapChain, UINT64_MAX, vulkanObject->syncObjects.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        vulkanObject->window.frameBufferResized = false;
        lreReCreateSwapChain(vulkanObject);
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to recreate swap chain");
        // fprintf(stderr,"Failed to recreate swap chain");
        // exit(EXIT_FAILURE);
    }

    vkResetFences(vulkanObject->device, 1, &vulkanObject->syncObjects.inFlightFences[currentFrame]);

    vkResetCommandBuffer(vulkanObject->commandBuffer[currentFrame], 0);

    lreRecordDrawCommandBuffer(vulkanObject,imageIndex,currentFrame,drawInfo);

    VkSubmitInfo submitInfo; memset(&submitInfo,0,sizeof(submitInfo));
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {vulkanObject->syncObjects.imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vulkanObject->commandBuffer[currentFrame];

    VkSemaphore signalSemaphores[] = {vulkanObject->syncObjects.renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vulkanObject->graphicsQueue, 1, &submitInfo, vulkanObject->syncObjects.inFlightFences[currentFrame]) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to submit draw command buffer!");
        // fprintf(stderr,"Failed to submit draw command buffer!");
        // exit(EXIT_FAILURE);
    }

    VkPresentInfoKHR presentInfo; memset(&presentInfo,0,sizeof(presentInfo));
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {vulkanObject->lreSwapChain.swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = NULL; // Optional

    result = vkQueuePresentKHR(vulkanObject->presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || vulkanObject->window.frameBufferResized == true) {
        vulkanObject->window.frameBufferResized = false;
        lreReCreateSwapChain(vulkanObject);
        return;
    } else if (result != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to recreate swap chain");
        // fprintf(stderr,"Failed to recreate swap chain");
        // exit(EXIT_FAILURE);
    }

}

#endif