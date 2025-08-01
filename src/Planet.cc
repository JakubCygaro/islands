#include <Object.hpp>

namespace obj {
    void Planet::render(){
        auto model = glm::mat4(1);
        model = glm::translate(model, m_pos);
        model = glm::scale(model, glm::vec3(1) * m_radius);
        m_shader->use_shader();
        m_shader->set_mat4(name_of(model), model);
        m_shader->set_vec3(name_of(color), m_color);
        m_sphere->draw();
    }
    void Planet::set_mass(float new_mass) {
        CelestialBody::set_mass(new_mass);
        m_radius = Planet::calculate_radius(m_mass);
    }
    // void Planet::update_radius() {
    //     //get radius of a sphere form density equation,
    //     //assuming the density of a planet to be equal to the density of earth
    //     m_radius = std::pow(m_mass/(((4./3.) * std::numbers::pi * 5.51)), 1./3.);
    // }
}
