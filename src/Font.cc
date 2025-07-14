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
        "   texture_coords = tex_coords;\n"
        "   gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);\n"
        "}"
        ;
    const char* FRAG =
        "#version 460\n"
        "out vec4 FragColor;\n"
        "in vec2 texture_coords;\n"
        "uniform sampler2D glyph_texture;\n"
        "void main(){\n"
        "   FragColor = texture(glyph_texture, texture_coords);\n"
        "}"
        ;
    font::FontBitmap gen_font_bitmap(const std::vector<font::Character>& chars, const uint32_t& glyph_width, const uint32_t& glyph_height){
        auto glyphs_x = static_cast<uint32_t>(std::sqrt(chars.size()));
        uint32_t glyphs_y = glyphs_x + 1;
        uint32_t font_bitmap;
        uint32_t bitmap_width{}, bitmap_height{};

        bitmap_width = glyphs_x * glyph_width;
        bitmap_height = glyphs_y * glyph_height;

        glGenTextures(1, &font_bitmap);
        glBindTexture(GL_TEXTURE_2D, font_bitmap);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            bitmap_width,
            bitmap_height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            NULL
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        uint32_t fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, font_bitmap, 0);

        // unsigned int rbo;
        // glGenRenderbuffers(1, &rbo);
        // glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, bitmap_width, bitmap_height);
        // glBindRenderbuffer(GL_RENDERBUFFER, 0);
        //
        // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        // glBindRenderbuffer(GL_FRAMEBUFFER, 0);

        if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
            throw std::runtime_error("Failed to complete the framebuffer");
        }
        auto glyph_shader = Shader(VERT, FRAG);


        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        uint32_t glyph_vbo = 0;
        uint32_t glyph_vao = 0;
        uint32_t glyph_ebo = 0;

        glGenBuffers(1, &glyph_vbo);
        glGenVertexArrays(1, &glyph_vao);

        glBindVertexArray(glyph_vao);
        glBindBuffer(GL_ARRAY_BUFFER, glyph_vbo);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(glm::vec2), NULL, GL_DYNAMIC_DRAW);

        //position
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
            (void*)0);
        glEnableVertexAttribArray(0);
        //texture coords
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

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
        auto projection = glm::ortho(0.0f, static_cast<float>(bitmap_width),
                0.0f, static_cast<float>(bitmap_height), 0.0f, 1.0f);

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

            glBindVertexArray(glyph_vao);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, glyph.texture_id);
            // segfaults on this call
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glDeleteTextures(1, &glyph.texture_id);
        }
        glEnable(GL_DEPTH_TEST);
        // glDeleteRenderbuffers(1, &rbo);
        glDeleteFramebuffers(1, &fbo);
        glDeleteVertexArrays(1, &glyph_vao);
        glDeleteBuffers(1, &glyph_vbo);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return font::FontBitmap(font_bitmap, glyphs_x, glyphs_y, glyph_width, glyph_height);
    }
}
namespace font{
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
        auto bitmap = gen_font_bitmap(characters, max_glyph_width, max_glyph_height);
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        return bitmap;
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

    }
    Text2D::Text2D(const Text2D& other):
        m_str{other.m_str},
        m_text_shader{other.m_text_shader},
        m_font_bitmap{other.m_font_bitmap},
        m_letter_data{other.m_letter_data},
        m_pos{other.m_pos}
    {

    }
    Text2D& Text2D::operator=(const Text2D& other){
        m_str = other.m_str;
        m_text_shader = other.m_text_shader;
        m_font_bitmap = other.m_font_bitmap;
        m_letter_data = other.m_letter_data;
        m_pos = other.m_pos;
        return *this;
    }
    Text2D::Text2D(Text2D&& other):
        m_str{other.m_str},
        m_text_shader{other.m_text_shader},
        m_font_bitmap{other.m_font_bitmap},
        m_letter_data{other.m_letter_data},
        m_pos{other.m_pos}
    {
        other.m_str = nullptr;
        other.m_text_shader = nullptr;
        other.m_font_bitmap = nullptr;
        m_letter_data.clear();
    }
    Text2D& Text2D::operator=(Text2D&& other){
        m_str = other.m_str;
        m_text_shader = other.m_text_shader;
        m_font_bitmap = other.m_font_bitmap;
        m_letter_data = other.m_letter_data;
        m_pos = other.m_pos;

        other.m_str = nullptr;
        other.m_text_shader = nullptr;
        other.m_font_bitmap = nullptr;
        other.m_letter_data.clear();

        return *this;
    }
    void Text2D::update() {
        std::string::const_iterator str_iter = m_str.begin();
        auto pos = m_pos;

        for(char c = *str_iter; str_iter != m_str.end(); str_iter++){
            auto glypth_coord = m_font_bitmap->texture_coords_for(c);
        }
    }
    void Text2D::draw() const {

        float data[] = {
            0.0, 0.0, 0.0, 1.0,
            0.0, 300.0, 0.0, 0.0,
            300.0, 300.0, 1.0, 0.0,
            300.0, 300.0, 1.0, 0.0,
            300.0, 0.0, 1.0, 1.0,
            0.0, 0.0, 0.0, 1.0,
        };

        uint32_t vbo{}, vao{};

        glGenBuffers(1, &vbo);
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
            (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_font_bitmap->bind_bitmap();
        m_text_shader->use_shader();

        glBindVertexArray(vao);

        glDrawArrays(GL_TRIANGLES, 0, 1);
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);

    }
}
