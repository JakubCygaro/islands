#version 460 core

layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform vec3 color;

layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
    mat4 text_projection;
};

out vec3 VertColor;
void main() {
    VertColor = color;
    gl_Position = projection * view * model * vec4(vert_pos, 1.0);
}
