#ifndef lre_instance_c
#define lre_instance_c
#include "lre_instance.h"

static FILE* LOGGING_FILE;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
    
    // LOGTOFILE(LOG_LEVEL_DEBUG,"validation layer: %s\n", pCallbackData->pMessage);
    fprintf(LOGGING_FILE,"validation layer: %s\n", pCallbackData->pMessage);

    return VK_FALSE;
}

static inline VkDebugUtilsMessengerCreateInfoEXT populateDebugMessengerCreateInfo() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo; memset(&createInfo,0,sizeof(createInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    LOGGING_FILE = fopen("validationlayers.log","w");
    return createInfo;
}

static const char** getRequiredExtensions(uint32_t *extensionCount) {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    const char** extensions = (const char**)malloc(glfwExtensionCount*sizeof(const char*)); 

    memcpy(extensions,glfwExtensions,glfwExtensionCount*sizeof(const char*));

    if (enableValidationLayers) {
        glfwExtensionCount+=1;
        extensions = realloc(extensions,glfwExtensionCount*sizeof(const char*));
        if (extensions == NULL) {
            fprintf(stderr,"Realloc failed");
            exit(EXIT_FAILURE);
        }
        extensions[glfwExtensionCount-1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    }
    // for (int i = 0; i < glfwExtensionCount; i++) {
    //     printf("%s\n",extensions[i]);
    // }
    *extensionCount = glfwExtensionCount;
    return extensions;
}

static bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    VkLayerProperties* availableLayers = (VkLayerProperties*)malloc(layerCount*sizeof(VkLayerProperties));
    
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    for (int i = 0; i < validationLayersSize; i++) {
        bool layerFound = false;
        for (int k = 0; k < layerCount; k++) {
            if (strcmp(validationLayers[i], availableLayers[k].layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            free(availableLayers);
            return false;
        }
    }

    free(availableLayers);
    return true;

}

VkInstance lreCreateInstance(const char *engineName,const char* appName,unsigned int engineVersion,unsigned int appVersion) {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Validation layers requested but not supported");
        // fprintf(stderr,"Validation layers requested but not supported");
        // exit(EXIT_FAILURE);
    }
    
    VkApplicationInfo appInfo; memset(&appInfo,0,sizeof(appInfo));
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = appVersion;
    appInfo.pEngineName = engineName;
    appInfo.engineVersion = engineVersion;
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo; memset(&createInfo,0,sizeof(createInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    
    uint32_t extensionCount = 0;
    const char** extensions = getRequiredExtensions(&extensionCount);

    createInfo.enabledExtensionCount = extensionCount;
    createInfo.ppEnabledExtensionNames = extensions;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = (uint32_t)validationLayersSize;
        createInfo.ppEnabledLayerNames = validationLayers;

        debugCreateInfo = populateDebugMessengerCreateInfo();
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = NULL;
    }

    VkInstance instance;
    if (vkCreateInstance(&createInfo, NULL, &instance) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create VkInstance");
        // fprintf(stderr,"Failed to create VkInstance");
        // exit(EXIT_FAILURE);
    }

    free(extensions);

    return instance;
}

void lreDestroyInstance(VkInstance instance) {
    vkDestroyInstance(instance,NULL);
}

static inline VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static inline void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debugMessenger, pAllocator);
    }
}

VkDebugUtilsMessengerEXT lreCreateDebugMessenger(VkInstance instance) {
    if (!enableValidationLayers) return NULL;

    VkDebugUtilsMessengerEXT debugMessenger;
    VkDebugUtilsMessengerCreateInfoEXT createInfo = populateDebugMessengerCreateInfo();

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, NULL, &debugMessenger) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create DebugUtilsMessengerEXT");
        // fprintf(stderr,"Failed to create DebugUtilsMessengerEXT");
        // exit(EXIT_FAILURE);
    }

    return debugMessenger;
}

void lreDestroyDebugMessenger(VkInstance instance,VkDebugUtilsMessengerEXT debugMessenger) {
    if (!enableValidationLayers) return;
    DestroyDebugUtilsMessengerEXT(instance,debugMessenger,NULL);
}



#endif