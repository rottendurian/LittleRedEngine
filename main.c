#include "cglm/cglm.h"

#include "lre_vertex.h"
#include "lre_object.h"

typedef struct UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} UniformBufferObject;

static void updateUBOs(lreVulkanObject* vulkanObject,LreUniformBufferObject* uniforms,uint32_t currentFrame) {
    double time = glfwGetTime();
    UniformBufferObject ubo; memset(&ubo,0,sizeof(ubo));
    glm_mat4_identity(ubo.model);
    glm_rotate(ubo.model,time*glm_rad(90.0f),(vec3){0.0f,1.0f,0.0f});
    glm_lookat((vec3){2.0f,2.0f,2.0f},(vec3){0.0f,0.0f,0.0f},(vec3){0.0f,0.0f,1.0f},ubo.view);
    glm_perspective(45,vulkanObject->lreSwapChain.extent.width/vulkanObject->lreSwapChain.extent.width,0.1f,100.0f,ubo.proj);
    ubo.proj[1][1] *= -1;
    memcpy(uniforms[currentFrame].map,&ubo,sizeof(ubo));
}

int main() {
    log_setup();
    LOGSELECTFILE("stderr.log");

    lreVulkanObject vulkanObject;
    vulkanObject.window = createWindow(50,50,"hello world");     
    vulkanObject.instance = createInstance("[LRE] Little Red Engine",VK_MAKE_VERSION(1,0,0),"application",VK_MAKE_VERSION(1,0,0));
    vulkanObject.debugger = createDebugMessenger(vulkanObject);
    
    vulkanObject.surface = createSurface(vulkanObject);
    vulkanObject.physicalDevice = pickPhysicalDevice(vulkanObject);

    vulkanObject.device = createLogicalDevice(vulkanObject);
    vulkanObject.graphicsQueue = getGraphicsQueue(vulkanObject);
    vulkanObject.presentQueue = getPresentQueue(vulkanObject);
    
    vulkanObject.lreSwapChain = createSwapChain(vulkanObject);
    vulkanObject.lreSwapChainImages = createImageViews(vulkanObject);
    vulkanObject.renderPass = createRenderPass(vulkanObject);


    VkDescriptorSetLayoutBinding* bindings = (VkDescriptorSetLayoutBinding*)alloca(sizeof(VkDescriptorSetLayoutBinding)*2); memset(bindings,0,sizeof(bindings));
    bindings[0].binding = 0;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    bindings[0].pImmutableSamplers = NULL;

    bindings[1].binding = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    bindings[1].pImmutableSamplers = NULL;
    
    VkDescriptorSetLayout descriptorSetLayout = lreCreateDescriptorSetLayout(vulkanObject.device,bindings,2);
    
    vulkanObject.pipelineLayout = lreCreateGraphicsPipelineLayout(vulkanObject.device,&descriptorSetLayout,1);

    LreVertexInputDescriptions vertexInputDescriptions;
    vertexInputDescriptions.bindingDescriptions = VertexGetBindingDescription();
    vertexInputDescriptions.bindingDescriptionCount = 1;
    vertexInputDescriptions.attributeDescriptions = VertexGetAttributeDescriptions();
    vertexInputDescriptions.attributeDescriptionCount = VERTEX_ATTRIB_COUNT;

    vulkanObject.graphicsPipeline = lreCreateGraphicsPipeline(vulkanObject.device,vulkanObject.renderPass,vulkanObject.pipelineLayout,&vertexInputDescriptions,"res/shaders/simple_shader.vert.spv","res/shaders/simple_shader.frag.spv");

    lreDestroyVertexInputDescriptions(&vertexInputDescriptions);

    vulkanObject.frameBuffer = createFrameBuffer(vulkanObject);
    vulkanObject.commandPool = createCommandPool(vulkanObject);

    const Vertex vertices[] = {
        {{-0.5f, -0.5f,0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f,0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f,0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f,0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };

    const uint16_t indices[] = {
        0,1,2,2,3,0,
    };
    
    LreBufferObject vertexBuffer = lreCreateBufferStaged(vulkanObject.device,vulkanObject.physicalDevice,vulkanObject.commandPool,vulkanObject.graphicsQueue,
        vertices,sizeof(vertices),VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    LreBufferObject indexBuffer = lreCreateBufferStaged(vulkanObject.device,vulkanObject.physicalDevice,vulkanObject.commandPool,vulkanObject.graphicsQueue,
        indices,sizeof(indices),VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    LreUniformBufferObject* uniformBuffers = (LreUniformBufferObject*)malloc(MAX_FRAMES_IN_FLIGHT*sizeof(LreUniformBufferObject));
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        uniformBuffers[i] = lreCreateUniformBuffer(vulkanObject.device,vulkanObject.physicalDevice,sizeof(UniformBufferObject));
    }
    LreTextureImageObject textureImage = lreCreateTextureImage2D(vulkanObject.device,vulkanObject.physicalDevice,vulkanObject.commandPool,vulkanObject.graphicsQueue,"res/textures/bluetreesforest.jpg");

    LreDescriptorPool descriptorPool = lreCreateDescriptorPool(vulkanObject.device,descriptorSetLayout,MAX_FRAMES_IN_FLIGHT,2,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,uniformBuffers,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,&textureImage);
    
    vulkanObject.commandBuffer = createCommandBuffer(vulkanObject);
    vulkanObject.syncObjects = createSyncObjects(vulkanObject);

    uint32_t currentFrame = 0;
    glfwSetWindowUserPointer(vulkanObject.window.window,&vulkanObject.window);

    while (!lreWindowShouldClose(&vulkanObject.window)) {
        glfwPollEvents();
        updateUBOs(&vulkanObject,uniformBuffers,currentFrame);
        lreDrawFrame(&vulkanObject,currentFrame,vertexBuffer.buffer,indexBuffer.buffer,descriptorPool.descriptorSets);
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
    vkDeviceWaitIdle(vulkanObject.device);

    destroySyncObjects(vulkanObject);
    destroyCommandBuffers(vulkanObject);

    lreDestroyDescriptorPool(vulkanObject.device,descriptorPool);

    lreDestroyBuffer(vulkanObject.device,vertexBuffer);
    lreDestroyBuffer(vulkanObject.device,indexBuffer);
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        lreDestroyUniformBuffer(vulkanObject.device,uniformBuffers[i]);
    }
    free(uniformBuffers);
    lreDestroyTextureImage(vulkanObject.device,textureImage);

    destroyCommandPool(vulkanObject);
    destroyFrameBuffer(vulkanObject);
    destroyGraphicsPipeline(vulkanObject);
    destroyGraphicsPipelineLayout(vulkanObject);
    lreDestroyDescriptorSetLayout(vulkanObject.device,descriptorSetLayout);
    destroyRenderPass(vulkanObject);
    destroyImageViews(vulkanObject);
    destroySwapChain(vulkanObject);
    destroySurface(vulkanObject);
    destroyDevice(vulkanObject);
    destroyDebugMessenger(vulkanObject);
    destroyInstance(vulkanObject);
    destroyWindow(vulkanObject);

    log_cleanup();
    return EXIT_SUCCESS;
}