#ifndef OBJECT_HPP
#define OBJECT_HPP
#include <glad/glad.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include "shader/Shader.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <Util.hpp>

namespace obj{
    // Wrapper around a sphere mesh stored in the GPU
    class UnitSphere {
    private:
        struct UnitSphereCreationData {
            using vec = std::vector<UnitSphereCreationData>;
            using vertex_t = glm::vec3;
            std::vector<vertex_t> vertices;
            std::vector<int32_t> indices;
        };
        uint32_t m_vao{}, m_vbo{}, m_ebo{};
        size_t m_num_indices{}, m_num_verticies{};
        uint32_t make_unit_sphere_vbo(const UnitSphereCreationData& data);
        uint32_t make_unit_sphere_ebo(const UnitSphereCreationData& data);
        UnitSphereCreationData make_unit_sphere();
    public:
        UnitSphere();
        UnitSphere(const UnitSphere&) = delete;
        UnitSphere& operator=(const UnitSphere&) = delete;
        UnitSphere(UnitSphere&& other);
        UnitSphere& operator=(UnitSphere&& other);
        ~UnitSphere();
        void draw() const;
    };

    class CelestialBody {
    protected:
        std::shared_ptr<Shader> m_shader;
        std::shared_ptr<UnitSphere> m_sphere;
        PROTECTED_PROPERTY(glm::vec3, pos)
        PROTECTED_PROPERTY(glm::vec3, speed)
        PROTECTED_PROPERTY(glm::vec3, acceleration)
        PROTECTED_PROPERTY(float, mass)
    public:
        CelestialBody();
        CelestialBody(std::shared_ptr<Shader> shader,
                glm::vec3 pos = glm::vec3(0),
                glm::vec3 speed = glm::vec3(0),
                glm::vec3 acc = glm::vec3(0),
                float mass = 1.0);
        CelestialBody(const char* shader_name);
        CelestialBody(const char* vert, const char* frag);
        CelestialBody(const CelestialBody&);
        CelestialBody& operator=(const CelestialBody&);
        CelestialBody(CelestialBody&&);
        CelestialBody& operator=(CelestialBody&&);
        virtual ~CelestialBody();
        virtual void update();
        virtual void render();
    };
}

#endif
