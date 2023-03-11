#ifndef lrepch_h
#define lrepch_h

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>

#include "logging/logging.h"

#ifndef NDEBUG
    #define LOGMEMORYALLOCATIONS
    #include "memory/memlog.h"
#endif

#endif