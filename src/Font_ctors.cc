#include <Font.hpp>
#include <cstdio>

namespace font {
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

    TextBase::TextBase():
        m_str{},
        m_text_shader{nullptr},
        m_font_bitmap{nullptr}
    {

    }
    TextBase::TextBase(std::shared_ptr<FontBitmap> font_bitmap, std::shared_ptr<Shader> text_shader, std::string text):
        m_str{text},
        m_text_shader{text_shader},
        m_font_bitmap{font_bitmap}
    {
    }
    TextBase::TextBase(const TextBase& other):
        m_str{other.m_str},
        m_text_shader{other.m_text_shader},
        m_font_bitmap{other.m_font_bitmap},
        m_pos{other.m_pos},
        m_model{other.m_model},
        m_rotation{other.m_rotation},
        m_scale{other.m_scale},
        m_color{other.m_color}
    {

    }
    TextBase& TextBase::operator=(const TextBase& other){
        m_str = other.m_str;
        m_text_shader = other.m_text_shader;
        m_font_bitmap = other.m_font_bitmap;
        m_pos = other.m_pos;
        m_model = other.m_model;
        m_rotation = other.m_rotation;
        m_scale = other.m_scale;
        m_color = other.m_color;
        return *this;
    }
    TextBase::TextBase(TextBase&& other):
        m_str{std::move(other.m_str)},
        m_text_shader{other.m_text_shader},
        m_font_bitmap{other.m_font_bitmap},
        m_pos{other.m_pos},
        m_model{other.m_model},
        m_rotation{other.m_rotation},
        m_scale{other.m_scale},
        m_color{other.m_color}    {
        other.m_text_shader = nullptr;
        other.m_font_bitmap = nullptr;
    }
    TextBase& TextBase::operator=(TextBase&& other){
        m_str = std::move(other.m_str);
        m_text_shader = other.m_text_shader;
        m_font_bitmap = other.m_font_bitmap;
        m_pos = other.m_pos;
        m_model = other.m_model;
        m_rotation = other.m_rotation;
        m_scale = other.m_scale;
        m_color = other.m_color;

        other.m_text_shader = nullptr;
        other.m_font_bitmap = nullptr;

        return *this;
    }
    TextBase::~TextBase() {}


    Text2D::Text2D(): TextBase() {
    }
    Text2D::Text2D(std::string text) : TextBase(font::DefaultFont::get_instance().get_font_bitmap(),
            DefaultShader::get_instance().get_shader(), text)
    {
        ::glGenVertexArrays(1, &m_vao);
        ::glGenBuffers(1, &m_vbo);
        ::glBindVertexArray(m_vao);
        ::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        ::glBufferData(GL_ARRAY_BUFFER, m_str.length() * 6 * sizeof(GlyphVertex), NULL, GL_STATIC_DRAW);

        ::glEnableVertexAttribArray(0);
        ::glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        ::glEnableVertexAttribArray(1);
        ::glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        ::glBindBuffer(GL_ARRAY_BUFFER, 0);
        ::glBindVertexArray(0);

        update();
        update_position();
    }
    Text2D::Text2D(const Text2D& other) :
        TextBase(other),
        m_vao{other.m_vao},
        m_vbo{other.m_vbo},
        m_height(other.m_height),
        m_width(other.m_width)
    {

    }
    Text2D& Text2D::operator=(const Text2D& other){
        TextBase::operator=(other);
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_width = other.m_width;
        m_height = other.m_height;
        return *this;
    }
    Text2D::Text2D(Text2D&& other) :
        TextBase(other),
        m_vao{other.m_vao},
        m_vbo{other.m_vbo},
        m_height(other.m_height),
        m_width(other.m_width)

    {
        other.m_vao = 0;
        other.m_vbo = 0;
    }
    Text2D& Text2D::operator=(Text2D&& other){
        TextBase::operator=(other);
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_width = other.m_width;
        m_height = other.m_height;
        other.m_vao = 0;
        other.m_vbo = 0;
        return *this;
    }
    Text2D::~Text2D(){
        if(m_vao){
            ::glDeleteVertexArrays(1, &m_vao);
            m_vao = 0;
        }
        if(m_vbo){
            ::glDeleteBuffers(1, &m_vbo);
            m_vbo = 0;
        }
    }
}
