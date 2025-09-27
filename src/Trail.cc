#include "Object.hpp"
#include <algorithm>

namespace obj{
    void Trail::fill(glm::vec3 val){
        ::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        std::fill(m_data.begin(), m_data.end(), val);

        ::glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_size, m_data.data(), GL_STATIC_DRAW);
        ::glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    void Trail::forward_render() {
        auto sh = Trail::shader_instance();
        sh->use_shader();
        sh->set_vec3("color", m_color);
        ::glBindVertexArray(m_vao);
        ::glDrawElements(GL_LINES, (m_size - 1) * 2, GL_UNSIGNED_INT, NULL);
        ::glBindVertexArray(0);
    }
    void Trail::push_point(glm::vec3 point){
        std::shift_left(m_data.begin(), m_data.end(), 1);
        m_data.back() = point;

        ::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        ::glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_size, m_data.data(), GL_STATIC_DRAW);
        ::glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glm::vec3 Trail::get_color() const{
        return m_color;
    }
    void Trail::set_color(glm::vec3 color){
        m_color = color;
    }
    void Trail::copy_from_vector(const std::vector<glm::vec3>& vec){
        m_size = vec.size();
        ::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        ::glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * m_size, vec.data(), GL_STATIC_DRAW);

        ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

        std::vector<uint32_t> ebo_data;

        for(size_t i = 0; i < m_size - 1; i++){
            ebo_data.push_back(i);
            ebo_data.push_back(i + 1);
        }

        ::glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_data.size() * sizeof(uint32_t), ebo_data.data(), GL_STATIC_DRAW);
    }

    std::size_t Trail::size() const {
        return m_size;
    };
}
