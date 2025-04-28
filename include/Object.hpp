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

    class CelestialBody {
    protected:
        uint32_t m_vao{};
        uint32_t m_vbo{};
        int32_t m_num_verticies{0};
        std::shared_ptr<Shader> m_shader;
        PROTECTED_PROPERTY(glm::vec3, pos)
        PROTECTED_PROPERTY(glm::vec3, speed)
        PROTECTED_PROPERTY(glm::vec3, acceleration)
    public:
        CelestialBody();
        CelestialBody(std::shared_ptr<Shader> shader);
        CelestialBody(const char* shader_name);
        CelestialBody(const char* vert, const char* frag);
        CelestialBody(const CelestialBody&);
        CelestialBody& operator=(const CelestialBody&);
        CelestialBody(CelestialBody&&);
        CelestialBody& operator=(CelestialBody&&);
        virtual ~CelestialBody();
        virtual void update();
        virtual void render(glm::mat4 view, glm::mat4 projection);
    };
    struct UnitSphereData {
        using vec = std::vector<UnitSphereData>;
        glm::vec3 vertex;
    };
    uint32_t make_unit_sphere_vbo(const UnitSphereData::vec& data);
    UnitSphereData::vec make_unit_sphere();
}

#endif
