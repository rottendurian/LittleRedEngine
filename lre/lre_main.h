#ifndef lre_main_h
#define lre_main_h

#include "cglm.h"
#include "pch.h"
#include "lre_object.h"

typedef struct LreVulkanContext {
    LreVulkanObject vulkanObject;
} LreVulkanContext;

LreVulkanContext* LreGenerateVulkanContext(uint32_t windowWidth, uint32_t windowHeight, const char* windowName);



#endif