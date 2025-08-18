#include "Object.hpp"

namespace obj {
    void Star::render(bool render_normals) {
        auto model = glm::mat4(1);
        model = glm::translate(model, m_pos);
        model = glm::scale(model, glm::vec3(1) * m_radius);
        m_shader->use_shader();
        m_shader->set_mat4(name_of(model), model);
        m_shader->set_vec3(name_of(color), m_color);
        m_sphere->draw();
        if(render_normals){
            m_normals_shader->use_shader();
            m_normals_shader->set_mat4(name_of(model), model);
            m_sphere->draw();
        }
    }
    void Star::set_mass(float m) {
        m_mass = m;
        m_radius = std::remove_reference<decltype(*this)>::type::calculate_radius(m_mass);

        m_attenuation_linear = calc_attenuation_linear(m_mass);
        m_attenuation_quadratic = calc_attenuation_quadratic(m_mass);
    }
    float Star::get_attenuation_linear() const {
        return m_attenuation_linear;
    }
    float Star::get_attenuation_quadratic() const {
        return m_attenuation_quadratic;
    }
    float Star::calc_attenuation_linear(float mass) {
        return 1.0 / std::pow(mass + 2.0, 4);
    }
    float Star::calc_attenuation_quadratic(float mass) {
        return 1.0 / std::pow(mass + 2.0, 6);
    }
}
