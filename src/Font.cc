#include "Font.hpp"
#include "shader/Shader.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace {
    const char* VERT =
        "#version 460\n"
        "layout (location = 0) in vec2 position;\n"
        "layout (location = 1) in vec2 tex_coords;\n"
        "uniform mat4 projection;\n"
        "out vec2 texture_coords;\n"
        "void main(){\n"
        "   vec4 pos = projection * vec4(position.xy, 0, 1.0);\n"
        "   gl_position = vec4(pos.x, pos.y, 0.0, 1.0);\n"
        "   texture_coords = tex_coords;\n"
        "}"
        ;
    const char* FRAG =
        "#version 460\n"
        "out vec4 FragColor;\n"
        "in vec2 texture_coords;\n"
        "in vec2 texture_coords;\n"
        "uniform sampler2D glyph_texture;\n"
        "void main(){\n"
        "   FragColor = texture(glyph_texture, texture_coords);\n"
        "   texture_coords = tex_coords;\n"
        "}"
        ;
    uint32_t gen_font_bitmap(const std::vector<font::Character>& chars, const uint32_t& glyph_width, const uint32_t& glyph_height){
        auto glyphs_x = static_cast<uint32_t>(std::sqrt(chars.size()));
        uint32_t glyphs_y = glyphs_x + 1;
        uint32_t font_bitmap;
        glGenTextures(1, &font_bitmap);
        glBindTexture(GL_TEXTURE_2D, font_bitmap);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            glyphs_x * glyph_width,
            glyphs_y * glyph_height,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            NULL
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        uint32_t fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, font_bitmap, 0);

        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, glyphs_x, glyphs_y);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
            throw std::runtime_error("Failed to complete the framebuffer");
        }
        auto glyph_shader = Shader(VERT, FRAG);

        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        uint32_t glyph_vbo = 0;
        uint32_t glyph_vao = 0;
        uint32_t glyph_ebo = 0;
        glGenBuffers(1, &glyph_vbo);
        glGenVertexArrays(1, &glyph_vao);
        glBindVertexArray(glyph_vao);
        glBindBuffer(GL_ARRAY_BUFFER, glyph_vbo);
        glBufferData(GL_ARRAY_BUFFER, 4 * (sizeof(glm::vec2)) + sizeof(glm::vec2), NULL, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
            (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &glyph_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glyph_ebo);
        uint32_t ebo[] = {
            1, 2, 3,
            2, 4, 3
        };
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ebo), ebo, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glm::vec2 glyph_data[] = {
            {0, 0}, {0, 1}, // top-left
            {0, 0}, {0, 0}, // bottom-left
            {0, 0}, {1, 1}, // top-right
            {0, 0}, {1, 0}, // bottom-right
        };
        auto projection = glm::ortho(0.0f, static_cast<float>(glyph_width),
                0.0f, static_cast<float>(glyph_height), 0.0f, 1.0f);
        glyph_shader.use_shader();
        glyph_shader.set_mat4("projection", projection);
        glBindVertexArray(glyph_vao);
        glActiveTexture(GL_TEXTURE0);

        for(size_t i = 0; i < chars.size(); i++){
            size_t x = i % glyphs_x;
            size_t y = static_cast<size_t>(i / glyphs_x);
            auto& glyph = chars[i];
            glyph_data[0] = { x * glyph_width, y * glyph_height };
            glyph_data[1] = { x * glyph_width, (y + 1) * glyph_height };
            glyph_data[2] = { (x + 1) * glyph_width, y * glyph_height };
            glyph_data[3] = { (x + 1) * glyph_width, (y + 1) * glyph_height };
            glBindBuffer(GL_ARRAY_BUFFER, glyph_vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(glyph_data), glyph_data, GL_DYNAMIC_DRAW);
            glBindTexture(GL_TEXTURE_2D, glyph.texture_id);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glDeleteTextures(1, glyph.texture_id);
        }
        glDeleteRenderbuffers(1, &rbo);
        glDeleteFramebuffers(1, &fbo);
        glDeleteVertexArrays(1, &glyph_vao);
        glDeleteBuffers(1, &glyph_vbo);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return font_bitmap;
    }
}
namespace font{
    void load_font(const std::string& file_path, int font_size){
        FT_Library ft;
        if(FT_Init_FreeType(&ft)){
            throw std::runtime_error("Failed to initialize freetype");
        }

        FT_Face face;
        if(FT_New_Face(ft, file_path.c_str(), 0, &face)){
            std::stringstream ss;
            ss << "Failed to load font file: '";
            ss << file_path << '\'';
            throw std::runtime_error(ss.str());
        }
        if(FT_Set_Pixel_Sizes(face, 0, font_size)){
            throw std::runtime_error("Failed to set pixel size for font");
        }
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        uint32_t max_glyph_width{}, max_glyph_height{};
        std::vector<Character> characters{};
        for(int ascii_code = 32; ascii_code < 128; ascii_code++){
            if(FT_Load_Char(face, (char)ascii_code, FT_LOAD_RENDER)){
                std::stringstream ss;
                ss << "Failed to load font glyph: '";
                ss << (char)ascii_code << '\'';
                ss << "for font: " << '\'' << file_path << '\'';
                throw std::runtime_error(ss.str());
            }
            max_glyph_width = std::max(face->glyph->bitmap.width, max_glyph_width);
            max_glyph_height = std::max(face->glyph->bitmap.rows, max_glyph_height);
            // if(face->glyph->bitmap.width > width) width = face->glyph->bitmap.width;
            // if(face->glyph->bitmap.rows > height) height = face->glyph->bitmap.rows;
            // generate texture
            uint32_t texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            font::Character ch{
                .texture_id = texture,
                .size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                .bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                .advance = (uint32_t)face->glyph->advance.x,
            };
            characters.emplace_back(std::move(ch));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        gen_font_bitmap(characters, max_glyph_width, max_glyph_height);
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }
    FontBitmap::FontBitmap(uint32_t bitmap, uint32_t glyphs_x, uint32_t glyphs_y,
            uint32_t glyph_width, uint32_t glyph_height) :
        bitmap_id(bitmap),
        glyphs_x(glyphs_x),
        glyphs_y(glyphs_y),
        glyph_width(glyph_width),
        glyph_height(glyph_height)
    {
        total_x = glyphs_x * glyph_width;
        total_y = glyphs_y * glyph_height;
    }
    FontBitmap::FontBitmap(const FontBitmap& other):
        bitmap_id(other.bitmap_id),
        glyphs_x(other.glyphs_x),
        glyphs_y(other.glyphs_y),
        glyph_width(other.glyph_width),
        glyph_height(other.glyph_height),
        total_x(other.total_x),
        total_y(other.total_y)
    {}
    FontBitmap& FontBitmap::operator=(const FontBitmap& other) {
        bitmap_id = other.bitmap_id;
        glyphs_x = other.glyphs_x;
        glyphs_y = other.glyphs_y;
        glyph_width = other.glyph_width;
        glyph_height = other.glyph_height;
        total_x = other.total_x;
        total_y = other.total_y;
        return *this;

    }
    FontBitmap::FontBitmap(FontBitmap&& other):
        bitmap_id(other.bitmap_id),
        glyphs_x(other.glyphs_x),
        glyphs_y(other.glyphs_y),
        glyph_width(other.glyph_width),
        glyph_height(other.glyph_height),
        total_x(other.total_x),
        total_y(other.total_y)
    {
        other.bitmap_id = 0;
        other.glyphs_x = 0;
        other.glyphs_y = 0;
        other.glyph_width = 0;
        other.glyph_height = 0;
        other.total_x = 0;
        other.total_y = 0;
    }
    FontBitmap& FontBitmap::operator=(FontBitmap&& other){
        bitmap_id = other.bitmap_id;
        glyphs_x = other.glyphs_x;
        glyphs_y = other.glyphs_y;
        glyph_width = other.glyph_width;
        glyph_height = other.glyph_height;
        total_x = other.total_x;
        total_y = other.total_y;
        other.bitmap_id = 0;
        other.glyphs_x = 0;
        other.glyphs_y = 0;
        other.glyph_width = 0;
        other.glyph_height = 0;
        other.total_x = 0;
        other.total_y = 0;
        return *this;
    }
    FontBitmap::~FontBitmap(){
        if(bitmap_id)
            glDeleteTextures(1, &bitmap_id);
    }

    FontBitmap::GlyphTextureCoordinates FontBitmap::texture_coords_for(char ch) const{
        auto idx = static_cast<int>(ch - 'a');
        auto x = idx % glyphs_x;
        auto y = idx / glyphs_y;

    }
    void FontBitmap::bind_bitmap() const {
        glBindTexture(GL_TEXTURE_2D, bitmap_id);
    }
    void FontBitmap::unbind_bitmap() const{
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
