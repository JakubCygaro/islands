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
        m_light_source_radius = calc_light_source_radius(m_attenuation_linear, m_attenuation_quadratic, m_color);
    }
    float Star::get_attenuation_linear() const {
        return m_attenuation_linear;
    }
    float Star::get_attenuation_quadratic() const {
        return m_attenuation_quadratic;
    }
    float Star::get_light_source_radius() const {
        return m_light_source_radius;
    }
    float Star::calc_attenuation_linear(float mass) {
        return 1.0 / std::pow(mass + 2.0, 1);
    }
    float Star::calc_attenuation_quadratic(float mass) {
        return 1.0 / std::pow(mass + 2.0, 2);
    }
    float Star::calc_light_source_radius(float lin, float quad, const glm::vec3& color) {
        float constant  = 1.0;
        float linear    = lin;
        float quadratic = quad;
        float light_max  = std::fmaxf(std::fmaxf(color.r, color.g), color.b);
        float radius    =
          (-linear +  std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * light_max)))
          / (2 * quadratic);
        return radius;
    }
}
