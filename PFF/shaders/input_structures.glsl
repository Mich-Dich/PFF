layout(set = 0, binding = 0) uniform global_scene_data{

	mat4 view;
	mat4 proj;
	mat4 viewproj;
	vec4 ambient_color;
	vec4 sunlight_direction; //w for sun power
	vec4 sunlight_color;
} scene_data;

layout(set = 1, binding = 0) uniform GLTF_material_data{   

	vec4 color_factors;
	vec4 metal_rough_factors;
	int color_tex_ID;
	int metalrough_tex_ID;
	
} material_data;

layout(set = 1, binding = 1) uniform sampler2D color_tex;
layout(set = 1, binding = 2) uniform sampler2D metal_rough_tex;