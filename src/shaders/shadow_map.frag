#version 460 core

in vec4 FragPos;

uniform float far_plane;
uniform vec3 current_light_pos;

layout(std140, binding = 1) uniform LightingGlobals {
    float ambient_strength;
    vec3 camera_pos;
};

void main() {
    float light_dist = length(FragPos.xyz - current_light_pos);

    light_dist = light_dist / far_plane;

    gl_FragDepth = light_dist;
}
