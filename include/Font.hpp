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
#include <files.hpp>
#ifdef DEBUG
#include <shader_files.hpp>
#else
#include <shaders.hpp>
#endif
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
    class TextBase {
        protected:
            std::string m_str{};
            std::shared_ptr<Shader> m_text_shader{};
            std::shared_ptr<FontBitmap> m_font_bitmap{};

            glm::vec3 m_pos{};
            glm::mat4 m_model{};
            float m_rotation{0.0f};
            float m_scale{1.0f};
            glm::vec3 m_color{1.0};
            glm::vec3 m_rotation_axis{0.0};

        public:
            virtual ~TextBase();
            TextBase();
            TextBase(std::shared_ptr<FontBitmap> font_bitmap, std::shared_ptr<Shader> text_shader, std::string text = "");
            TextBase(const TextBase& other);
            TextBase& operator=(const TextBase& other);
            TextBase(TextBase&& other);
            TextBase& operator=(TextBase&& other);

            virtual void draw() const = 0;
            virtual void debug_draw() const = 0;
            virtual void set_text(std::string&& new_text);
            virtual const std::string& get_text() const;
            virtual void set_color(glm::vec3 new_col);
            virtual const glm::vec3& get_color() const;
            virtual void set_rotation(float r);
            virtual const float& get_rotation() const;
            virtual void set_rotation_axis(glm::vec3 a);
            virtual const glm::vec3& get_rotation_axis() const;
            virtual void set_scale(float s);
            virtual const float& get_scale() const;
            virtual float get_text_height() const = 0;
            virtual float get_text_width() const = 0;
    };
    class Text2D : public TextBase {
    private:
        class DefaultShader {
            std::shared_ptr<Shader> m_shader;

            inline DefaultShader() {
#ifdef DEBUG
            m_shader = std::make_shared<Shader>(Shader(
                        std::string(files::src::shaders::TEXT_VERT),
                        std::string(files::src::shaders::TEXT_FRAG)
                        ));
#else
            m_shader = std::make_shared<Shader>(Shader(
                        shaders::TEXT_VERT,
                        shaders::TEXT_FRAG,
                        ));
#endif
            }
            DefaultShader(const DefaultShader&) = delete;
            DefaultShader operator=(const DefaultShader&) = delete;
        public:
            inline static DefaultShader& get_instance() {
                static DefaultShader instance;
                return instance;
            }
            inline std::shared_ptr<Shader> get_shader() {
                return m_shader;
            }

        };


    protected:
        uint32_t m_vao{}, m_vbo{};
        float m_height{}, m_width{};
        Text2D(std::shared_ptr<FontBitmap> font, std::shared_ptr<Shader> shader, std::string text);
    public:
        virtual ~Text2D();
        Text2D();
        Text2D(std::string text);
        Text2D(const Text2D& other);
        Text2D& operator=(const Text2D& other);
        Text2D(Text2D&& other);
        Text2D& operator=(Text2D&& other);
        virtual void draw() const override;
        virtual void debug_draw() const override;
        const glm::vec3& get_pos() const;
        void set_pos(glm::vec2&& new_pos);
        virtual void set_text(std::string&& new_text) override;
        virtual void set_rotation(float r) override;
        virtual void set_scale(float s) override;
        virtual float get_text_height() const override;
        virtual float get_text_width() const override;
    private:
        void update();
        void update_position();
    };

    class Text3D : public Text2D {
    private:
        class DefaultShader {
            std::shared_ptr<Shader> m_shader;

            inline DefaultShader() {
#ifdef DEBUG
            m_shader = std::make_shared<Shader>(Shader(
                        std::string(files::src::shaders::TEXT3D_VERT),
                        std::string(files::src::shaders::TEXT3D_FRAG)
                        ));
#else
            m_shader = std::make_shared<Shader>(Shader(
                        shaders::TEXT3D_VERT,
                        shaders::TEXT3D_FRAG,
                        ));
#endif
            }
            DefaultShader(const DefaultShader&) = delete;
            DefaultShader operator=(const DefaultShader&) = delete;
        public:
            inline static DefaultShader& get_instance() {
                static DefaultShader instance;
                return instance;
            }
            inline std::shared_ptr<Shader> get_shader() {
                return m_shader;
            }

        };
    public:
        virtual ~Text3D();
        Text3D();
        Text3D(std::string text);
        Text3D(const Text3D& other);
        Text3D& operator=(const Text3D& other);
        Text3D(Text3D&& other);
        Text3D& operator=(Text3D&& other);
        virtual void draw() const override;
        virtual void debug_draw() const override;
        void set_pos(glm::vec3&& new_pos);
    private:
        void update();
        void update_position();
    };
    class DefaultFont {
        std::shared_ptr<FontBitmap> m_font;

        inline DefaultFont(){
            m_font = std::make_shared<font::FontBitmap>(font::load_font(files::game_data::fonts::ARCADE_TTF, 48));
        }
        DefaultFont(const DefaultFont&) = delete;
        DefaultFont& operator=(const DefaultFont&) = delete;

    public:
        inline static DefaultFont& get_instance() {
            static DefaultFont instance;
            return instance;
        }
        inline std::shared_ptr<FontBitmap> get_font_bitmap() {
            return m_font;
        }
    };
}
#endif
