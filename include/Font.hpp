#ifndef FONT_HPP
#define FONT_HPP
#include <cstdint>
#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <stdexcept>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include "freetype2/freetype/freetype.h"
#include <glad/glad.h>
#include <sstream>
#include <vector>
#include "shader/Shader.hpp"
namespace font{
    struct Character {
        uint32_t texture_id{};
        glm::ivec2 size{};
        glm::ivec2 bearing{};
        uint32_t advance{};
    };
    class FontBitmap {
        uint32_t bitmap_id{};
        uint32_t glyphs_x{}, glyphs_y{};
        uint32_t glyph_count{};
        uint32_t glyph_width{}, glyph_height{};
        float total_x{}, total_y{};
        char first_char{}, last_char{};

    public:
        struct GlyphTextureCoordinates {
            glm::vec2 top_left{};
            glm::vec2 top_right{};
            glm::vec2 bottom_left{};
            glm::vec2 bottom_right{};
        };
        FontBitmap(uint32_t bitmap, uint32_t glyphs_x, uint32_t glyphs_y, uint32_t glyph_width, uint32_t glyph_height, char first, char last);
        FontBitmap(const FontBitmap& other);
        FontBitmap& operator=(const FontBitmap& other);
        FontBitmap(FontBitmap&& other);
        FontBitmap& operator=(FontBitmap&& other);

        GlyphTextureCoordinates texture_coords_for(char ch) const;
        void bind_bitmap() const;
        void unbind_bitmap() const;
        uint32_t get_glyph_width() const;
        uint32_t get_glyph_height() const;
        ~FontBitmap();
    };
    font::FontBitmap load_font(const std::string& file_path, int font_size);

    struct GlyphVertex {
        glm::vec2 pos;
        glm::vec2 tex;
    };
    class Text2D {
    private:
        std::string m_str{};
        std::shared_ptr<Shader> m_text_shader{};
        std::shared_ptr<FontBitmap> m_font_bitmap{};

        glm::vec2 m_pos{};
        glm::mat4 m_model{};
        float m_rotation{0.0f};
        float m_scale{1.0f};
        glm::vec3 m_color{1.0};

        uint32_t m_vao{}, m_vbo{};

    public:
        ~Text2D();
        Text2D();
        Text2D(std::shared_ptr<FontBitmap> font_bitmap, std::shared_ptr<Shader> text_shader, std::string text = "");
        Text2D(const Text2D& other);
        Text2D& operator=(const Text2D& other);
        Text2D(Text2D&& other);
        Text2D& operator=(Text2D&& other);
        void draw() const;
        void debug_draw() const;
        const glm::vec2& get_pos() const;
        void set_pos(glm::vec2&& new_pos);
        void set_text(std::string&& new_text);
        const std::string& get_text() const;
        void set_color(glm::vec3&& new_col);
        const glm::vec3& get_color() const;
        void set_rotation(float r);
        const float& get_rotation() const;
        void set_scale(float s);
        const float& get_scale() const;
    private:
        void update();
        void update_position();
    };
}
#endif
