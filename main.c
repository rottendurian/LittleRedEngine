#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include "cglm/cglm.h"

#include "lre_window.h"
#include "lre_swapchain.h"
#include "lre_render.h"
#include "lre_instance.h"
#include "lre_device.h"
#include "lre_pipeline.h"
#include "lre_buffer.h"

#include "lre_objmodelloader.h"

#define LRE_CAMERA3D_STATIC
#include "lre_camera3d.h"


//LreInstanceClass
//LreSwapchainClass
//LreRenderPassClass
//LreDrawClass?

//Need to identify common data so I don't have to rehash a lot of it
//ObjModel class

//TODO
//Lighting
//multisampling
//antialiasing
//compute shaders
//abstractions


typedef struct UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
} UniformBufferObject;

typedef struct PushConstant {
    mat4 camera;
    mat4 model;
} PushConstant;

static void updateUBOs(LreVulkanObject* vulkanObject,LreUniformBufferObject* uniforms,uint32_t currentFrame) {
    double time = glfwGetTime();
    UniformBufferObject ubo; memset(&ubo,0,sizeof(ubo));
    glm_mat4_identity(ubo.model);
    glm_rotate(ubo.model,time*glm_rad(90.0f),(vec3){0.0f,1.0f,0.0f});
    glm_lookat((vec3){2.0f,2.0f,2.0f},(vec3){0.0f,0.0f,0.0f},(vec3){0.0f,0.0f,1.0f},ubo.view);
    glm_perspective(45,vulkanObject->lreSwapChain.extent.width/vulkanObject->lreSwapChain.extent.width,0.1f,100.0f,ubo.proj);
    ubo.proj[1][1] *= -1;
    ubo.time = time;
    // printf("%lf",ubo.time);
    memcpy(uniforms[currentFrame].map,&ubo,sizeof(ubo));
}

static void updatePushConstant(LreVulkanObject* vulkanObject,PushConstant* pushConstant) {
    double time = glfwGetTime();
    UniformBufferObject ubo; memset(&ubo,0,sizeof(ubo));
    glm_mat4_identity(ubo.model);
    glm_rotate(ubo.model,time*glm_rad(90.0f),(vec3){0.0f,1.0f,0.0f});

    glm_lookat((vec3){2.0f,2.0f,2.0f},(vec3){0.0f,0.0f,0.0f},(vec3){0.0f,0.0f,1.0f},ubo.view);
    glm_perspective(45,vulkanObject->lreSwapChain.extent.width/vulkanObject->lreSwapChain.extent.width,0.1f,100.0f,ubo.proj);
    ubo.proj[1][1] *= -1;

    // glm_perspective(45,vulkanObject->lreSwapChain.extent.width/vulkanObject->lreSwapChain.extent.width,0.1f,100.0f,pushConstant->model);

    // mat4 pushConstant;
    glm_mat4_mul_sse2(ubo.proj,ubo.view,pushConstant->camera);
    // pushConstant->model = ubo.model;
    memcpy(pushConstant->model,ubo.model,sizeof(mat4));

    // return pushConstant;
}

static void updateCamera(LreVulkanObject* vulkanObject,PushConstant* pushConstant,Camera3D* camera) {
    
    lre_camera3D_update_aspect(camera,(float)vulkanObject->lreSwapChain.extent.width/(float)vulkanObject->lreSwapChain.extent.width);
    mat4 proj;
    glm_mat4_mul_sse2(camera->proj,camera->view,proj);
    // glm_mat4_copy(camera->proj,proj);
    memcpy(pushConstant->camera,proj,sizeof(mat4));

    mat4 model;
    glm_mat4_identity(model);
    glm_rotate(model,glm_rad(-90.0f),(vec3){1.0f,0.0f,0.0f});
    memcpy(pushConstant->model,model,sizeof(mat4));
}

// glfwSetCursorPosCallback(window, mouse_callback);
// glfwSetScrollCallback(window, scroll_callback);
// float last_x;
// float last_y;
// bool firstMouse;

