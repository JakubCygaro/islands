#include <Object.hpp>

namespace obj {
    void Planet::render(){
        auto model = glm::mat4(1);
        model = glm::translate(model, m_pos);
        model = glm::scale(model, glm::vec3(1) * m_radius);
        m_shader->use_shader();
        m_shader->set_mat4(name_of(model), model);
        m_shader->set_vec3(name_of(color), m_color);
        auto inverse_matrix = glm::mat3(glm::transpose(glm::inverse(model)));
        m_shader->set_mat3(name_of(inverse_matrix), inverse_matrix);
        m_sphere->draw();
    }
    void Planet::set_mass(float new_mass) {
        CelestialBody::set_mass(new_mass);
        m_radius = Planet::calculate_radius(m_mass);
    }
}
