#ifndef lre_vertex_h
#define lre_vertex_h

#include "cglm.h"
#include "pch.h"

//bindings, bindingtype, descriptorcount, stageflags
//vertex information
//buffers (vertex and index)
//create texture, and buffers
//potentially update those
//change settings in the pipeline and rebuild after stuff like new shaders
//cleanup (potentially a class wrapper later on)

static const int VERTEX_ATTRIB_COUNT = 3;

typedef struct Vertex {
    vec3 pos;
    vec3 color;
    vec2 texCoord;
} Vertex;

static VkVertexInputBindingDescription* VertexGetBindingDescription() {
    VkVertexInputBindingDescription* bindingDescription = (VkVertexInputBindingDescription*)malloc(sizeof(VkVertexInputBindingDescription)); 
    memset(bindingDescription,0,sizeof(VkVertexInputBindingDescription));
    bindingDescription->binding = 0;
    bindingDescription->stride = sizeof(Vertex);
    bindingDescription->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

//REMEMBER TO FREEEE
static VkVertexInputAttributeDescription* VertexGetAttributeDescriptions() {
    VkVertexInputAttributeDescription* attributeDescriptions = (VkVertexInputAttributeDescription*)malloc(sizeof(VkVertexInputAttributeDescription)*VERTEX_ATTRIB_COUNT);
    memset(attributeDescriptions,0,sizeof(VkVertexInputAttributeDescription)*VERTEX_ATTRIB_COUNT);

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
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

static inline size_t VertexHash(Vertex vertex) {
    const size_t prime = 0x01000193; // prime number for FNV-1a
    size_t hash = 0x811c9dc5; // FNV-1a offset basis

    // Hash the pos field
    hash = (hash ^ *(const uint32_t*)&vertex.pos[0]) * prime;
    hash = (hash ^ *(const uint32_t*)&vertex.pos[1]) * prime;
    hash = (hash ^ *(const uint32_t*)&vertex.pos[2]) * prime;

    // Hash the color field
    hash = (hash ^ *(const uint32_t*)&vertex.color[0]) * prime;
    hash = (hash ^ *(const uint32_t*)&vertex.color[1]) * prime;
    hash = (hash ^ *(const uint32_t*)&vertex.color[2]) * prime;

    // Hash the texCoord field
    hash = (hash ^ *(const uint32_t*)&vertex.texCoord[0]) * prime;
    hash = (hash ^ *(const uint32_t*)&vertex.texCoord[1]) * prime;

    return (size_t)hash;
}

static inline bool VertexCmp(Vertex v1,Vertex v2) {
    // return v1.pos == v2.pos && v1.color == v2.color && v1.texCoord == v2.texCoord;
    int posComparison = memcmp(&v1.pos, &v2.pos, sizeof(vec3));
    if (posComparison != 0) {
        return !posComparison;
    }

    int colorComparison = memcmp(&v1.color, &v2.color, sizeof(vec3));
    if (colorComparison != 0) {
        return !colorComparison;
    }

    return !memcmp(&v1.texCoord, &v2.texCoord, sizeof(vec2));
}

// array(Vertex,0);
// array(uint32_t,0);
#define ARRAY_STATIC
#define ARRAY_TYPE uint32_t
#define ARRAY_NAME array_uint32_t
#include "array/array.h"

#define ARRAY_STATIC
#define ARRAY_TYPE Vertex
#define ARRAY_NAME array_Vertex
#include "array/array.h"


// #include "array/array.h"
#define HASHTABLEDEFAULTFILLVALUE 0
#define HASHTABLEDEFAULTTYPEVALUE (Vertex){{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f}}
#define HASHTABLE_STATIC
#define HASHTABLE_HASHFUNC VertexHash
#define HASHTABLE_COMPARE VertexCmp
#define HASHTABLE_KEYTYPE Vertex
#define HASHTABLE_DATATYPE uint32_t
#define HASHTABLE_NAME hashtable_Vertex
#define HASHTABLE_USIZE uint32_t
#include "hashtable/hashtable.h"

// #include "hashtable/hashtableold2.h"
// hashtable(Vertex,uint32_t,VertexHash,VertexCmp,HASHNULL);

#undef HASHTABLEDEFAULTFILLVALUE
#undef HASHTABLEDEFAULTTYPEVALUE

#endif