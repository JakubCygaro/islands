#version 460 core

out vec4 FragColor;
in vec3 VertColor;

in vec2 glyph_coord;

uniform sampler2D font_bitmap;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(font_bitmap, glyph_coord).r);
    FragColor = vec4(1.0, 1.0, 1.0, 1.0f);
}
