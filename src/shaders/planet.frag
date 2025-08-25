#version 460 core

layout (location = 0) out vec3 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec4 g_albedo_spec;

in LightData {
    vec3 FragPos;
    vec3 VertColor;
    vec3 Normal;
} light_data;


layout(std140, binding = 1) uniform LightingGlobals {
    float ambient_strength;
    vec3 camera_pos;
};

void main() {

    g_position = light_data.FragPos;
    g_normal = normalize(light_data.Normal);
    g_albedo_spec.rgb = light_data.VertColor;
    g_albedo_spec.a = 8.0;
}
