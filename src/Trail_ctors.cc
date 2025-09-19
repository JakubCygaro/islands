#include "Font.hpp"
#include "Object.hpp"

namespace obj{

    Trail::Trail(){}

    Trail::Trail(uint32_t points):
        m_size(points),
        m_data(m_size)
    {
        ::glGenVertexArrays(1, &m_vao);

        ::glGenBuffers(1, &m_vbo);
        ::glGenBuffers(1, &m_ebo);

        ::glBindVertexArray(m_vao);

        ::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        ::glBufferData(GL_ARRAY_BUFFER, m_size * sizeof(glm::vec3), NULL, GL_STATIC_DRAW);
        ::glEnableVertexAttribArray(0);
        ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

        std::vector<uint32_t> ebo_data;

        for(size_t i = 0; i < m_size - 1; i++){
            ebo_data.push_back(i);
            ebo_data.push_back(i + 1);
        }

        ::glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_data.size() * sizeof(uint32_t), ebo_data.data(), GL_STATIC_DRAW);


        ::glBindVertexArray(0);
        ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    Trail::Trail(Trail&& other):
        m_vao(other.m_vao)
        , m_vbo(other.m_vbo)
        , m_ebo(other.m_ebo)
        , m_size(other.m_size)
        , m_data(other.m_data)
    {
        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;
    }
    Trail& Trail::operator=(Trail&& other){
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ebo = other.m_ebo;
        m_size = other.m_size;
        m_data = other.m_data;

        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;

        return *this;
    }
    Trail::~Trail(){
        if(m_vao)
            ::glDeleteVertexArrays(1, &m_vao);
        if(m_vbo)
            ::glDeleteBuffers(1, &m_vbo);
        if(m_ebo)
            ::glDeleteBuffers(1, &m_ebo);
    }
}
