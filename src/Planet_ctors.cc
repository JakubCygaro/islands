#include <Object.hpp>
namespace obj {
    Planet::Planet(std::shared_ptr<Shader> shader,
                glm::vec3 pos,
                glm::vec3 speed,
                glm::vec3 acc,
                float mass)
        :
            CelestialBody(UnitSphere::instance(), pos, speed, acc, mass),
            m_shader(shader)
    {
        if (!m_shader){
            m_shader = Planet::shader_instance();
        }
        m_radius = std::remove_reference<decltype(*this)>::type::calculate_radius(m_mass);
    }
    Planet::Planet(const Planet& other) : CelestialBody(other), m_shader(other.m_shader) {}
    Planet& Planet::operator=(const Planet& other) {
        m_shader = other.m_shader;
        return *this;
    }
    Planet::Planet(Planet&& other) : CelestialBody(other), m_shader(other.m_shader) {
        other.m_shader = nullptr;
    }
    Planet& Planet::operator=(Planet&& other) {
        m_shader = other.m_shader;
        other.m_shader = nullptr;
        return *this;
    }
    Planet::~Planet() {

    }

}
