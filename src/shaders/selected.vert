#version 460 core

layout(location = 0) in vec3 center_pos;

uniform vec3 move_vector;
uniform mat4 model;
uniform float radius;

layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
    mat4 text_projection;
};

out VS_OUT {
    float radius;
    vec3 move_vector;
} vs_out;

void main() {
    gl_Position = model * vec4(0.0, 0.0, 0.0, 1.0);
    vs_out.radius = radius;
    vs_out.move_vector = normalize(move_vector);
}
