#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo_spec;

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

layout(std140, binding = 2) restrict readonly buffer LightSources {
    LightSource light_sources[];
};

void main(){
    vec3 frag_pos = texture(g_position, TexCoords).rgb;
    vec3 norm = texture(g_normal, TexCoords).rgb;
    vec3 albedo = texture(g_albedo_spec, TexCoords).rgb;
    float specular = texture(g_albedo_spec, TexCoords).a;

    vec3 result = vec3(0);

    vec3 ambient = ambient_strength * albedo;
    result += ambient;

    vec3 view_dir = normalize(camera_pos - frag_pos);

    for(int i = 0; i < light_sources.length(); i++) {
        vec3 light_source_color = vec3(light_sources[i].color);
        vec3 light_source_pos = vec3(light_sources[i].position);

        vec3 light_dir = normalize(light_source_pos - frag_pos);
        float diff = max(dot(norm, light_dir), 0.0);
        vec3 diffuse = light_source_color * diff * albedo;

        vec3 halfway_dir = normalize(light_dir + view_dir);

        vec3 reflect_dir = reflect(-light_dir, norm);
        float spec = pow(max(dot(norm, halfway_dir), 0.0), 8.0); // 4 <- shininess value
        vec3 specular = 0.25 * light_source_color * spec;

        float distance = length(light_source_pos - frag_pos);
        float attenuation = 1.0 / (1.0 +
                light_sources[i].att_linear * distance
                + light_sources[i].att_quadratic * (distance * distance));

        diffuse *= attenuation;
        specular *= attenuation;

        result += (diffuse + specular);
    }
    // FragColor = vec4(result, 1.0f);
    FragColor = vec4(result, 1.0f);
}
