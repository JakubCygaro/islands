#version 460 core
layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec3 normal;

uniform mat4 model;

layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
};

out VS_OUT {
    vec3 normal;
} vs_out;

void main() {
    mat3 inverse_matrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = vec3(vec4(inverse_matrix * normal, 0.0));
    gl_Position = view * model * vec4(vert_pos, 1.0);
}
