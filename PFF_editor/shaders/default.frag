#version 450

layout(location = 0) in vec3 frag_color;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push {
	mat4 transform;			// projection * view * modle
	mat4 normal_matrix;
} push;

void main() {
  outColor = vec4(frag_color, 1.0);
}