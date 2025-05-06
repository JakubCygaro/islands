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
private:
    inline static const char* C_BODY_SHADER_FILE = "c_body";
    inline static std::shared_ptr<Shader> s_c_body_shader = nullptr;

protected:
    std::shared_ptr<Shader> m_shader;
    std::shared_ptr<UnitSphere> m_sphere;
    PROTECTED_PROPERTY(glm::vec3, pos)
    PROTECTED_PROPERTY(glm::vec3, speed)
    PROTECTED_PROPERTY(glm::vec3, acceleration)
    PROTECTED_PROPERTY(glm::vec3, color)
    /*PROTECTED_PROPERTY(float, mass)*/
    float m_mass {};
    float m_radius {};

public:
    inline static const float MASS_TO_RADIUS_RATIO = 0.05f;
    // one unit of mass in the simulation is equal to 1 kg
    inline static const float MASS_BOOST_FACTOR = 1e3;

public:
    CelestialBody();
    CelestialBody(std::shared_ptr<Shader> shader = nullptr,
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
    virtual void render();
    float get_mass() const;
    void set_mass(float m);
    float get_radius() const;

    static std::shared_ptr<Shader> shader_instance();

private:
    void update_radius();
};
}

#endif
