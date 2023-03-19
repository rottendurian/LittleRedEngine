#ifndef lre_camera3d_h
#define lre_camera3d_h

#define CGLM_FORCE_DEPTH_ZERO_TO_ONE
#include "cglm/cglm.h"

#ifdef LRE_CAMERA3D_STATIC
    #define LRE_CAMERA3D_STATICI static inline
    #define LRE_CAMERA3D_IMPLEMENTATION
#else
    #define LRE_CAMERA3D_STATICI
#endif

#ifndef LRE_CAMERA3D_DEFAULTS
    #define LRE_CAMERA3D_DEFAULTS
    #define LRE_CAMERA3D_YAW -90.0f
    #define LRE_CAMERA3D_PITCH 0.0f
    #define LRE_CAMERA3D_SPEED 1.0f
    #define LRE_CAMERA3D_MAXFOV 90.0f
    #define LRE_CAMERA3D_SENSITIVITY 0.1f
    #define LRE_CAMERA3D_WORLD_UP (vec3){0.0f, 1.0f, 0.0f}
#endif

enum Camera3DMovement {
    CAMERA3D_FORWARD,
    CAMERA3D_BACKWARD,
    CAMERA3D_LEFT,
    CAMERA3D_RIGHT,
    CAMERA3D_UP,
    CAMERA3D_DOWN,
};

typedef struct Camera3D {
    float fov;
    float near;
    float far;
    float aspectRatio;

    float speed;
    float sensitivity;

    float yaw;
    float pitch;

    vec3 pos;
    vec3 front;
    vec3 up;
    vec3 right;

    mat4 proj;
    mat4 view;

} Camera3D;

LRE_CAMERA3D_STATICI Camera3D lre_camera3D_new(float fov, float near, float far, uint32_t screenWidth, uint32_t screenHeight, vec3 pos);
LRE_CAMERA3D_STATICI void lre_camera3D_update_proj(Camera3D *camera);
LRE_CAMERA3D_STATICI void lre_camera3D_update_view(Camera3D *camera);
LRE_CAMERA3D_STATICI void lre_camera3D_update_aspect(Camera3D *camera,float aspectRatio);
LRE_CAMERA3D_STATICI void lre_camera3D_update(Camera3D *camera);
LRE_CAMERA3D_STATICI void lre_camera3D_move(Camera3D *camera, enum Camera3DMovement direction, float deltaTime);
LRE_CAMERA3D_STATICI void lre_camera3D_rotate(Camera3D *camera, float xoffset, float yoffset,float deltaTime);
LRE_CAMERA3D_STATICI void lre_camera3D_update_vectors(Camera3D* camera);
LRE_CAMERA3D_STATICI void lre_camera3D_zoom(Camera3D *camera, float yoffset);

#ifdef LRE_CAMERA3D_IMPLEMENTATION

LRE_CAMERA3D_STATICI Camera3D lre_camera3D_new(float fov, float near, float far, uint32_t screenWidth, uint32_t screenHeight, vec3 pos) {
    Camera3D camera; 
    camera.fov = fov;
    camera.near = near;
    camera.far = far;
    camera.aspectRatio = (float)screenWidth/(float)screenHeight;

    camera.speed = LRE_CAMERA3D_SPEED;
    camera.sensitivity = LRE_CAMERA3D_SENSITIVITY;

    camera.yaw = LRE_CAMERA3D_YAW;
    camera.pitch = LRE_CAMERA3D_PITCH;
    
    glm_vec3_copy(pos, camera.pos);

    glm_vec3_copy((vec3){-1.0f, 0.0f, 0.0f}, camera.front);
    glm_vec3_copy(LRE_CAMERA3D_WORLD_UP, camera.up);
    glm_vec3_cross(camera.up,camera.front,camera.right);
    
    lre_camera3D_update(&camera);

    return camera;
}

LRE_CAMERA3D_STATICI void lre_camera3D_update_proj(Camera3D *camera) {
    // glm_perspective_rh_zo(glm_rad(camera->fov), camera->aspectRatio, camera->near, camera->far, camera->proj);
    glm_perspective(glm_rad(camera->fov), camera->aspectRatio, camera->near, camera->far, camera->proj);
    camera->proj[1][1]*=-1;
}

