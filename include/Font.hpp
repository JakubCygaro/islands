#ifndef FONT_HPP
#define FONT_HPP
#include <cstdint>
#include <glm/ext/vector_float2.hpp>
#include <glm/glm.hpp>
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

    public:
        struct GlyphTextureCoordinates {
            glm::vec2 top_left{};
            glm::vec2 top_right{};
            glm::vec2 bottom_left{};
            glm::vec2 bottom_right{};
        };
        FontBitmap(uint32_t bitmap, uint32_t glyphs_x, uint32_t glyphs_y, uint32_t glyph_width, uint32_t glyph_height);
        FontBitmap(const FontBitmap& other);
        FontBitmap& operator=(const FontBitmap& other);
        FontBitmap(FontBitmap&& other);
        FontBitmap& operator=(FontBitmap&& other);

        GlyphTextureCoordinates texture_coords_for(char ch) const;
        void bind_bitmap() const;
        void unbind_bitmap() const;
        ~FontBitmap();
    };
    void load_font(const std::string& file_path, int font_size);
}
#endif
