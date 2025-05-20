#include "Font.hpp"
#include <cmath>
#include <cstdint>

namespace {
    uint32_t gen_font_bitmap(const std::vector<font::Character>& chars, const uint32_t& width, const uint32_t& height){
        auto total_width = static_cast<uint32_t>(std::sqrt(chars.size()));
        uint32_t total_height = total_width + 1;
        uint32_t font_bitmap;
        glGenTextures(1, &font_bitmap);
        glBindTexture(GL_TEXTURE_2D, font_bitmap);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            total_width,
            total_height,
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
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, total_width, total_height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
            throw std::runtime_error("Failed to complete the framebuffer");
        }

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
        uint32_t width{}, height{};
        std::vector<Character> characters{};
        for(int ascii_code = 32; ascii_code < 128; ascii_code++){
            if(FT_Load_Char(face, (char)ascii_code, FT_LOAD_RENDER)){
                std::stringstream ss;
                ss << "Failed to load font glyph: '";
                ss << (char)ascii_code << '\'';
                ss << "for font: " << '\'' << file_path << '\'';
                throw std::runtime_error(ss.str());
            }
            if(face->glyph->bitmap.width > width) width = face->glyph->bitmap.width;
            if(face->glyph->bitmap.rows > height) height = face->glyph->bitmap.rows;
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
        gen_font_bitmap(characters, width, height);
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }
}
