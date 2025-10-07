#version 460 core

layout (location = 0) in vec2 vert_pos;
layout (location = 1) in vec2 coord;

layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
    mat4 text_projection;
};

uniform mat4 model;
uniform mat3 scaling;
uniform float width;
out vec2 glyph_coord;

// out vec3 VertColor;
void main() {
    glyph_coord = coord;
    mat4 model_view = view * model;
    // https://stackoverflow.com/a/66951138
    // https://stackoverflow.com/a/5487981
    // we basically need to nuke the Rotation part of the matrix,
    // but since we want to preserve the scaling, the indentity matrix
    // (that goes in the place of the Rotation section) has to be scaled
    // and put it the place of the Rotation matrix
    // I hope this shit makes sense in 10 years
    model_view[0].xyz = scaling[0];
    model_view[1].xyz = scaling[1];

    // vert_pos.x -= width / 2.0;

    vec4 viewspace = model_view * vec4(vert_pos.x - (width / 2.0), vert_pos.y, 0.0, 1.0);

    gl_Position = projection * viewspace;
    // gl_Position = projection * view * model * vec4(vert_pos, 0.0, 1.0);
}
