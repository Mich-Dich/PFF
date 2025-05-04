#version 450

#extension GL_GOOGLE_include_directive : require
#include "input_structures.glsl"

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inUV;

layout (location = 0) out vec4 outFragColor;

void main() 
{
	float lightValue = max(dot(inNormal, scene_data.sunlight_direction.xyz), 0.1f);

	vec3 color = inColor;
	vec3 ambient = color *  scene_data.ambient_color.xyz;

	outFragColor = vec4(color * lightValue *  scene_data.sunlight_color.w + ambient ,1.0f);
}