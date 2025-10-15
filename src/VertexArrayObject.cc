#include <VertexArrayObject.hpp>
#include <glad/glad.h>
VertexArrrayObject::VertexArrrayObject(uint32_t vao, uint32_t vbo, uint32_t ebo):
    m_vao(vao)
    , m_vbo(vbo)
    , m_ebo(ebo)
{

}
VertexArrrayObject::VertexArrrayObject(){}
VertexArrrayObject::VertexArrrayObject(VertexArrrayObject&& other):
    m_vao(other.m_vao)
    , m_vbo(other.m_vbo)
    , m_ebo(other.m_ebo)
{
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
}
VertexArrrayObject& VertexArrrayObject::operator=(VertexArrrayObject&& other){
    m_vao = other.m_vao;
    m_vbo = other.m_vbo;
    m_ebo = other.m_ebo;
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    return *this;
}
VertexArrrayObject::~VertexArrrayObject(){
    if(m_vao){
        ::glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if(m_vbo){
        ::glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if(m_ebo){
        ::glDeleteBuffers(1, &m_ebo);
        m_ebo = 0;
    }
}
void VertexArrrayObject::bind() const{
    ::glBindVertexArray(m_vao);
}
void VertexArrrayObject::unbind() const{
    ::glBindVertexArray(0);
}
