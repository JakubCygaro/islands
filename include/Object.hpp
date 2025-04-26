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

class GameObject {
protected:
    uint32_t m_vao{};
    std::vector<uint32_t> m_vbo{};
    std::shared_ptr<Shader> m_shader;
    PROTECTED_PROPERTY(glm::vec3, pos)
public:
    GameObject();
    GameObject(std::shared_ptr<Shader> shader);
    GameObject(const char* shader_name);
    GameObject(const char* vert, const char* frag);
    GameObject(const GameObject&);
    GameObject& operator=(const GameObject&);
    GameObject(GameObject&&);
    GameObject& operator=(GameObject&&);
    ~GameObject();
    virtual void update() = 0;
    virtual void render() = 0;
};

class CelestialBody : public GameObject {
    PROPERTY(glm::vec3, speed)
    PROPERTY(glm::vec3, acceleration)
public:
    CelestialBody(std::shared_ptr<Shader> shader);

    void update() override;
    void render() override;
};

#endif
