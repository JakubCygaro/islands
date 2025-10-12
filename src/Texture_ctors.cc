#include "Object.hpp"
#include <glad/glad.h>
#include <stb_image/stb_image.h>

namespace obj {
    Texture::Texture(const std::string& path_to_texture){
        ::glGenTextures(1, &m_texture_id);
        ::glBindTexture(GL_TEXTURE_2D, m_texture_id);

        ::stbi_set_flip_vertically_on_load_thread(true);
        unsigned char* data{};
        int32_t w{}, h{}, chan{};

        data = ::stbi_load(path_to_texture.c_str(), &w, &h, &chan, 0);
        if(!data){
            ::stbi_image_free(data);
            std::ostringstream iss;
            iss << "Failed to load texture at path\n";
            iss << path_to_texture;
            throw std::runtime_error(iss.str());
        }
        GLenum format{};
        switch (chan){
            case 1:
                format = GL_RED;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:{
                ::stbi_image_free(data);
                std::ostringstream iss;
                iss << "Loaded texture of unknown component number (" << chan << ')' << std::endl;
                iss << path_to_texture;
                throw std::runtime_error(iss.str());
            }break;
        }
        ::glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        ::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        ::glGenerateMipmap(GL_TEXTURE_2D);
        ::stbi_image_free(data);

    }
    Texture::Texture(Texture&& other):
        m_texture_id(other.m_texture_id)
    {
        other.m_texture_id = 0;
    }
    Texture& Texture::operator=(Texture&& other){
        m_texture_id = other.m_texture_id;
        other.m_texture_id = 0;
        return *this;
    }
    Texture::~Texture(){
        if(m_texture_id){
            ::glDeleteTextures(1, &m_texture_id);
            m_texture_id = 0;
        }
    }
}
