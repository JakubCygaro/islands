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
    }
    Star::Star(const Star& other) : CelestialBody(other), m_shader(other.m_shader){

    }
    Star& Star::operator=(const Star& other) {
        m_shader = other.m_shader;
        return *this;
    }
    Star::Star(Star&& other) : CelestialBody(other), m_shader(other.m_shader){
        other.m_shader = nullptr;
    }
    Star& Star::operator=(Star&& other) {
        m_shader = other.m_shader;
        other.m_shader = nullptr;
        return *this;
    }
    Star::~Star() {

    }
}
