#include "Object.hpp"
namespace obj {
    void Texture::bind() const{
        ::glBindTexture(GL_TEXTURE_2D, m_texture_id);
    }
}
