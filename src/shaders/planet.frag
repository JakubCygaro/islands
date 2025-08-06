#version 460 core

out vec4 FragColor;
in vec3 VertColor;

layout(std140, binding = 1) uniform LightingGlobals {
    vec4 ambient_color;
    float ambient_strength;
};

struct LightSource {
    vec4 color;
    vec4 position;
};

layout(std140, binding = 2) uniform LightSources {
    LightSource light_sources[];
};

void main() {

    vec3 ambient = ambient_strength * vec3(ambient_color);

    vec3 result = ambient * VertColor;

    FragColor = vec4(result, 1.0f);
}
