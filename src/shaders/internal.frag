#version 460 core
uniform sampler2D internal_buffer_texture;

in vec2 TexCoord;

out vec4 FragColor;

void main(){
    FragColor = texture(TexCoord, internal_buffer_texture);
}
