#version 460 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 coord;

out vec2 TexCoord;
void main(){
    TexCoord = coord;
    gl_Position = vec4(pos, 0.0, 1.0);
}
