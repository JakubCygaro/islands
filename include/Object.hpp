#ifndef OBJECT_HPP
#define OBJECT_HPP
#include "shader/Shader.hpp"
#include <Util.hpp>
#include <cstdint>
#include <glad/glad.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace obj {
// Wrapper around a sphere mesh stored in the GPU
class UnitSphere {
private:
    struct UnitSphereCreationData {
        using vec = std::vector<UnitSphereCreationData>;
        using vertex_t = glm::vec3;
        std::vector<vertex_t> vertices;
        std::vector<int32_t> indices;
    };
    uint32_t m_vao {}, m_vbo {}, m_ebo {};
    size_t m_num_indices {}, m_num_verticies {};
    uint32_t make_unit_sphere_vbo(const UnitSphereCreationData& data);
    uint32_t make_unit_sphere_ebo(const UnitSphereCreationData& data);
    UnitSphereCreationData make_unit_sphere();

    inline static std::shared_ptr<UnitSphere> s_instance = nullptr;

public:
    UnitSphere();
    UnitSphere(const UnitSphere&) = delete;
    UnitSphere& operator=(const UnitSphere&) = delete;
    UnitSphere(UnitSphere&& other);
    UnitSphere& operator=(UnitSphere&& other);
    ~UnitSphere();
    void draw() const;

    static std::shared_ptr<UnitSphere> instance();
};

class CelestialBody {
protected:
    PROTECTED_PROPERTY(glm::vec3, pos)
    PROTECTED_PROPERTY(glm::vec3, speed)
    PROTECTED_PROPERTY(glm::vec3, acceleration)
    PROTECTED_PROPERTY(glm::vec3, color)
    /*PROTECTED_PROPERTY(float, mass)*/
    void update_radius();
protected:
    std::shared_ptr<UnitSphere> m_sphere = nullptr;
    float m_mass {};
    float m_radius {};

public:
    CelestialBody();
    CelestialBody(std::shared_ptr<UnitSphere> sphere = nullptr,
        glm::vec3 pos = glm::vec3(0),
        glm::vec3 speed = glm::vec3(0),
        glm::vec3 acc = glm::vec3(0),
        float mass = 1.0);
    CelestialBody(const CelestialBody&);
    CelestialBody& operator=(const CelestialBody&);
    CelestialBody(CelestialBody&&);
    CelestialBody& operator=(CelestialBody&&);
    virtual ~CelestialBody();
    virtual void update(double& delta_t);
    virtual void render() = 0;
    virtual float get_mass() const;
    virtual void set_mass(float m);
    virtual float get_radius() const;
};

class Planet : public CelestialBody {
private:
    inline static std::shared_ptr<Shader> shader_instance() {
        if(s_planet_shader){
            return s_planet_shader;
        } else {
            s_planet_shader = std::make_shared<Shader>(Shader::from_shader_dir(Planet::PLANET_SHADER_FILE));
            return s_planet_shader;
        }
    }
    inline static std::shared_ptr<Shader> s_planet_shader = nullptr;
    inline static const char* PLANET_SHADER_FILE = "planet";
    inline static const float MASS_TO_RADIUS_RATIO = 0.05f;
    // one unit of mass in the simulation is equal to 1 kg

    std::shared_ptr<Shader> m_shader = nullptr;
    void update_radius();
public:
    inline static const float MASS_BOOST_FACTOR = 1e3;
    Planet(std::shared_ptr<Shader> shader = nullptr,
        glm::vec3 pos = glm::vec3(0),
        glm::vec3 speed = glm::vec3(0),
        glm::vec3 acc = glm::vec3(0),
        float mass = 1.0);
    Planet(const Planet&);
    Planet& operator=(const Planet&);
    Planet(Planet&&);
    Planet& operator=(Planet&&);
    virtual ~Planet();
public:
    virtual void render() override;
};
}

#endif
