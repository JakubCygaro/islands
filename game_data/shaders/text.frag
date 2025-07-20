#version 460 core

out vec4 FragColor;

in vec2 glyph_coord;

uniform sampler2D font_bitmap;

void main() {
    // FragColor = vec4(1.0, 1.0, 1.0, 0.5);
    FragColor = vec4(1.0, 1.0, 1.0, texture(font_bitmap, glyph_coord).r);
}
