#version 450
#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec3 out_color;
layout (location = 1) out vec2 out_UV;
layout (location = 2) out vec3 out_normal;

struct vertex {

	vec3 position;
	float uv_x;
	vec3 normal;
	float uv_y;
	vec4 color;
}; 

layout(buffer_reference, std430) readonly buffer vertex_buffer { 
	vertex vertices[];
};

// push constant
layout( push_constant ) uniform constants {

	mat4 render_matrix;
	vertex_buffer vertex_buffer;

} push_constant;

void main() {	

    vertex v = push_constant.vertex_buffer.vertices[gl_VertexIndex];
    gl_Position = push_constant.render_matrix * vec4(v.position, 1.0f);
    out_color = v.color.xyz;
    out_UV = vec2(v.uv_x, v.uv_y);
    out_normal = normalize(v.normal);
}