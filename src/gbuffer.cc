#include "Font.hpp"
#include "Game.hpp"
#include <glm/ext/vector_float2.hpp>
#include <sstream>
#include <stdexcept>
Gbuffer::Gbuffer(){}
Gbuffer::Gbuffer(int32_t width, int32_t height):
    width(width),
    height(height)
{
    glGenFramebuffers(1, &this->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);

    glGenTextures(1, &g_position);
    glBindTexture(GL_TEXTURE_2D, g_position);
    glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGBA16F,
            width,
            height,
            0,
            GL_RGBA,
            GL_FLOAT,
            NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_position, 0);

    glGenTextures(1, &g_normal);
    glBindTexture(GL_TEXTURE_2D, g_normal);
    glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGBA16F,
            width,
            height,
            0,
            GL_RGBA,
            GL_FLOAT,
            NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_normal, 0);

    glGenTextures(1, &g_color_spec);
    glBindTexture(GL_TEXTURE_2D, g_color_spec);
    glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_color_spec, 0);

    uint32_t attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

    glDrawBuffers(3, attachments);

    glGenRenderbuffers(1, &this->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        std::ostringstream oss;
        oss << "Gbuffer creation error: " << std::endl;
        throw std::runtime_error(oss.str());
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenBuffers(1, &quad_vbo);
    glGenBuffers(1, &quad_ebo);
    glGenVertexArrays(1, &quad_vao);

    glBindVertexArray(quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glm::vec2 quad_data[8] = {
        {-1.0, 1.0}, {0.0, 1.0},
        {-1.0, -1.0},{0.0, 0.0},
        {1.0, -1.0}, {1.0, 0.0},
        {1.0, 1.0},  {1.0, 1.0}
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof quad_data, quad_data, GL_STATIC_DRAW);

    uint32_t ebo_data[6] = {
        0, 1, 2,
        2, 3, 0
    };

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof ebo_data, ebo_data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec2),
        (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec2),
        (void*)sizeof(glm::vec2));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

}
Gbuffer::Gbuffer(const Gbuffer& other):
    width(other.width),
    height(other.height),
    quad_vbo(other.quad_vbo),
    quad_ebo(other.quad_ebo),
    g_position(other.g_position),
    g_normal(other.g_normal),
    g_color_spec(other.g_color_spec),
    fbo(other.fbo),
    rbo(other.rbo),
    quad_vao(other.quad_vao)

{

}
Gbuffer& Gbuffer::operator=(const Gbuffer& other){
    width = other.width;
    height = other.height;
    g_position = other.g_position;
    g_normal = other.g_normal;
    g_color_spec = other.g_color_spec;
    fbo = other.fbo;
    rbo = other.rbo;
    quad_vao = other.quad_vao;
    quad_vbo = other.quad_vbo;
    quad_ebo = other.quad_ebo;

    return *this;
}
Gbuffer::Gbuffer(Gbuffer&& other):
    width(other.width),
    height(other.height),
    quad_vbo(other.quad_vbo),
    quad_ebo(other.quad_ebo),
    g_position(other.g_position),
    g_normal(other.g_normal),
    g_color_spec(other.g_color_spec),
    fbo(other.fbo),
    rbo(other.rbo),
    quad_vao(other.quad_vao)
{
    other.width = 0;
    other.height = 0;
    other.g_position = 0;
    other.g_normal = 0;
    other.g_color_spec = 0;
    other.fbo = 0;
    other.rbo = 0;
    other.quad_vao = 0;
    other.quad_vbo = 0;
    other.quad_ebo = 0;
}
Gbuffer& Gbuffer::operator=(Gbuffer&& other){
    width = other.width;
    height = other.height;
    g_position = other.g_position;
    g_normal = other.g_normal;
    g_color_spec = other.g_color_spec;
    fbo = other.fbo;
    rbo = other.rbo;
    quad_vao = other.quad_vao;
    quad_vbo = other.quad_vbo;
    quad_ebo = other.quad_ebo;
    other.width = 0;
    other.height = 0;
    other.g_position = 0;
    other.g_normal = 0;
    other.g_color_spec = 0;
    other.fbo = 0;
    other.rbo = 0;
    other.quad_vao = 0;
    other.quad_vbo = 0;
    other.quad_ebo = 0;
    return *this;
}
Gbuffer::~Gbuffer(){
    if(g_position)
        glDeleteBuffers(1, &g_position);
    if(g_normal)
        glDeleteBuffers(1, &g_normal);
    if(g_color_spec)
        glDeleteBuffers(1, &g_color_spec);
    if(fbo)
        glDeleteFramebuffers(1, &fbo);
    if(rbo)
        glDeleteRenderbuffers(1, &rbo);
    if(quad_vao)
        glDeleteBuffers(1, &quad_vao);
    if(quad_vbo)
        glDeleteBuffers(1, &quad_vbo);
    if(quad_ebo)
        glDeleteBuffers(1, &quad_ebo);
}
void Gbuffer::bind() const{
    assert(fbo != 0);
    assert(quad_vao != 0);
    assert(quad_vbo != 0);
    assert(quad_ebo != 0);
    assert(g_position != 0);
    assert(g_normal != 0);
    assert(rbo != 0);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
}
void Gbuffer::unbind() const{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
