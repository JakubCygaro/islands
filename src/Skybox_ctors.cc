#include "Skybox.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

Skybox::Skybox(const std::array<std::string, 6>& path_to_faces)
{

    ::glGenTextures(1, &m_cubemap);
    ::glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);

    int w {}, h {}, chan {};
    unsigned char* data {};
    ::stbi_set_flip_vertically_on_load(true);
    for (auto i = 0; i < 6; i++) {
        data = ::stbi_load(path_to_faces[i].c_str(), &w, &h, &chan, 0);
        if (!data) {
            ::stbi_image_free(data);
            std::ostringstream iss;
            iss << "Failed to load cubemap face texture from:\n\t";
            iss << path_to_faces[i];
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
                iss << path_to_faces[i];
                throw std::runtime_error(iss.str());
            }break;
        }
        ::glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
            0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        ::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        ::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        ::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        ::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        ::glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        ::stbi_image_free(data);
    }
    ::glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    ::glGenVertexArrays(1, &m_cube_vao);
    ::glGenBuffers(1, &m_cube_vbo);
    ::glGenBuffers(1, &m_cube_ebo);

    ::glBindVertexArray(m_cube_vao);
    ::glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);

    ::glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vbo_data), cube_vbo_data, GL_STATIC_DRAW);

    ::glEnableVertexAttribArray(0);
    ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    ::glBindVertexArray(0);
}
Skybox::Skybox(Skybox&& other)
    : m_cubemap(other.m_cubemap)
    , m_cube_vao(other.m_cube_vao)
    , m_cube_vbo(other.m_cube_vbo)
{
    m_cube_vao = other.m_cube_vao;
    m_cube_vbo = other.m_cube_vbo;
    other.m_cubemap = 0;
    other.m_cube_vao = 0;
    other.m_cube_vbo = 0;
}
Skybox& Skybox::operator=(Skybox&& other)
{
    m_cubemap = other.m_cubemap;
    m_cube_vao = other.m_cube_vao;
    m_cube_vbo = other.m_cube_vbo;
    other.m_cubemap = 0;
    other.m_cube_vao = 0;
    other.m_cube_vbo = 0;
    return *this;
}
Skybox::~Skybox()
{
    if (m_cubemap) {
        ::glDeleteTextures(1, &m_cubemap);
        m_cubemap = 0;
    }
    if (m_cube_vao) {
        ::glDeleteTextures(1, &m_cube_vao);
        m_cube_vao = 0;
    }
    if (m_cube_vbo) {
        ::glDeleteTextures(1, &m_cube_vbo);
        m_cube_vbo = 0;
    }
}