LRE_CAMERA3D_STATICI void lre_camera3D_update_view(Camera3D *camera) {
    vec3 front;
    glm_vec3_add(camera->pos, camera->front, front);
    glm_lookat(camera->pos, front, camera->up, camera->view);
    // glm_lookat_rh(camera->pos, camera->front, camera->up, camera->view);
}

LRE_CAMERA3D_STATICI void lre_camera3D_update_aspect(Camera3D *camera,float aspectRatio) {
    camera->aspectRatio = aspectRatio;
    lre_camera3D_update(camera);
}

LRE_CAMERA3D_STATICI void lre_camera3D_update(Camera3D *camera) {
    lre_camera3D_update_vectors(camera);
    lre_camera3D_update_view(camera);
    lre_camera3D_update_proj(camera);
}

LRE_CAMERA3D_STATICI void lre_camera3D_move(Camera3D *camera, enum Camera3DMovement direction, float deltaTime) {
    float velocity = camera->speed*deltaTime;
    vec3 pos;
    switch (direction) {
        case CAMERA3D_FORWARD:
            glm_vec3_scale(camera->front, velocity, pos);
            glm_vec3_add(camera->pos, pos, camera->pos);
            break;
        case CAMERA3D_BACKWARD:
            glm_vec3_scale(camera->front, velocity, pos);
            glm_vec3_sub(camera->pos, pos, camera->pos);
            break;
        case CAMERA3D_LEFT:
            glm_vec3_scale(camera->right, velocity, pos);
            glm_vec3_sub(camera->pos, pos, camera->pos);
            break;
        case CAMERA3D_RIGHT:
            glm_vec3_scale(camera->right, velocity, pos);
            glm_vec3_add(camera->pos, pos, camera->pos);
            break;
        case CAMERA3D_UP:
            glm_vec3_scale(camera->up, velocity, pos);
            glm_vec3_add(camera->pos, pos, camera->pos);
            break;
        case CAMERA3D_DOWN:
            glm_vec3_scale(camera->up, velocity, pos);
            glm_vec3_sub(camera->pos, pos, camera->pos);
            break;
    }
    // lre_camera3D_update_vectors(camera);
    // printf("Camera Pos {%f, %f, %f}\n",camera->pos[0],camera->pos[1],camera->pos[2]);
}

LRE_CAMERA3D_STATICI void lre_camera3D_rotate(Camera3D *camera, float xoffset, float yoffset, float deltaTime) {
    xoffset *= camera->sensitivity;
    yoffset *= camera->sensitivity;

    camera->yaw += xoffset;
    camera->pitch += yoffset;

    if (camera->pitch > 89.0f)
        camera->pitch = 89.0f;
    if (camera->pitch < -89.0f)
        camera->pitch = -89.0f;

    vec3 front;
    front[0] = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)); //x
    front[1] = sin(glm_rad(camera->pitch)); //y
    front[2] = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch)); //z

    glm_vec3_normalize(front);
    glm_vec3_copy(front, camera->front);
    
    // lre_camera3D_update_vectors(camera);
}

LRE_CAMERA3D_STATICI void lre_camera3D_update_vectors(Camera3D* camera) {
    glm_vec3_cross(camera->front, LRE_CAMERA3D_WORLD_UP, camera->right);
    glm_vec3_normalize(camera->right);
    glm_vec3_cross(camera->right, camera->front, camera->up);
    glm_vec3_normalize(camera->up);
}

LRE_CAMERA3D_STATICI void lre_camera3D_zoom(Camera3D *camera, float yoffset) {
    float fov = camera->fov;
    if (camera->fov >= 1.0f && camera->fov <= LRE_CAMERA3D_MAXFOV)
        camera->fov -= yoffset;
    if (camera->fov <= 1.0f)
        camera->fov = 1.0f;
    if (camera->fov >= LRE_CAMERA3D_MAXFOV)
        camera->fov = LRE_CAMERA3D_MAXFOV;
    if (camera->fov != fov)
        lre_camera3D_update_proj(camera);
}

#endif

#undef LRE_CAMERA3D_STATICI
#undef LRE_CAMERA3D_IMPLEMENTATION

#endif