#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out float lightIntensity;


layout (push_constant) uniform PushConstant {
    mat4 camera;
    mat4 model;
} pushConstant;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));
const float AMBIENT = 0.02;
const vec3 color = vec3(1.0, 1.0, 1.0);

void main() {
  gl_Position = pushConstant.camera * pushConstant.model * vec4(inPosition, 1.0);

  vec3 normalWorldSpace = normalize(mat3(transpose(inverse(pushConstant.model))) * inNormal);

  lightIntensity = AMBIENT + max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0);

  fragTexCoord = inTexCoord;

  
}

// gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);

// void main() {
    
//     // float waveAmplitude = 0.1; // adjust the amplitude of the wave
//     // float waveFrequency = 2.0; // adjust the frequency of the wave
//     // float wavePhase = 0.0; // adjust the phase of the wave

//     // vec3 waveOffset = vec3(waveAmplitude * sin(waveFrequency * inPosition.x + ubo.time + wavePhase), 0.0, waveAmplitude * sin(waveFrequency * inPosition.x + ubo.time + wavePhase));
//     gl_Position = pushConstant.camera * vec4(inPosition, 1.0);

    
//     fragNormal = normalize(inNormal);
//     fragPosition = inPosition;//vec3(pushConstant.model * vec4(inPosition, 1.0));
//     // fragNormal = mat3(transpose(inverse(pushConstant.model))) * normalize(inNormal);
//     fragTexCoord = inTexCoord;
// }


