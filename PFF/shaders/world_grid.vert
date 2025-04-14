#version 450

layout(location = 0) out vec2 fragTexCoord;

// Match your GPU_scene_data structure
layout(set = 0, binding = 0) uniform SceneData {
    mat4 view;
    mat4 proj;
    mat4 proj_view;  // This is the critical matrix
    vec4 ambient_color;
    vec4 sunlight_direction;
    vec4 sunlight_color;
} scene_data;

// // Hardcoded grid vertices (XY plane at Z=0)
// const vec3 vertices[4] = vec3[4](
//     vec3(-1.0, 0.0, -1.0),  // Bottom left
//     vec3( 1.0, 0.0, -1.0),  // Bottom right
//     vec3( 1.0, 0.0,  1.0),  // Top right
//     vec3(-1.0, 0.0,  1.0)   // Top left
// );
// const int indices[6] = int[6](0,2,1,2,0,3);


void main() {

    vec3 vertices[4] = vec3[4](
        vec3(-1.0, 0.0, -1.0),  // Bottom left
        vec3(1.0, 0.0, -1.0),   // Bottom right
        vec3(-1.0, 0.0, 1.0),   // Top left
        vec3(1.0, 0.0, 1.0)     // Top right
    );

    gl_Position = scene_data.view * vec4(vertices[gl_VertexIndex], 1.0);
    fragTexCoord = vec2(gl_VertexIndex % 2, gl_VertexIndex / 2);

    // // Transform using proj_view matrix
    // gl_Position = scene_data.proj_view * vec4(vertices[gl_VertexIndex], 1.0);
    // fragTexCoord = vec2(vertices[gl_VertexIndex].x + 1.0, vertices[gl_VertexIndex].z + 1.0) * 0.5;
}
