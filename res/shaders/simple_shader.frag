#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in float lightIntensity;

layout(location = 0) out vec4 outColor;

// const vec3 lightPos = vec3(1.0,1.0,1.0);
// const vec3 lightColor = vec3(1.0,1.0,1.0); 
// const vec3 ambient = (0.11*lightColor);


void main() {

    outColor = texture(texSampler, fragTexCoord);
    outColor.rgb *= vec3(lightIntensity);
    
}
