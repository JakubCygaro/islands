#version 460 core

layout (points) in;
layout (line_strip, max_vertices = 2) out;

layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
    mat4 text_projection;
};

in VS_OUT {
    float radius;
    vec3 move_vector;
} gs_in[];

void main() {
    gl_Position = projection * view * gl_in[0].gl_Position;
    EmitVertex();

    gl_Position = projection * view * (gl_in[0].gl_Position + (vec4(gs_in[0].move_vector, 0.0) * (1.0 + gs_in[0].radius)));
    EmitVertex();

    EndPrimitive();
}
