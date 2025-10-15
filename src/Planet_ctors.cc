#include <Object.hpp>
#include <Singletons.hpp>
using namespace gm::singl;
namespace obj {
    Planet::Planet(Shader* shader,
                glm::vec3 pos,
                glm::vec3 speed,
                glm::vec3 acc,
                float mass)
        :
            CelestialBody(buffer_instances::get_instance<UnitSphereVAO>(buffer_instances::BufferInstance::UnitSphere)
                    , pos, speed, acc, mass),
            m_shader(shader)
    {
        if (!m_shader){
            m_shader = shader_instances::get_instance(shader_instances::ShaderInstance::Planet);
        }
        m_radius = std::remove_reference<decltype(*this)>::type::calculate_radius(m_mass);
    }
    Planet::Planet(const Planet& other) : CelestialBody(other), m_shader(other.m_shader) {}
    Planet& Planet::operator=(const Planet& other) {
        CelestialBody::operator=(other);
        m_shader = other.m_shader;
        return *this;
    }
    Planet::Planet(Planet&& other) : CelestialBody(other), m_shader(other.m_shader) {
        other.m_shader = nullptr;
    }
    Planet& Planet::operator=(Planet&& other) {
        CelestialBody::operator=(other);
        m_shader = other.m_shader;
        other.m_shader = nullptr;
        return *this;
    }
    Planet::~Planet() {

    }

}
