#version 450

// shader inputs from vertex shader
layout (location = 0) in vec3 in_color;
layout (location = 1) in vec2 in_UV;
layout (location = 2) in vec3 in_normal; // New input: face normal interpolated from the vertex shader

// output fragment color
layout (location = 0) out vec4 out_frag_color;

// texture to access
layout (set = 0, binding = 0) uniform sampler2D display_texture;

void main() {

    const vec3 sun = normalize(vec3(0.5, 1.0, 0.3));
    float diffuse = max(dot(normalize(in_normal), sun), 0.0);
    vec4 texColor = texture(display_texture, in_UV);
    out_frag_color = vec4(texColor.rgb * diffuse, texColor.a);
}
