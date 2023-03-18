#ifndef lre_objmodelloader_h
#define lre_objmodelloader_h

#include "lre_vertex.h"
#include "fast_obj.h"

typedef struct LreObjModelData {
    array_Vertex* mvertices;
    array_uint32_t* mindices;
} LreObjModelData;

static inline LreObjModelData lreLoadObjectModel(const char* filename) {
    fastObjMesh* mesh = fast_obj_read(filename);
#ifdef LRE_OBJMODELLOADER_DEBUG
    printf("Group count: %lu\n",mesh->group_count);
#endif
    array_Vertex* mvertices = array_Vertex_new(100);
    array_uint32_t* mindices = array_uint32_t_new(100);
    hashtable_Vertex* hashtableVertices = hashtable_Vertex_new();

    for (uint32_t i = 0; i < mesh->group_count; i++) {
        const fastObjGroup group = mesh->groups[i];
#ifdef LRE_OBJMODELLOADER_DEBUG
        printf("Group[%lu] name: %s",i,group.name);
#endif
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

    hashtable_Vertex_cleanup(hashtableVertices);

    fast_obj_destroy(mesh);

    return (LreObjModelData){mvertices,mindices};

}



#endif