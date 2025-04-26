#version 460 core

layout (location = 0) in vec3 vert_pos;

//uniform mat4 view;
//uniform mat4 projection;

uniform mat4 model;

layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
}

void main() {
    gl_Position = projection * view * model * vec4(vert_pos, 1.0);
}
