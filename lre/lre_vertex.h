#ifndef lre_vertex_h
#define lre_vertex_h

#include "cglm.h"
#include "pch.h"

static const int VERTEX_ATTRIB_COUNT = 3;

typedef struct Vertex {
    vec2 pos;
    vec3 color;
    vec2 texCoord;
} Vertex;

static VkVertexInputBindingDescription VertexGetBindingDescription() {
    VkVertexInputBindingDescription bindingDescription; memset(&bindingDescription,0,sizeof(bindingDescription));
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

//REMEMBER TO FREEEE
static VkVertexInputAttributeDescription* VertexGetAttributeDescriptions() {
    VkVertexInputAttributeDescription* attributeDescriptions = (VkVertexInputAttributeDescription*)malloc(sizeof(VkVertexInputAttributeDescription)*VERTEX_ATTRIB_COUNT); memset(attributeDescriptions,0,sizeof(VkVertexInputAttributeDescription)*VERTEX_ATTRIB_COUNT);

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

    return attributeDescriptions;
}


#endif