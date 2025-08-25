#version 460 core

out vec4 FragColor;

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo_spec;

layout(std140, binding = 1) uniform LightingGlobals {
    float ambient_strength;
    vec3 camera_pos;
};

uniform vec3 color;
uniform float att_linear;
uniform float att_quadratic;
uniform vec3 light_source_pos;

in VS_OUT {
    noperspective vec2 TexCoords;
} fs_in;

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

    result += (diffuse + specular);
    FragColor = vec4(result, 1.0f);
}
