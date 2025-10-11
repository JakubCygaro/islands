#version 460 core

out vec4 FragColor;
in vec3 VertColor;
in vec2 TexCoord;

uniform bool has_texture = false;
uniform sampler2D body_texture;

void main() {
    if(has_texture){
        FragColor = vec4(texture(body_texture, TexCoord).rgb, 1.0f);
    } else {
        FragColor = vec4(VertColor.xyz, 1.0f);
    }
}
