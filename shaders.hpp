
    /* PYTHON AUTOGEN */
    namespace shaders {
        
            inline constexpr const char* C_BODY_FRAG = R"$(
                    #version 460 core
out vec4 FragColor;
in vec3 VertColor;
void main() {
    FragColor = vec4(VertColor.xyz, 1.0f);
}
)$";
        
            inline constexpr const char* TEST_VERT = R"$(
                    #version 330 core
layout (location = 0) in vec3 aPos;
void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
)$";
        
            inline constexpr const char* PLANET_FRAG = R"$(
                    #version 460 core
out vec4 FragColor;
in vec3 VertColor;
void main() {
    FragColor = vec4(VertColor.xyz, 1.0f);
}
)$";
        
            inline constexpr const char* FONT_BITMAP_FRAG = R"$(
                    #version 460 core
out vec4 FragColor;
in vec2 texture_coords;
uniform sampler2D glyph_texture;
void main(){
    FragColor = vec4(texture(glyph_texture, texture_coords).r);
}
)$";
        
            inline constexpr const char* PLANET_VERT = R"$(
                    #version 460 core
layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec3 normal;
uniform mat4 model;
uniform vec3 color;
layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
};
out vec3 VertColor;
void main() {
    VertColor = color;
    gl_Position = projection * view * model * vec4(vert_pos, 1.0);
}
)$";
        
            inline constexpr const char* TEXT_FRAG = R"$(
                    #version 460 core
out vec4 FragColor;
in vec2 glyph_coord;
uniform sampler2D font_bitmap;
uniform vec3 color;
void main() {
    FragColor = vec4(color.rgb, texture(font_bitmap, glyph_coord).r);
}
)$";
        
            inline constexpr const char* FONT_BITMAP_VERT = R"$(
                    #version 460 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 tex_coords;
uniform mat4 projection;
out vec2 texture_coords;
void main(){
   vec4 pos = projection * vec4(position.xy, -1.0, 1.0);
   texture_coords = tex_coords;
   gl_Position = vec4(pos.x, pos.y, -1.0, 1.0);
}
)$";
        
            inline constexpr const char* C_BODY_VERT = R"$(
                    #version 460 core
layout (location = 0) in vec3 vert_pos;
layout (location = 1) in vec3 normal;
uniform mat4 model;
uniform vec3 color;
layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
};
out vec3 VertColor;
void main() {
    VertColor = color;
    gl_Position = projection * view * model * vec4(vert_pos, 1.0);
}
)$";
        
            inline constexpr const char* TEXT_VERT = R"$(
                    #version 460 core
layout (location = 0) in vec2 vert_pos;
layout (location = 1) in vec2 coord;
layout(std140, binding = 0) uniform Matrices {
    mat4 view;
    mat4 projection;
    mat4 text_projection;
};
out vec2 glyph_coord;
uniform mat4 model;
void main() {
    glyph_coord = coord;
    gl_Position = text_projection * model *vec4(vert_pos, 0.0, 1.0);
}
)$";
        
            inline constexpr const char* TEST_FRAG = R"$(
                    #version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
)$";
        
    }
    