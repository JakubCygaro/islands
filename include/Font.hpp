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
        ~FontBitmap();
    };
    font::FontBitmap load_font(const std::string& file_path, int font_size);

    class Text2D {
    private:
        // std::string m_str{};
        std::shared_ptr<Shader> m_text_shader{};
        std::shared_ptr<FontBitmap> m_font_bitmap{};
        std::vector<glm::vec4> m_letter_data{};

        glm::vec2 m_pos{};

    public:
        Text2D();
        Text2D(std::shared_ptr<FontBitmap> font_bitmap, std::shared_ptr<Shader> text_shader);
        Text2D(const Text2D& other);
        Text2D& operator=(const Text2D& other);
        Text2D(Text2D&& other);
        Text2D& operator=(Text2D&& other);
        void update();
        void draw() const;

    };
}
#endif
