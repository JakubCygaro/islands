#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 offset;

uniform mat4 model;

layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
    mat4 text_projection;
};

layout(std140, binding = 1) uniform LightingGlobals {
    float ambient_strength;
    vec3 camera_pos;
};

out VS_OUT {
    float grid_alpha;
};

void main(){
    vec4 world_pos = model * vec4(pos, 1.0);
    world_pos.x += offset.x;
    world_pos.z += offset.y;
    // grid_alpha = (length(vec3(world_pos) - camera_pos) / 4.0);
    grid_alpha = 1.0;
    gl_Position = projection * view * world_pos;
}
