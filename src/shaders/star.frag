#version 460 core

#define M_PI 3.1415926535897932384626433832795

out vec4 FragColor;
in vec3 VertColor;
in vec2 TexCoord;
in vec3 VertModelPos;

uniform bool has_texture = false;
uniform sampler2D body_texture;

void main() {
    if(has_texture){
        vec2 tex_coord = vec2((atan(VertModelPos.y, VertModelPos.x) / M_PI + 1.0) * 0.5,
                (asin(VertModelPos.z) / M_PI + 0.5));
        // tex_coord = vec2(1.0 - tex_coord.y, 1.0 - tex_coord.x);
        FragColor = vec4(texture(body_texture, tex_coord).rgb, 1.0f);
    } else {
        FragColor = vec4(VertColor.xyz, 1.0f);
    }
}
