#version 450

#extension GL_GOOGLE_include_directive : require
#extension GL_EXT_buffer_reference : require

#include "input_structures.glsl"

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec3 outColor;
layout (location = 2) out vec2 outUV;

struct Vertex {

	vec3 position;
	float uv_x;
	vec3 normal;
	float uv_y;
	vec4 color;
}; 

layout(buffer_reference, std430) readonly buffer VertexBuffer{ 
	
	Vertex vertices[];
};

//push constants block
layout( push_constant ) uniform constants{

	mat4 render_matrix;
	VertexBuffer vertex_buffer;
} PushConstants;

void main() {

	Vertex v = PushConstants.vertex_buffer.vertices[gl_VertexIndex];
	vec4 position = vec4(v.position, 1.0f);
	gl_Position =  scene_data.viewproj * PushConstants.render_matrix * position;
	mat3 normalMatrix = transpose(inverse(mat3(PushConstants.render_matrix)));
	outNormal = normalize(normalMatrix * v.normal);

	// outColor = v.color.xyz;
	outColor = material_data.color_factors.xyz;
	outUV.x = v.uv_x;
	outUV.y = v.uv_y;
}