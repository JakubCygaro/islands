#version 460 core

out vec4 FragColor;

in LightData {
    vec3 FragPos;
    vec3 VertColor;
    vec3 Normal;
} light_data;


layout(std140, binding = 1) uniform LightingGlobals {
    vec4 ambient_color;
    float ambient_strength;
};

struct LightSource {
    vec4 position;
    vec4 color;
};

layout(std140, binding = 2) buffer LightSources {
    LightSource light_sources[];
};

void main() {

    vec3 ambient = ambient_strength * vec3(ambient_color);

    vec3 result;

    vec3 norm = normalize(light_data.Normal);

    for(int i = 0; i < light_sources.length(); i++) {
        vec3 light_dir = normalize(vec3(light_sources[i].position) - light_data.FragPos);
        float diff = max(dot(norm, light_dir), 0.0);
        vec3 diffuse = diff * vec3(light_sources[i].color);
        result = (ambient + diffuse) * light_data.VertColor;
    }


    FragColor = vec4(result, 1.0f);
}
