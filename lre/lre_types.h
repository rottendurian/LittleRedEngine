#ifndef lre_types_h
#define lre_types_h

typedef struct LreWindowMouseOffsets {
    float lastX;
    float lastY;
    float offsetX;
    float offsetY;
    bool firstMouse;
} LreWindowMouseOffsets;

typedef struct LreWindowScrollOffsets {
    float offsetX;
    float offsetY;
} LreWindowScrollOffsets;

//window
typedef struct LreWindow {
    GLFWwindow* window;
    int width;
    int height;
    const char* name;
    int frameBufferResized;
    LreWindowMouseOffsets mouseOffsets;
    LreWindowScrollOffsets scrollOffsets;
} LreWindow; 

//device
typedef struct LreQueueFamilyVec {
    VkDeviceQueueCreateInfo* queueInfos;
    uint32_t size;
} LreQueueFamilyVec;

typedef struct Optional_U32 {
    bool has_value;
    uint32_t value;
} Optional_U32;

typedef struct QueueFamilyIndices {
    bool exists;
    Optional_U32 graphicsFamily;
    Optional_U32 presentFamily;
} QueueFamilyIndices;

//swapchain
typedef struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR* formats;
    uint32_t formatCount;
    VkPresentModeKHR* presentModes;
    uint32_t presentModeCount;
} SwapChainSupportDetails;

typedef struct LreSwapChainDetails {
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
    VkExtent2D extent;
} LreSwapChainDetails;

typedef struct LreSwapChain {
    VkSwapchainKHR swapChain;
    VkSurfaceFormatKHR surfaceFormat;
    VkExtent2D extent;
} LreSwapChain;

typedef struct LreSwapChainImages {
    VkImage* images;
    uint32_t count;
    VkImageView* imageViews;
} LreSwapChainImages;

//pipeline
typedef struct LreDescriptorPool {
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet* descriptorSets;
} LreDescriptorPool;

typedef struct LreVertexInputDescriptions {
    VkVertexInputBindingDescription* bindingDescriptions;
    uint32_t bindingDescriptionCount;
    VkVertexInputAttributeDescription* attributeDescriptions;
    uint32_t attributeDescriptionCount;
} LreVertexInputDescriptions;

//pipeline creation info
typedef struct LrePushConstantInfo {
    VkShaderStageFlags stageFlags;
    uint32_t size;
    void* data;
} LrePushConstantInfo;

typedef struct LreDescriptorSetItem {
    uint32_t binding;
    uint32_t descriptorCount;
    VkDescriptorType type;
    VkShaderStageFlags stageFlags;
    union {
        LreTextureImageObject* textureImage;
        LreUniformBufferObject* uniformBuffer;
    };
} LreDescriptorSetItem;

typedef struct LreDescriptorSetInfo {
    LreDescriptorSetItem* items;
    uint32_t itemCount;
} LreDescriptorSetInfo;

//buffer
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

//warning that this is not a LreTextureImageObject, it has no sampler
typedef struct LreTextureObject {
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
} LreTextureObject;

typedef struct LreTextureImageObject {
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
    VkSampler sampler;
} LreTextureImageObject;

//render
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

    uint32_t clearColorCount;
    VkClearValue* clearColor;

    void* pushConstant;
    uint32_t pushConstantSize;
} LreDrawInfo;

typedef struct LreSynchronization {
    VkSemaphore* imageAvailableSemaphores;
    VkSemaphore* renderFinishedSemaphores;
    VkFence* inFlightFences;
} LreSynchronization;


//objects
typedef struct LreInstanceObject {
    LreWindow window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
} LreInstanceObject;

typedef struct LreRenderObject {
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkCommandPool commandPool;
    VkCommandBuffer* commandBuffers;
    LreSynchronization syncObjects;
} LreRenderObject;

typedef struct LreSwapChainObject {
    LreSwapChain swapChain;
    LreSwapChainImages swapChainImages;
    LreTextureObject depthImage;
} LreSwapChainObject;

typedef struct LrePipelineObject {
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    LreFrameBuffer frameBuffers;
    LreDescriptorPool descriptorPool;

    //additional info - need a pipeline builder & defaults
    LreDescriptorSetInfo descriptorSets; //descriptorSet info
    LrePushConstantInfo pushConstant; //pushConstant info
    LreVertexInputDescriptions vertexInputDescriptions;


} LrePipelineObject;

typedef struct LreVulkanObject {
    LreInstanceObject instance;
    LreRenderObject render;
    LreSwapChainObject swapChain;
    LrePipelineObject pipeline;
} LreVulkanObject;

//vulkan object
typedef struct LreVulkanObject {
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
    LreTextureObject depthImage;
    LreFrameBuffer frameBuffer;
    VkCommandPool commandPool;
    VkCommandBuffer* commandBuffer;
    LreSynchronization syncObjects;
} LreVulkanObject;


#endif