#version 450

#extension GL_GOOGLE_include_directive : require

#include "input_structures.glsl"

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

void main() {

    vec3 N = normalize(inNormal);
    vec3 lightDir = normalize(scene_data.sunlight_direction.xyz);
    float diff = max(dot(N, lightDir), 0.0);
    vec3 color = inColor * texture(color_tex, inUV).xyz;
    
    // Light components
    vec3 ambient = color * scene_data.ambient_color.xyz;
    vec3 lightColor = scene_data.sunlight_color.xyz * scene_data.sunlight_color.w;
    vec3 diffuse = lightColor * diff * color;
    
    vec3 finalColor = color * (0.1 + diff);
    vec3 gammaCorrected = pow(finalColor, vec3(1.0 / 2.2));
    outFragColor = vec4(gammaCorrected, 1.0);
}
