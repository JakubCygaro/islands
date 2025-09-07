#version 460 core
layout (location = 0) in vec3 pos;

uniform mat4 model;

layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
};

void main(){
    gl_Position = model * vec4(pos, 1.0);
}
