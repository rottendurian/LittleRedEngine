#ifndef lre_device_c
#define lre_device_c

#include "lre_device.h"

static QueueFamilyIndices indicesStatic = {0}; 

static inline QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device,VkSurfaceKHR surface) {
    if (indicesStatic.exists != 0) {
        return indicesStatic;
    }
    QueueFamilyIndices indices; memset(&indices,0,sizeof(indices));

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties* queueFamilies = (VkQueueFamilyProperties*)malloc(queueFamilyCount*sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    int i = 0;
    for (i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily.value = i;
            indices.graphicsFamily.has_value = true;
            break;
        }
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

    if (presentSupport) {
        indices.presentFamily.value = i;
        indices.presentFamily.has_value = true;
    }

    free(queueFamilies);
    memcpy(&indicesStatic,&indices,sizeof(indicesStatic));
    indicesStatic.exists = true;
    return indicesStatic;
}

QueueFamilyIndices lreFindQueueFamilies(VkPhysicalDevice physicalDevice,VkSurfaceKHR surface) {
    return findQueueFamilies(physicalDevice,surface);
}


static inline bool isDeviceSuitable(VkPhysicalDevice device,VkSurfaceKHR surface) { //can implement more later
    // VkPhysicalDeviceProperties deviceProperties; memset(&deviceProperties,0,sizeof(deviceProperties));
    // vkGetPhysicalDeviceProperties(device, &deviceProperties);
    // VkPhysicalDeviceFeatures deviceFeatures; memset(&deviceFeatures,0,sizeof(deviceFeatures));
    // vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    QueueFamilyIndices indices = findQueueFamilies(device,surface);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = lreQuerySwapChainSupport(device,surface);
        swapChainAdequate = !(swapChainSupport.formatCount == 0) && !(swapChainSupport.presentModeCount == 0);
        free(swapChainSupport.formats);
        free(swapChainSupport.presentModes);
    }

    VkPhysicalDeviceFeatures supportedFeatures; memset(&supportedFeatures,0,sizeof(supportedFeatures));
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return QueueFamilyIndices_isComplete(&indices) && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

VkPhysicalDevice lrePickPhysicalDevice(VkInstance instance,VkSurfaceKHR surface) {
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

    if (deviceCount == 0) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to find GPUs with Vulkan support");
        // fprintf(stderr,"Failed to find GPUs with Vulkan support");
        // exit(EXIT_FAILURE);
    }

    VkPhysicalDevice* devices = (VkPhysicalDevice*)malloc(deviceCount*sizeof(VkPhysicalDevice));

    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    for (int i = 0; i < deviceCount; i++) {
        if (isDeviceSuitable(devices[i],surface)) {
            physicalDevice = devices[i];
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to find suitable GPU");

        // fprintf(stderr,"Failed to find a suitable GPU");
        // exit(EXIT_FAILURE);
    }

    free(devices);
    return physicalDevice;
}

// static inline bool hashset_uint32_compare_func(uint32_t x,uint32_t y) {
//     return x == y;
// }
// #include "hashtable/hashfunctions.h"
// #define HASHSET_STATIC
// #define HASHSET_NAME hashset_uint32_t
// #define HASHSET_KETTYPE uint32_t
// #define HAHSET_USIZE uint32_t
// #define HASHSET_HASHFUNC jenkins_uint32
// #define HASHSET_COMPARE hashset_uint32_compare_func
// #include "hashtable/hashset.h"

// #include "lre_helper.h"
#include "hashtable/hashfunctions.h"
#include "hashtable/hashsetold.h"
hashset(uint32_t,jenkins_uint32,default_compare,SETNOTHEAP);


static inline LreQueueFamilyVec lreCreatePresentQueue(VkPhysicalDevice physicalDevice,VkSurfaceKHR surface,QueueFamilyIndices *indices) { //friendly reminder to free this thing

    uint32_t queueSize = 4;
    uint32_t queueIter = 0;
    
    VkDeviceQueueCreateInfo* queueCreateInfos = (VkDeviceQueueCreateInfo*)malloc(queueSize*sizeof(VkDeviceQueueCreateInfo));
    
    hashset_uint32_t* uniqueQueueFamilies = hashset_uint32_t_create();
    // lreSet uniqueQueueFamilies = lreCreateSet(2);

    uniqueQueueFamilies = hashset_uint32_t_insert(uniqueQueueFamilies,indices->graphicsFamily.value);
    uniqueQueueFamilies = hashset_uint32_t_insert(uniqueQueueFamilies,indices->presentFamily.value);

    // lreInsertSet(&uniqueQueueFamilies,indices->graphicsFamily.value);
    // lreInsertSet(&uniqueQueueFamilies,indices->presentFamily.value);

    

    float queuePriority = 1.0f;
    for (int i = 0; i < set_internal_primes[uniqueQueueFamilies->mem_size]; i++) {
        
        uint32_t value =  hashset_uint32_t_get(uniqueQueueFamilies,i);
        // uint32_t value = lreGetSet(&uniqueQueueFamilies,i);
        
        if (value != HASHTABLEDEFAULTVALUE) {
            // printf("Value worked %u\n",value);
            VkDeviceQueueCreateInfo queueCreateInfo; memset(&queueCreateInfo,0,sizeof(queueCreateInfo));
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = value;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            if (queueIter >= queueSize) {
                queueSize*=2;
                queueCreateInfos = realloc(queueCreateInfos,queueSize*sizeof(VkDeviceQueueCreateInfo));
            }
            queueCreateInfos[queueIter] = queueCreateInfo;
            queueIter++;
        } else {
            continue;
        }
    }

    // lreCleanUpSet(&uniqueQueueFamilies);
    hashset_uint32_t_cleanup(uniqueQueueFamilies);
    
    // queueIter;
    queueCreateInfos = realloc(queueCreateInfos,(queueIter)*sizeof(VkDeviceQueueCreateInfo));

    LreQueueFamilyVec infos = {queueCreateInfos,queueIter};

    return infos;
}

VkDevice lreCreateLogicalDevice(VkPhysicalDevice physicalDevice,VkSurfaceKHR surface) {
    VkDevice device;
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice,surface);

    LreQueueFamilyVec queueCreateInfos = lreCreatePresentQueue(physicalDevice,surface,&indices);

    //printf("create infos worked");
    VkDeviceQueueCreateInfo queueCreateInfo; memset(&queueCreateInfo,0,sizeof(queueCreateInfo));
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value;
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures; memset(&deviceFeatures,0,sizeof(deviceFeatures));
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo; memset(&createInfo,0,sizeof(createInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.queueInfos;
    createInfo.queueCreateInfoCount = queueCreateInfos.size;

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

    createInfo.enabledExtensionCount = (uint32_t)(lreDeviceExtensionsSize);
    createInfo.ppEnabledExtensionNames = lreDeviceExtensions;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = (uint32_t)validationLayersSize;
        createInfo.ppEnabledLayerNames = validationLayers;
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, NULL, &device) != VK_SUCCESS) {
        LOGTOFILE(LOG_LEVEL_ERROR,"Failed to create vulkan physical device");
        // fprintf(stderr,"Failed to create vulkan physical device");
        // exit(EXIT_FAILURE);
    }

    free(queueCreateInfos.queueInfos);

    return device;
}

VkQueue lreGetGraphicsQueue(VkPhysicalDevice physicalDevice,VkDevice device,VkSurfaceKHR surface) {
    VkQueue graphicsQueue;
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice,surface);
    vkGetDeviceQueue(device,indices.graphicsFamily.value,0,&graphicsQueue);
    
    return graphicsQueue;
}

VkQueue lreGetPresentQueue(VkPhysicalDevice physicalDevice,VkDevice device,VkSurfaceKHR surface) {
    VkQueue presentQueue;
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice,surface);
    vkGetDeviceQueue(device,indices.presentFamily.value,0,&presentQueue);
    
    return presentQueue;
}



#endif
