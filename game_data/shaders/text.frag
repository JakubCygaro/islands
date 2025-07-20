#version 460 core

out vec4 FragColor;

in vec2 glyph_coord;

uniform sampler2D font_bitmap;
uniform vec3 color;

void main() {
    // FragColor = vec4(1.0, 1.0, 1.0, 0.5);
    FragColor = vec4(color.rgb, texture(font_bitmap, glyph_coord).r);
}
