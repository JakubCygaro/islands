#ifndef OBJECT_HPP
#define OBJECT_HPP
#include <glad/glad.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include "shader/Shader.hpp"
#include <cstdint>
class Object {
private:
    uint32_t m_vao{};
    uint32_t m_vbo{};
    Shader m_shader;
    glm::vec3 m_pos{0};
public:
    Object();
    Object(const char* shader);
    ~Object();
    void update();
    void render(glm::mat4& view, glm::mat4& projection);
};

#endif
