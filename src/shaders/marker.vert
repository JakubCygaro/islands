#version 460 core

layout(location = 0) in vec3 pos;

uniform mat4 model;

layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
    mat4 text_projection;
};

layout(std140, binding = 1) uniform LightingGlobals {
    float ambient_strength;
    vec3 camera_pos;
};

void main() {
    gl_Position = projection * view * model * vec4(pos, 1.0);
    // gl_Position = projection * view * vec4(pos, 0.0);
}
