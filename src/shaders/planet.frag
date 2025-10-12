#version 460 core

#define M_PI 3.1415926535897932384626433832795

layout (location = 0) out vec3 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec4 g_albedo_spec;

uniform bool has_texture = false;
uniform sampler2D body_texture;

in LightData {
    vec3 FragPos;
    vec3 ModelVertPos;
    vec3 VertColor;
    vec3 Normal;
    vec2 TexCoord;
} light_data;


layout(std140, binding = 1) uniform LightingGlobals {
    float ambient_strength;
    vec3 camera_pos;
};

void main() {
    g_position = light_data.FragPos;
    g_normal = normalize(light_data.Normal);
    if(has_texture){
        vec2 tex_coord = vec2((atan(light_data.ModelVertPos.y, light_data.ModelVertPos.x) / M_PI + 1.0) * 0.5,
                (asin(light_data.ModelVertPos.z) / M_PI + 0.5));
        g_albedo_spec.rgb = texture(body_texture, tex_coord).rgb;
    } else {
        g_albedo_spec.rgb = light_data.VertColor;
    }
    g_albedo_spec.a = 8.0;
}
