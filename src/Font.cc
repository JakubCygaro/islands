#include "Font.hpp"
#include <cstdio>
#include <stb_image/stb_image.h>
#include "shader/Shader.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <string>

namespace {
    /// This is the actual implementation of the load_font function.
    /// It iterates through all the characters and renders them onto a texture in a separate framebuffer object.
    /// It then returns a font::FontBitmap object that contains a pointer to that texture and handles access to it.
    font::FontBitmap gen_font_bitmap(const std::vector<font::Character>& chars, const uint32_t& glyph_width, const uint32_t& glyph_height, char first, char last){
        auto glyphs_x = static_cast<uint32_t>(std::sqrt(chars.size()));
        uint32_t glyphs_y = glyphs_x + 1;
        uint32_t font_bitmap;
        uint32_t bitmap_width{}, bitmap_height{};

        bitmap_width = glyphs_x * glyph_width;
        bitmap_height = glyphs_y * glyph_height;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &font_bitmap);
        glBindTexture(GL_TEXTURE_2D, font_bitmap);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_R8,
            bitmap_width,
            bitmap_height,
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
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, font_bitmap, 0);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
            throw std::runtime_error("Failed to complete the framebuffer");
        }

        GLint gl_viewport[4];
        glGetIntegerv(GL_VIEWPORT, gl_viewport);
        glViewport(0, 0, bitmap_width, bitmap_height);

        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        uint32_t glyph_vbo = 0;
        uint32_t glyph_vao = 0;
        uint32_t glyph_ebo = 0;

        glGenBuffers(1, &glyph_vbo);
        glGenVertexArrays(1, &glyph_vao);

        glBindVertexArray(glyph_vao);
        glBindBuffer(GL_ARRAY_BUFFER, glyph_vbo);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec2), NULL, GL_DYNAMIC_DRAW);

        //position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
            (void*)0);
        //texture coords
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glGenBuffers(1, &glyph_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glyph_ebo);
        uint32_t ebo[] = {
            0, 1, 3,
            3, 2, 0
        };
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ebo), ebo, GL_STATIC_DRAW);

        glm::vec2 glyph_data[] = {
            {0, 0}, {0, 1}, // top-left
            {0, 0}, {0, 0}, // bottom-left
            {0, 0}, {1, 1}, // top-right
            {0, 0}, {1, 0}, // bottom-right
        };
        auto projection = glm::ortho(0.0f, static_cast<float>(bitmap_width), static_cast<float>(bitmap_height),
                0.0f);

        glBindVertexArray(glyph_vao);
        auto glyph_shader = Shader::from_shader_dir("font_bitmap");
        glyph_shader.use_shader();
        glyph_shader.set_mat4("projection", projection);


        for(size_t i = 0; i < chars.size(); i++){
            size_t x = i % glyphs_x;
            size_t y = static_cast<size_t>(i / glyphs_x);

            auto& glyph = chars[i];
            //fill in the position data of the glyph in the buffer
            glyph_data[0] = { x * glyph_width, y * glyph_height };
            glyph_data[2] = { x * glyph_width, (y + 1) * glyph_height };
            glyph_data[4] = { (x + 1) * glyph_width, y * glyph_height };
            glyph_data[6] = { (x + 1) * glyph_width, (y + 1) * glyph_height };

            glBindBuffer(GL_ARRAY_BUFFER, glyph_vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glyph_data), glyph_data);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, glyph.texture_id);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glDeleteTextures(1, &glyph.texture_id);
        }
        // glReadBuffer(GL_COLOR_ATTACHMENT0);
        // std::vector<unsigned char> pixels(3 * bitmap_width * bitmap_height);
        // glPixelStorei(GL_PACK_ALIGNMENT, 1);
        // glReadPixels(0, 0, bitmap_width, bitmap_height, GL_RED, GL_UNSIGNED_BYTE, pixels.data());
        //
        // bool any_data = std::any_of(pixels.begin(), pixels.end(), [](unsigned char v){return v != 0;});
        // if(!any_data) throw std::runtime_error("framebuffer empty");

        // post rendering cleanup
        glDeleteFramebuffers(1, &fbo);
        glDeleteVertexArrays(1, &glyph_vao);
        glDeleteBuffers(1, &glyph_vbo);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // set the viewport to the previous value
        glViewport(0, 0, gl_viewport[2], gl_viewport[3]);
        glEnable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        std::printf("glyphs_x: %d, glyphs_y: %d\n", glyphs_x, glyphs_y);
        return font::FontBitmap(font_bitmap, glyphs_x, glyphs_y, glyph_width, glyph_height, first, last);
    }
}
namespace font{
    /// This function loads a .ttf font file under the file_path and returns a FontBitmap wrapper object.
    font::FontBitmap load_font(const std::string& file_path, int font_size){
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
        auto start_char = ' ', last_char = 'z';
        for(int ascii_code = start_char; ascii_code <= (int)last_char; ascii_code++){
            if(FT_Load_Char(face, (char)ascii_code, FT_LOAD_RENDER)){
                std::stringstream ss;
                ss << "Failed to load font glyph: '";
                ss << (char)ascii_code << '\'';
                ss << "for font: " << '\'' << file_path << '\'';
                throw std::runtime_error(ss.str());
            }
            max_glyph_width = std::max(face->glyph->bitmap.width, max_glyph_width);
            max_glyph_height = std::max(face->glyph->bitmap.rows, max_glyph_height);

            // generate texture
            uint32_t texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_R8,
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
        auto bitmap = gen_font_bitmap(characters, max_glyph_width, max_glyph_height, start_char, last_char);
        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        return bitmap;
    }
    FontBitmap::FontBitmap(uint32_t bitmap, uint32_t glyphs_x, uint32_t glyphs_y,
            uint32_t glyph_width, uint32_t glyph_height, char first, char last) :
        bitmap_id(bitmap),
        glyphs_x(glyphs_x),
        glyphs_y(glyphs_y),
        glyph_width(glyph_width),
        glyph_height(glyph_height),
        first_char(first),
        last_char(last)
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
        total_y(other.total_y),
        first_char(other.first_char),
        last_char(other.last_char)
    {}
    FontBitmap& FontBitmap::operator=(const FontBitmap& other) {
        bitmap_id = other.bitmap_id;
        glyphs_x = other.glyphs_x;
        glyphs_y = other.glyphs_y;
        glyph_width = other.glyph_width;
        glyph_height = other.glyph_height;
        total_x = other.total_x;
        total_y = other.total_y;
        first_char = other.first_char;
        last_char = other.last_char;
        return *this;

    }
    FontBitmap::FontBitmap(FontBitmap&& other):
        bitmap_id(other.bitmap_id),
        glyphs_x(other.glyphs_x),
        glyphs_y(other.glyphs_y),
        glyph_width(other.glyph_width),
        glyph_height(other.glyph_height),
        total_x(other.total_x),
        total_y(other.total_y),
        first_char(other.first_char),
        last_char(other.last_char)
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
        first_char = other.first_char;
        last_char = other.last_char;
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
        if (ch > last_char)
            throw std::runtime_error("tried to access a character that is outside the texture bitmap");

        auto idx = static_cast<int>(ch - first_char);

        if(idx < 0)
            throw std::runtime_error("tried to access a character that is outside the texture bitmap");
        auto x = idx % glyphs_x;
        auto y = idx / glyphs_y;

        auto w_prc = (float)glyph_width / (float)total_x;
        auto h_prc = (float)glyph_height / (float)total_y;
        return GlyphTextureCoordinates {
            .top_left = { x * w_prc, y * h_prc },
            .top_right = { (x + 1) * w_prc, y * h_prc },
            .bottom_left = { x * w_prc, (y + 1) * h_prc },
            .bottom_right { (x + 1) * w_prc, (y + 1) * h_prc }
        };

    }
    void FontBitmap::bind_bitmap() const {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bitmap_id);
    }
    void FontBitmap::unbind_bitmap() const{
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    Text2D::Text2D(){}
    Text2D::Text2D(std::shared_ptr<FontBitmap> font_bitmap, std::shared_ptr<Shader> text_shader):
        m_text_shader{text_shader},
        m_font_bitmap{font_bitmap}
    {
        // m_str = "";
    }
    Text2D::Text2D(const Text2D& other):
        // m_str{other.m_str},
        m_text_shader{other.m_text_shader},
        m_font_bitmap{other.m_font_bitmap},
        m_letter_data{other.m_letter_data},
        m_pos{other.m_pos}
    {

    }
    Text2D& Text2D::operator=(const Text2D& other){
        // m_str = other.m_str;
        m_text_shader = other.m_text_shader;
        m_font_bitmap = other.m_font_bitmap;
        m_letter_data = other.m_letter_data;
        m_pos = other.m_pos;
        return *this;
    }
    Text2D::Text2D(Text2D&& other):
        // m_str{other.m_str},
        m_text_shader{other.m_text_shader},
        m_font_bitmap{other.m_font_bitmap},
        m_letter_data{other.m_letter_data},
        m_pos{other.m_pos}
    {
        // other.m_str = nullptr;
        other.m_text_shader = nullptr;
        other.m_font_bitmap = nullptr;
        m_letter_data.clear();
    }
    Text2D& Text2D::operator=(Text2D&& other){
        // m_str = other.m_str;
        m_text_shader = other.m_text_shader;
        m_font_bitmap = other.m_font_bitmap;
        m_letter_data = other.m_letter_data;
        m_pos = other.m_pos;

        // other.m_str = nullptr;
        other.m_text_shader = nullptr;
        other.m_font_bitmap = nullptr;
        other.m_letter_data.clear();

        return *this;
    }
    void Text2D::update() {
        // std::string::const_iterator str_iter = m_str.begin();
        // auto pos = m_pos;
        //
        // for(char c = *str_iter; str_iter != m_str.end(); str_iter++){
        //     auto glypth_coord = m_font_bitmap->texture_coords_for(c);
        // }
    }
    void Text2D::draw() const {

        glDisable(GL_DEPTH_TEST);
        float data[] = {
            0.0, 0.0, 0.0, 0.0,
            0.0, 300.0, 0.0, 1.0,
            600.0, 300.0, 1.0, 1.0,
            600.0, 300.0, 1.0, 1.0,
            600.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 0.0,
        };

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        uint32_t vbo{}, vao{};
        glGenBuffers(1, &vbo);
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
            (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(vao);
        m_font_bitmap->bind_bitmap();
        m_text_shader->use_shader();

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_font_bitmap->unbind_bitmap();
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glEnable(GL_DEPTH_TEST);
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    }
}
