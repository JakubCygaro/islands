#version 460 core

out vec4 FragColor;

in vec2 texture_coords;

uniform sampler2D glyph_texture;

void main(){
   // FragColor = texture(glyph_texture, texture_coords);
    FragColor = vec4(texture(glyph_texture, texture_coords).r);
   // FragColor = vec4(1.0);
}
