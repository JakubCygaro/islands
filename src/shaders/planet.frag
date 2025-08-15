#version 460 core

out vec4 FragColor;

in LightData {
    vec3 FragPos;
    vec3 VertColor;
    vec3 Normal;
} light_data;


layout(std140, binding = 1) uniform LightingGlobals {
    float ambient_strength;
    vec3 camera_pos;
};

struct LightSource {
    vec4 position;
    vec4 color;
    float att_linear;
    float att_quadratic;
};

layout(std140, binding = 2) buffer LightSources {
    LightSource light_sources[];
};

void main() {

    vec3 ambient = ambient_strength * light_data.VertColor;

    vec3 result;

    vec3 norm = normalize(light_data.Normal);

    for(int i = 0; i < light_sources.length(); i++) {
        vec3 light_source_color = vec3(light_sources[i].color);
        vec3 light_source_pos = vec3(light_sources[i].position);

        vec3 light_dir = normalize(light_source_pos - light_data.FragPos);
        float diff = max(dot(norm, light_dir), 0.0);
        vec3 diffuse = diff * light_source_color;

        // vec3 view_dir = normalize(camera_pos - light_data.FragPos);
        // vec3 reflect_dir = reflect(-light_dir, norm);
        // float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32); // 32 <- shininess value
        // vec3 specular = 0.5 * spec * light_data.VertColor;
        // result = (ambient + diffuse + specular) * light_data.VertColor;

        float distance = length(light_source_pos - light_data.FragPos);
        float attenuation = 1.0 / (1.0 +
                light_sources[i].att_linear * distance
                + light_sources[i].att_quadratic * (distance * distance));

        diffuse *= attenuation;

        result += (diffuse) * light_data.VertColor;
    }
    result += ambient;


    FragColor = vec4(result, 1.0f);
    // FragColor = vec4(light_data.VertColor, 1.0f);
}
