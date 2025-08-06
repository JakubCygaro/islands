#version 460 core

layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec3 normal;

//uniform mat4 view;
//uniform mat4 projection;

uniform mat4 model;
uniform mat3 inverse_matrix;
uniform vec3 color;

layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
};

out LightData {
    vec3 FragPos;
    vec3 VertColor;
    vec3 Normal;
} light_data;

void main() {
    light_data.VertColor = color;
    light_data.Normal = inverse_matrix * normal;
    vec4 world_pos = model * vec4(vert_pos, 1.0);
    light_data.FragPos = vec3(world_pos);
    gl_Position = projection * view * world_pos;
}
