#version 460 core

layout (location = 0) in vec3 vert_pos;

uniform mat4 model;

layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
};

out VS_OUT {
    noperspective vec2 TexCoords;
} vs_out;

void main(){
    // vec4 ndc = projection * view * model * vec4(vert_pos, 1.0);
    // vec4 ndc = projection * view * model * vec4(vert_pos, 1.0);
    // vec2 ndc_tex_coords = vec2(min(ndc.x + 1.0, 1.0), min(ndc.y + 1.0, 1.0));
    // vs_out.TexCoords = ndc_tex_coords;
    // gl_Position = ndc;
    gl_Position = projection * view * model * vec4(vert_pos, 1.0);
    vs_out.TexCoords = vec2(
        gl_Position.x / gl_Position.w,
        gl_Position.y / gl_Position.w
    ) / 2.0 + 0.5;
    // gl_Position.z = 0.0;
}
