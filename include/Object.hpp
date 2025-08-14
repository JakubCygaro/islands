#ifndef OBJECT_HPP
#define OBJECT_HPP
#include "shader/Shader.hpp"
#include "Util.hpp"
#include <shaders.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <GL/gl.h>
#include <cmath>
#include <cstddef>
#include <cstdint>
#ifdef DEBUG
#include <shader_files.hpp>
#endif
namespace obj {
// Wrapper around a sphere mesh stored in the GPU
class UnitSphere {
private:
    struct UnitSphereCreationData {
        using vec = std::vector<UnitSphereCreationData>;
        using vertex_t = glm::vec3;
        using normal_t = glm::vec3;
        struct VertexData{
            vertex_t vert{};
            normal_t normal{};
        };
        std::vector<VertexData> vertices;
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
protected:
    std::shared_ptr<UnitSphere> m_sphere = nullptr;
    float m_mass {};
    float m_radius {};

public:
    // one unit of mass in the simulation is equal to 1 kg
    inline static const float MASS_BOOST_FACTOR = 1e3;
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
#ifdef DEBUG
            //load directly from source tree -> works without whole project rebuild
            s_planet_shader = std::make_shared<Shader>(Shader(std::string(files::src::shaders::PLANET_VERT),
                        std::string(files::src::shaders::PLANET_FRAG)));
#else
            s_planet_shader = std::make_shared<Shader>(Shader(shaders::PLANET_VERT, shaders::PLANET_FRAG));
#endif
            return s_planet_shader;
        }
    }
    inline static std::shared_ptr<Shader> s_planet_shader = nullptr;
    inline static float calculate_radius(float mass) {
        //get radius of a sphere from density equation,
        //assuming the density of a planet to be equal to the density of the earth
        return std::pow(mass/(((4./3.) * std::numbers::pi * 5.51)), 1./3.);
    }

    std::shared_ptr<Shader> m_shader = nullptr;
public:
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
    virtual void set_mass(float) override;
};
class Star : public CelestialBody {
private:
    inline static std::shared_ptr<Shader> shader_instance() {
        if(s_star_shader){
            return s_star_shader;
        } else {
#ifdef DEBUG
            //load directly from source tree -> works without whole project rebuild
            s_star_shader = std::make_shared<Shader>(Shader(std::string(files::src::shaders::STAR_VERT),
                        std::string(files::src::shaders::STAR_FRAG)));
#else
            s_star_shader = std::make_shared<Shader>(Shader(shaders::STAR_VERT, shaders::STAR_FRAG));
#endif
            return s_star_shader;
        }
    }
    inline static std::shared_ptr<Shader> s_star_shader = nullptr;
    inline static float calculate_radius(float mass) {
        //get radius of a sphere from density equation,
        //assuming the density of a star to be equal to the density of the sun
        return std::pow(mass/(((4./3.) * std::numbers::pi * 1.622)), 1./3.);
    }

    std::shared_ptr<Shader> m_shader = nullptr;
public:
    Star(std::shared_ptr<Shader> shader = nullptr,
        glm::vec3 pos = glm::vec3(0),
        glm::vec3 speed = glm::vec3(0),
        glm::vec3 acc = glm::vec3(0),
        float mass = 1.0);
    Star(const Star&);
    Star& operator=(const Star&);
    Star(Star&&);
    Star& operator=(Star&&);
    virtual ~Star();
public:
    virtual void render() override;
    virtual void set_mass(float) override;
};
}

#endif
