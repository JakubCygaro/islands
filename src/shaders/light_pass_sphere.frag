#version 460 core

out vec4 FragColor;

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo_spec;
uniform samplerCube shadow_map;

layout(std140, binding = 1) uniform LightingGlobals {
    float ambient_strength;
    vec3 camera_pos;
};

uniform vec3 color;
uniform float att_linear;
uniform float att_quadratic;
uniform vec3 light_source_pos;
uniform float far_plane;

in VS_OUT {
    noperspective vec2 TexCoords;
} fs_in;

vec3 sample_offset_directions[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float calculate_shadow(vec3 frag_pos, vec3 normal, vec3 light_dir){
    vec3 frag_to_light = frag_pos - light_source_pos;
    float current_depth = length(frag_to_light);
    int samples = 20;
    float bias = max(0.05 * (1.0 - dot(normal, light_dir)), 0.005);
    float view_distance = length(camera_pos - frag_pos);
    float disk_radius = (1.0 + (view_distance / far_plane)) / 25.0;
    float shadow = 0.0;
    for(int i = 0; i < samples; i++){
        float closest_depth = texture(shadow_map, frag_to_light + sample_offset_directions[i] * disk_radius).r;
        closest_depth *= far_plane;
        shadow += current_depth - bias > closest_depth ? 1.0 : 0.0;
    }
    shadow /= float(samples);
    return shadow;
}

void main(){
    vec2 ndc = fs_in.TexCoords;
    vec3 frag_pos = texture(g_position, ndc).rgb;
    vec3 norm = texture(g_normal, ndc).rgb;
    vec3 albedo = texture(g_albedo_spec, ndc).rgb;
    float specular_v = texture(g_albedo_spec, ndc).a;

    vec3 result = vec3(0);

    vec3 ambient = ambient_strength * albedo;
    result += ambient;

    vec3 view_dir = normalize(camera_pos - frag_pos);

    vec3 light_source_color = color;

    vec3 light_dir = normalize(light_source_pos - frag_pos);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = light_source_color * diff * albedo;

    vec3 halfway_dir = normalize(light_dir + view_dir);

    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(norm, halfway_dir), 0.0), specular_v); // 4 <- shininess value
    vec3 specular = 0.25 * light_source_color * spec;

    float distance = length(light_source_pos - frag_pos);
    float attenuation = 1.0 / (1.0 +
            att_linear * distance
            + att_quadratic * (distance * distance));

    diffuse *= attenuation;
    specular *= attenuation;

    float shadow = calculate_shadow(frag_pos, norm, light_dir);

    result += (1.0 - shadow) * specular + min(1.2 - shadow, 1.0) * diffuse;
    FragColor = vec4(result, 1.0f);
}
