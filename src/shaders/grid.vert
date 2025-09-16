#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 offset;

uniform mat4 model;

layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
    mat4 text_projection;
};

void main(){
    gl_Position = projection * view * model * vec4(pos.x + offset.x, 0.0, pos.z + offset.y, 1.0);
}
