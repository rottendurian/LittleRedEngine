#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include "cglm/cglm.h"

#include "lre_vertex.h"
#include "lre_object.h"
#include "fast_obj.h"

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
} UniformBufferObject;

static void updateUBOs(LreVulkanObject* vulkanObject,LreUniformBufferObject* uniforms,uint32_t currentFrame) {
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
    freopen("stderr.log","w",stderr);
    double beforeModel = glfwGetTime();

    fastObjMesh* mesh = fast_obj_read("res/meshes/vikingroom/viking_room.obj");
    printf("Group count: %lu\n",mesh->group_count);

    
    array_Vertex* mvertices = array_Vertex_new(50);
    array_uint32_t* mindices = array_uint32_t_new(50);
    hashtable_Vertex* hashtableVertices = hashtable_Vertex_new();

    for (uint32_t i = 0; i < mesh->group_count; i++) {
        const fastObjGroup group = mesh->groups[i];
        
        printf("Group[%lu] name: %s",i,group.name);

        int idx = 0;
        for (uint32_t j = 0; j < group.face_count; j++) {
            uint32_t fv = mesh->face_vertices[group.face_offset+j];
            
            for (uint32_t k = 0; k < fv; k++) {
                fastObjIndex mi = mesh->indices[group.index_offset+idx];
                
                Vertex vertex;
                vertex.pos[0] = mesh->positions[3 * mi.p + 0];
                vertex.pos[1] = mesh->positions[3 * mi.p + 1];
                vertex.pos[2] = mesh->positions[3 * mi.p + 2];

                vertex.color[0] = 1.0f;
                vertex.color[1] = 1.0f;
                vertex.color[2] = 1.0f;

                vertex.texCoord[0] = mesh->texcoords[2 * mi.t + 0];
                vertex.texCoord[1] = 1.0f - mesh->texcoords[2 * mi.t + 1];

                hashtable_Vertex_metadata* meta = hashtable_Vertex_get(hashtableVertices,vertex);
                // printf("found tag %zu\n",(size_t)meta);

                if (meta == NULL) {
                    hashtableVertices = hashtable_Vertex_insert(hashtableVertices,vertex,mvertices->items);
                    mindices = array_uint32_t_push(mindices,mvertices->items);
                    mvertices = array_Vertex_push(mvertices,vertex);
                } else {
                    // printf("data = %lu\n",meta->data);
                    mindices = array_uint32_t_push(mindices,(uint32_t)meta->data);
                }
                
                idx++;
            }
        }

    }
    double afterModel = glfwGetTime();
    printf("After model, %lf\n",afterModel-beforeModel);
    
    printf("hashtable size %zu\n",hashtableVertices->item_count);
    hashtable_Vertex_cleanup(hashtableVertices);
    // for (int i = 0; i < mindices->items; i++) {
    //     fprintf(stdout,"%lu,",*_array_uint32_t_get(mindices,i));
    // }
    fprintf(stdout,"indices size %lu\n",mindices->items);

    // for (int i = 0; i < mvertices->items/4; i++) {
    //     Vertex vertex = *_array_Vertex_get(mvertices,i);
    //     fprintf(stdout,"p{%f,%f,%f},t{%f,%f}\n",vertex.pos[0],vertex.pos[1],vertex.pos[2],vertex.texCoord[0],vertex.texCoord[1]);
    // }
    fprintf(stdout,"vertices size %lu\n",mvertices->items);

    // exit(EXIT_SUCCESS);

//instance
    LreVulkanObject vulkanObject;
    vulkanObject.window = createWindow(50,50,"hello world");     
    vulkanObject.instance = createInstance("[LRE] Little Red Engine",VK_MAKE_VERSION(1,0,0),"application",VK_MAKE_VERSION(1,0,0));
    vulkanObject.debugger = createDebugMessenger(vulkanObject);
    vulkanObject.surface = createSurface(vulkanObject);
    
    vulkanObject.physicalDevice = pickPhysicalDevice(vulkanObject);
    vulkanObject.device = createLogicalDevice(vulkanObject);

//render
    vulkanObject.commandPool = createCommandPool(vulkanObject);
    vulkanObject.commandBuffer = lreCreateDrawCommandBuffer(vulkanObject.device,vulkanObject.commandPool);
    vulkanObject.syncObjects = lreCreateSyncObjects(vulkanObject.device);


//swapchain
    vulkanObject.graphicsQueue = lreGetGraphicsQueue(vulkanObject.physicalDevice,vulkanObject.device,vulkanObject.surface);
    vulkanObject.presentQueue = lreGetPresentQueue(vulkanObject.physicalDevice,vulkanObject.device,vulkanObject.surface);
    
    vulkanObject.lreSwapChain = lreCreateSwapChain(&vulkanObject.window,vulkanObject.surface,vulkanObject.device,vulkanObject.physicalDevice);
    vulkanObject.lreSwapChainImages = lreCreateSwapchainImageViews(vulkanObject.device,&vulkanObject.lreSwapChain);
    vulkanObject.depthImage = lreCreateDepthResources(vulkanObject.device,vulkanObject.physicalDevice,vulkanObject.lreSwapChain);


//upload data?
    Vertex* vertices = _array_Vertex_get(mvertices,0);
    uint32_t* indices = _array_uint32_t_get(mindices,0);
    VkDeviceSize verticesSize = mvertices->items*sizeof(Vertex);
    VkDeviceSize indicesSize = mindices->items;
    
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

//renderpass (descriptorSetBindings, vertexInput, indexInput, )

    VkDescriptorSetLayoutBinding* descriptorBindings = (VkDescriptorSetLayoutBinding*)alloca(sizeof(VkDescriptorSetLayoutBinding)*2);
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
    

    VkDescriptorSetLayout descriptorSetLayout = lreCreateDescriptorSetLayout(vulkanObject.device,descriptorBindings,2);
    
    vulkanObject.renderPass = createRenderPass(vulkanObject);
    vulkanObject.pipelineLayout = lreCreateGraphicsPipelineLayout(vulkanObject.device,&descriptorSetLayout,1);
    vulkanObject.graphicsPipeline = lreCreateGraphicsPipeline(vulkanObject.device,vulkanObject.renderPass,vulkanObject.pipelineLayout,&vertexInputDescriptions,"res/shaders/simple_shader.vert.spv","res/shaders/simple_shader.frag.spv");
    vulkanObject.frameBuffer = lreCreateFrameBuffer(vulkanObject.device,vulkanObject.lreSwapChain,&vulkanObject.lreSwapChainImages,vulkanObject.renderPass,vulkanObject.depthImage);

    lreDestroyVertexInputDescriptions(&vertexInputDescriptions);


    LreDescriptorPool descriptorPool = lreCreateDescriptorPool(vulkanObject.device,descriptorSetLayout,MAX_FRAMES_IN_FLIGHT,2,VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,uniformBuffers,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,&textureImage);
    
    

    uint32_t currentFrame = 0;
    glfwSetWindowUserPointer(vulkanObject.window.window,&vulkanObject.window);

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

    VkClearValue clearColor[] = {{{0.0f, 1.0f, 1.0f, 1.0f}},{{1.0f, 0}}};
    drawInfo.clearColorCount = sizeof(clearColor)/sizeof(clearColor[0]);
    drawInfo.clearColor = clearColor;

    
    while (!lreWindowShouldClose(&vulkanObject.window)) {
        glfwPollEvents();
        updateUBOs(&vulkanObject,uniformBuffers,currentFrame);
        lreDrawFrame(&vulkanObject,currentFrame,&drawInfo);
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
    vkDeviceWaitIdle(vulkanObject.device);



    destroySyncObjects(vulkanObject);
    destroyCommandBuffers(vulkanObject);

    lreDestroyDescriptorPool(vulkanObject.device,descriptorPool);

    lreDestroyBuffer(vulkanObject.device,vertexBuffer);
    lreDestroyBuffer(vulkanObject.device,indexBuffer);
    array_uint32_t_cleanup(mindices);
    array_Vertex_cleanup(mvertices);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        lreDestroyUniformBuffer(vulkanObject.device,uniformBuffers[i]);
    }
    free(uniformBuffers);
    lreDestroyTextureImage(vulkanObject.device,textureImage);

    destroyCommandPool(vulkanObject);
    lreDestroyTexture(vulkanObject.device,vulkanObject.depthImage);
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

    fast_obj_destroy(mesh);

    log_cleanup();
    return EXIT_SUCCESS;
}