#include "Object.hpp"

namespace obj {
    Star::Star(std::shared_ptr<Shader> shader,
        glm::vec3 pos,
        glm::vec3 speed,
        glm::vec3 acc,
        float mass) : CelestialBody(UnitSphere::instance(), pos, speed, acc, mass), m_shader(shader)
    {
        if (!m_shader) {
            m_shader = Star::shader_instance();
        }
        m_radius = std::remove_reference<decltype(*this)>::type::calculate_radius(m_mass);
    }
    Star::Star(const Star& other) : CelestialBody(other), m_shader(other.m_shader){

    }
    Star& Star::operator=(const Star& other) {
        CelestialBody::operator=(other);
        m_shader = other.m_shader;
        return *this;
    }
    Star::Star(Star&& other) : CelestialBody(other), m_shader(other.m_shader){
        other.m_shader = nullptr;
    }
    Star& Star::operator=(Star&& other) {
        CelestialBody::operator=(other);
        m_shader = other.m_shader;
        other.m_shader = nullptr;
        return *this;
    }
    Star::~Star() {

    }
}
