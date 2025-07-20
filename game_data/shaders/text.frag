#version 460 core

out vec4 FragColor;
// in vec3 VertColor;

in vec2 glyph_coord;

uniform sampler2D font_bitmap;

void main() {
    // vec4 sampled = vec4(1.0, 1.0, 1.0, texture(font_bitmap, glyph_coord).r);
    // vec4 sampled = vec4(texture(font_bitmap, glyph_coord).rgb, 1.0);
    // float a = texture(font_bitmap, glyph_coord).r;
    // FragColor = vec4(1.0, 1.0, 1.0, a);
    // FragColor = vec4(1.0, 1.0, 1.0, 1.0f) * sampled;
    // FragColor = vec4(0.5, 0.2, 0.3, 0.0f);
    FragColor = vec4(1.0, 1.0, 1.0, texture(font_bitmap, glyph_coord).r);
    // FragColor = vec4(1.0);
}
