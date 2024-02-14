#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 frag_color;

layout(set = 0, binding = 0) uniform global_ubo {
  mat4 projection_view_matrix;
  vec3 direction_to_light;
} ubo;

layout(push_constant) uniform Push {
	mat4 modle_matrix;
	mat4 normal_matrix;
} push;

const float AMBIENT = 0.02;

void main() {
  // gl_Position = vec4(push.modle_matrix * position + push.offset, 0.0, 1.0);
  gl_Position = ubo.projection_view_matrix * push.modle_matrix * vec4(position, 1.0);

  // TEMP: only works in certain situations
  //mat3 normal_matrix = transpose(inverse(mat3(push.modle_matrix)));
  //vec3 normal_world_space = normalize(normal_matrix * normal);
  
  vec3 normal_world_space = normalize(mat3(push.normal_matrix) * normal);
  float light_intensity = max(dot(normal_world_space, ubo.direction_to_light), AMBIENT);

  frag_color = light_intensity * color;
}