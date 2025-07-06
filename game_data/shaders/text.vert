#version 460 core

layout (location = 0) in vec2 vert_pos;
layout (location = 1) in vec2 coord;

//uniform mat4 view;
//uniform mat4 projection;

uniform mat4 model;
// uniform vec3 color;

layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
    mat4 text_projection;
};

out vec2 glyph_coord;
// out vec3 VertColor;
void main() {
    glyph_coord = coord;
    gl_Position = projection * vec4(vert_pos, 0.0, 1.0);
}