static void glfwProccessInput(LreVulkanObject* vulkanObject,Camera3D* camera,float deltaTime) {
    float offsetX = 0;
    float offsetY = 0;
    lreWindowGetMouseOffsets(&vulkanObject->window,&offsetX,&offsetY);
    lre_camera3D_rotate(camera,offsetX,offsetY,deltaTime);
    lreWindowGetScrollOffsets(&vulkanObject->window,&offsetY);
    lre_camera3D_zoom(camera,offsetY);

    // printf("offsets x:%f,y:%f\n",offsetX,offsetY);
    if (glfwGetKey(vulkanObject->window.window, GLFW_KEY_W) == GLFW_PRESS)
        lre_camera3D_move(camera,CAMERA3D_FORWARD,deltaTime);
    if (glfwGetKey(vulkanObject->window.window, GLFW_KEY_S) == GLFW_PRESS)
        lre_camera3D_move(camera,CAMERA3D_BACKWARD,deltaTime);
    if (glfwGetKey(vulkanObject->window.window, GLFW_KEY_A) == GLFW_PRESS)
        lre_camera3D_move(camera,CAMERA3D_LEFT,deltaTime);
    if (glfwGetKey(vulkanObject->window.window, GLFW_KEY_D) == GLFW_PRESS)
        lre_camera3D_move(camera,CAMERA3D_RIGHT,deltaTime);
    if (glfwGetKey(vulkanObject->window.window, GLFW_KEY_SPACE) == GLFW_PRESS)
        lre_camera3D_move(camera,CAMERA3D_UP,deltaTime);
    if (glfwGetKey(vulkanObject->window.window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        lre_camera3D_move(camera,CAMERA3D_DOWN,deltaTime);
}

int main() {
    log_setup();
    LOGSELECTFILE("stderr.log");
    freopen("stderr.log","w",stderr);

//camera
    Camera3D camera = lre_camera3D_new(60,0.1,100.0,50,50,(vec3){0.0,0.0,0.0});

//modelData
    LreObjModelData modelData = lreLoadObjectModel("res/meshes/vikingroom/viking_room.obj");
    // LreObjModelData modelData = lreLoadObjectModel("res/meshes/symphysis/symphysis.obj");
    // LreObjModelData modelData = lreLoadObjectModel("res/meshes/teapot.obj");
    // LreObjModelData modelData = lreLoadObjectModel("res/meshes/cube.obj");
    fprintf(stdout,"indices size %lu\n",modelData.mindices->items);
    fprintf(stdout,"vertices size %lu\n",modelData.mvertices->items);

//instance
    LreVulkanObject vulkanObject;
    vulkanObject.window = lreCreateWindow(800,600,"Vulkan");
    glfwSetWindowUserPointer(vulkanObject.window.window,&vulkanObject.window);

    vulkanObject.instance = lreCreateInstance("[LRE] Little Red Engine","application",VK_MAKE_VERSION(1,0,0),VK_MAKE_VERSION(1,0,0));
    vulkanObject.debugger = lreCreateDebugMessenger(vulkanObject.instance);
    vulkanObject.surface = lreCreateSurface(vulkanObject.instance,&vulkanObject.window);
    vulkanObject.physicalDevice = lrePickPhysicalDevice(vulkanObject.instance,vulkanObject.surface);
    vulkanObject.device = lreCreateLogicalDevice(vulkanObject.physicalDevice,vulkanObject.surface);
    
//render
    vulkanObject.graphicsQueue = lreGetGraphicsQueue(vulkanObject.physicalDevice,vulkanObject.device,vulkanObject.surface);
    vulkanObject.presentQueue = lreGetPresentQueue(vulkanObject.physicalDevice,vulkanObject.device,vulkanObject.surface);
    vulkanObject.commandPool = lreCreateCommandPool(vulkanObject.physicalDevice,vulkanObject.device,vulkanObject.surface);
    vulkanObject.commandBuffer = lreCreateDrawCommandBuffer(vulkanObject.device,vulkanObject.commandPool);
    vulkanObject.syncObjects = lreCreateSyncObjects(vulkanObject.device);

//swapchain
    vulkanObject.lreSwapChain = lreCreateSwapChain(&vulkanObject.window,vulkanObject.surface,vulkanObject.device,vulkanObject.physicalDevice);
    vulkanObject.lreSwapChainImages = lreCreateSwapchainImageViews(vulkanObject.device,&vulkanObject.lreSwapChain);
    vulkanObject.depthImage = lreCreateDepthResources(vulkanObject.device,vulkanObject.physicalDevice,vulkanObject.lreSwapChain);


//upload data?
    Vertex* vertices = _array_Vertex_get(modelData.mvertices,0);
    uint32_t* indices = _array_uint32_t_get(modelData.mindices,0);
    VkDeviceSize verticesSize = modelData.mvertices->items*sizeof(Vertex);
    VkDeviceSize indicesSize = modelData.mindices->items;
    
    LreBufferObject vertexBuffer = lreCreateBufferStaged(vulkanObject.device,vulkanObject.physicalDevice,vulkanObject.commandPool,vulkanObject.graphicsQueue,
        vertices,verticesSize,VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    LreBufferObject indexBuffer = lreCreateBufferStaged(vulkanObject.device,vulkanObject.physicalDevice,vulkanObject.commandPool,vulkanObject.graphicsQueue,
        indices,indicesSize*sizeof(uint32_t),VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    LreUniformBufferObject* uniformBuffers = (LreUniformBufferObject*)malloc(MAX_FRAMES_IN_FLIGHT*sizeof(LreUniformBufferObject));
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        uniformBuffers[i] = lreCreateUniformBuffer(vulkanObject.device,vulkanObject.physicalDevice,sizeof(UniformBufferObject));
    }
    LreTextureImageObject textureImage = lreCreateTextureImage2D(vulkanObject.device,vulkanObject.physicalDevice,vulkanObject.commandPool,vulkanObject.graphicsQueue,"res/meshes/vikingroom/viking_room.png");
    // LreTextureImageObject textureImage = lreCreateTextureImage2D(vulkanObject.device,vulkanObject.physicalDevice,vulkanObject.commandPool,vulkanObject.graphicsQueue,"res/meshes/symphysis/symphysis.png");

//renderpass (Pipelinestuff, descriptorSetBindings, pushConstant, vertexInput, indexInput, )
    const descriptorBindingsCount = 2;

    VkDescriptorSetLayoutBinding* descriptorBindings = (VkDescriptorSetLayoutBinding*)alloca(sizeof(VkDescriptorSetLayoutBinding)*descriptorBindingsCount);
    descriptorBindings[0].binding = 0;
    descriptorBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorBindings[0].descriptorCount = 1;
    descriptorBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    descriptorBindings[0].pImmutableSamplers = NULL;

    descriptorBindings[1].binding = 1;
    descriptorBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorBindings[1].descriptorCount = 1;
    descriptorBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    descriptorBindings[1].pImmutableSamplers = NULL;
    
    LreVertexInputDescriptions vertexInputDescriptions;
    vertexInputDescriptions.bindingDescriptions = VertexGetBindingDescription();
    vertexInputDescriptions.bindingDescriptionCount = 1;
    vertexInputDescriptions.attributeDescriptions = VertexGetAttributeDescriptions();
    vertexInputDescriptions.attributeDescriptionCount = VERTEX_ATTRIB_COUNT;
    
    VkDescriptorSetLayout descriptorSetLayout = lreCreateDescriptorSetLayout(vulkanObject.device,descriptorBindings,descriptorBindingsCount);
    vulkanObject.renderPass = lreCreateRenderPass(vulkanObject.device,vulkanObject.physicalDevice,vulkanObject.lreSwapChain.surfaceFormat.format);
    
    VkPushConstantRange pushConstantRange;
    pushConstantRange.offset = 0;
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.size = sizeof(PushConstant);

    vulkanObject.pipelineLayout = lreCreateGraphicsPipelineLayout(vulkanObject.device,&descriptorSetLayout,1,&pushConstantRange,1);
    vulkanObject.graphicsPipeline = lreCreateGraphicsPipeline(vulkanObject.device,vulkanObject.renderPass,vulkanObject.pipelineLayout,&vertexInputDescriptions,"res/shaders/simple_shader.vert.spv","res/shaders/simple_shader.frag.spv");
    vulkanObject.frameBuffer = lreCreateFrameBuffer(vulkanObject.device,vulkanObject.lreSwapChain,&vulkanObject.lreSwapChainImages,vulkanObject.renderPass,vulkanObject.depthImage);

    lreDestroyVertexInputDescriptions(&vertexInputDescriptions);

    LreDescriptorPool descriptorPool = lreCreateDescriptorPool(vulkanObject.device,descriptorSetLayout,MAX_FRAMES_IN_FLIGHT,2,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,uniformBuffers,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,&textureImage);

    uint32_t currentFrame = 0;

    VkDeviceSize bufferOffsets[] = {0};
    LreDrawInfo drawInfo;
    drawInfo.vertexBuffers = &vertexBuffer.buffer;
    drawInfo.bufferOffsets = bufferOffsets;
    drawInfo.bufferCount = 1;
    drawInfo.bufferStartIndex = 0;

    drawInfo.indexBuffer = indexBuffer.buffer;
    drawInfo.indexCount = indicesSize;
    drawInfo.indexType = VK_INDEX_TYPE_UINT32;
    drawInfo.indexOffset = 0;

    drawInfo.descriptorSets = descriptorPool.descriptorSets;
    drawInfo.descriptorStartSet = 0;
    drawInfo.descriptorCount = 1;

    drawInfo.pushConstant = NULL;
    drawInfo.pushConstantSize = 0;

    VkClearValue clearColor[] = {{{0.0f, 1.0f, 1.0f, 1.0f}},{{1.0f, 0}}};
    drawInfo.clearColorCount = sizeof(clearColor)/sizeof(clearColor[0]);
    drawInfo.clearColor = clearColor;

    PushConstant pushConstant; memset(&pushConstant,0,sizeof(pushConstant));
    drawInfo.pushConstant = &pushConstant;
    drawInfo.pushConstantSize = sizeof(pushConstant);

    double curTime = glfwGetTime();
    double prevTime = curTime;
    double deltaTime = curTime-prevTime;
    int deltaTimeCount = 0;
    double totalDeltaTime = 0.0;

    char titleBuffer[100]; memset(titleBuffer,0,sizeof(titleBuffer));


    while (!lreWindowShouldClose(&vulkanObject.window)) {
        prevTime = curTime;
        curTime = glfwGetTime();
        deltaTime = curTime-prevTime;
        deltaTimeCount++;
        totalDeltaTime+=deltaTime;

        if(totalDeltaTime >= 0.5) {
            sprintf(titleBuffer,"Vulkan: FPS{%lf}, CameraPos:{%f,%f,%f}",1.0/(totalDeltaTime/(double)deltaTimeCount),camera.pos[0],camera.pos[1],camera.pos[2]);
            totalDeltaTime = 0.0;
            deltaTimeCount = 0;
        }

        
        glfwSetWindowTitle(vulkanObject.window.window,titleBuffer);

        glfwPollEvents();
        glfwProccessInput(&vulkanObject,&camera,deltaTime);

        updateUBOs(&vulkanObject,uniformBuffers,currentFrame);
        // updatePushConstant(&vulkanObject,&pushConstant);
        updateCamera(&vulkanObject,&pushConstant,&camera);
        lreDrawFrame(&vulkanObject,currentFrame,&drawInfo);

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
    vkDeviceWaitIdle(vulkanObject.device);


    lreDestroySyncObjects(vulkanObject.device,vulkanObject.syncObjects);
    lreDestroyCommandBuffers(vulkanObject.commandBuffer);
    
    lreDestroyDescriptorPool(vulkanObject.device,descriptorPool);

    lreDestroyBuffer(vulkanObject.device,vertexBuffer);
    lreDestroyBuffer(vulkanObject.device,indexBuffer);
    array_uint32_t_cleanup(modelData.mindices);
    array_Vertex_cleanup(modelData.mvertices);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        lreDestroyUniformBuffer(vulkanObject.device,uniformBuffers[i]);
    }
    free(uniformBuffers);
    lreDestroyTextureImage(vulkanObject.device,textureImage);

    
    lreDestroyCommandPool(vulkanObject.device,vulkanObject.commandPool);
    lreDestroyTexture(vulkanObject.device,vulkanObject.depthImage);

    lreDestroyFrameBuffer(vulkanObject.device,vulkanObject.frameBuffer);
    
    lreDestroyGraphicsPipeline(vulkanObject.device,vulkanObject.graphicsPipeline);
    lreDestroyGraphicsPipelineLayout(vulkanObject.device,vulkanObject.pipelineLayout);

    lreDestroyDescriptorSetLayout(vulkanObject.device,descriptorSetLayout);
    
    lreDestroyRenderPass(vulkanObject.device,vulkanObject.renderPass);
    lreDestroySwapchainImageViews(vulkanObject.device,&vulkanObject.lreSwapChainImages);
    lreDestroySwapChain(vulkanObject.device,&vulkanObject.lreSwapChain);
    lreDestroySurface(vulkanObject.instance,vulkanObject.surface);
    lreDestroyDevice(vulkanObject.device);
    lreDestroyDebugMessenger(vulkanObject.instance,vulkanObject.debugger);
    lreDestroyInstance(vulkanObject.instance);
    lreDestroyWindow(&vulkanObject.window);

    log_cleanup();
    return EXIT_SUCCESS;
}