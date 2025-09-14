#version 460 core
uniform sampler2D internal_buffer_texture;

in vec2 TexCoord;

out vec4 FragColor;

void main(){
    FragColor = texture(internal_buffer_texture, TexCoord);
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
