#include "Game.hpp"
#include "Util.hpp"
#include <GL/gl.h>
#include <Object.hpp>
#include <cmath>
#include <cstdio>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <memory>
namespace obj {
    CelestialBody::CelestialBody(const char *shader) {
        m_shader = std::make_shared<Shader>(Shader::from_shader_dir(shader));
        m_vao = 0;
        m_vbo = 0;
        auto sphere = make_unit_sphere();
        m_vertices = sphere.size();
        m_vbo = make_unit_sphere_vbo(sphere);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
         (void*)0);
        glEnableVertexAttribArray(0);
      /*std::printf("Object constructor called\n");*/
      /*std::uint32_t vbo, vao;*/
      /*glGenBuffers(1, &vbo);*/
      /*glGenVertexArrays(1, &vao);*/
      /*glBindVertexArray(vao);*/
      /*glBindBuffer(GL_ARRAY_BUFFER, vbo);*/
      /*float verts[] = {*/
      /*    -0.5f, -0.5f, -0.5f,*/
      /*     0.5f, -0.5f, -0.5f,*/
      /*     0.5f,  0.5f, -0.5f,*/
      /*     0.5f,  0.5f, -0.5f,*/
      /*    -0.5f,  0.5f, -0.5f,*/
      /*    -0.5f, -0.5f, -0.5f,*/
      /**/
      /*    -0.5f, -0.5f,  0.5f,*/
      /*     0.5f, -0.5f,  0.5f,*/
      /*     0.5f,  0.5f,  0.5f,*/
      /*     0.5f,  0.5f,  0.5f,*/
      /*    -0.5f,  0.5f,  0.5f,*/
      /*    -0.5f, -0.5f,  0.5f,*/
      /**/
      /*    -0.5f,  0.5f,  0.5f,*/
      /*    -0.5f,  0.5f, -0.5f,*/
      /*    -0.5f, -0.5f, -0.5f,*/
      /*    -0.5f, -0.5f, -0.5f,*/
      /*    -0.5f, -0.5f,  0.5f,*/
      /*    -0.5f,  0.5f,  0.5f,*/
      /**/
      /*     0.5f,  0.5f,  0.5f,*/
      /*     0.5f,  0.5f, -0.5f,*/
      /*     0.5f, -0.5f, -0.5f,*/
      /*     0.5f, -0.5f, -0.5f,*/
      /*     0.5f, -0.5f,  0.5f,*/
      /*     0.5f,  0.5f,  0.5f,*/
      /**/
      /*    -0.5f, -0.5f, -0.5f,*/
      /*     0.5f, -0.5f, -0.5f,*/
      /*     0.5f, -0.5f,  0.5f,*/
      /*     0.5f, -0.5f,  0.5f,*/
      /*    -0.5f, -0.5f,  0.5f,*/
      /*    -0.5f, -0.5f, -0.5f,*/
      /**/
      /*    -0.5f,  0.5f, -0.5f,*/
      /*     0.5f,  0.5f, -0.5f,*/
      /*     0.5f,  0.5f,  0.5f,*/
      /*     0.5f,  0.5f,  0.5f,*/
      /*    -0.5f,  0.5f,  0.5f,*/
      /*    -0.5f,  0.5f, -0.5f,*/
      /*};*/
      /*glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);*/
      /**/
      /*std::printf("VAO: %d VBO: %d\n", vao, vbo);*/
      /**/
      /*Shader test_shader;*/
      /*test_shader = Shader::from_shader_dir(shader);*/
      /*std::cout << "shader loaded successfully\n";*/
      /*m_shader = test_shader;*/
      /*m_vao = vao;*/
      /*m_vbo = vbo;*/
    }

    CelestialBody::CelestialBody() : m_vao{0}, m_vbo{0}, m_shader{nullptr} {}

    CelestialBody::CelestialBody(std::shared_ptr<Shader> shader)
        : m_vao{0}, m_vbo{0}, m_shader{shader} {
            auto sphere = make_unit_sphere();
            m_vbo = make_unit_sphere_vbo(sphere);
        }
    CelestialBody::CelestialBody(const char *vert, const char *frag)
        : m_vao{0}, m_vbo{0},
          m_shader{std::make_unique<Shader>(Shader(vert, frag))} {}
    CelestialBody::CelestialBody(const CelestialBody &other)
        : m_vbo{other.m_vbo}, m_vao{other.m_vao}, m_pos{other.m_pos},
          m_shader{other.m_shader} {}
    CelestialBody &CelestialBody::operator=(const CelestialBody &other) {
      m_pos = other.m_pos;
      m_vao = other.m_vao;
      m_pos = other.m_pos;
      m_shader = other.m_shader;
      return *this;
    }
    CelestialBody::CelestialBody(CelestialBody &&other)
        : m_vbo{std::move(other.m_vbo)}, m_vao{other.m_vao}, m_pos{other.m_pos},
          m_shader{std::move(other.m_shader)} {
      other.m_vao = 0;
      other.m_shader = nullptr;
    }
    CelestialBody &CelestialBody::operator=(CelestialBody &&other) {
      m_vbo = std::move(other.m_vbo);
      m_vao = other.m_vao;
      other.m_vao = 0;
      m_pos = other.m_pos;
      m_shader = std::move(other.m_shader);
      return *this;
    }
    CelestialBody::~CelestialBody() {
      glDeleteVertexArrays(1, &m_vao);
      glDeleteBuffers(1, &m_vbo);
    }

    uint32_t make_unit_sphere_vbo(const UnitSphereData::vec& data) {
        uint32_t vbo = 0;
        glGenBuffers(1, &vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(obj::UnitSphereData), data.data(), GL_STATIC_DRAW);
        return vbo;
    }
    void CelestialBody::update(){

    }
    void CelestialBody::render(){
        auto model = glm::mat4(1);
        model = glm::translate(model, m_pos);
        m_shader->use_shader();
        m_shader->set_mat4(name_of(model), model);
        glBindVertexArray(m_vao);
        glDrawArrays(GL_TRIANGLES, 0, m_vertices);
    }
    UnitSphereData::vec make_unit_sphere() {
        UnitSphereData::vec ret{};
        float pitch = 0, yaw = 0;
        const int step = 30;
        const int pitch_step = 180 / step;
        const int yaw_step = 360 / (step * 2);
        glm::vec3 bottom_pole = glm::vec3(0, -1.0f, 0);
        ret.push_back({bottom_pole});

        for(int i = 0; i < step; i++){
            pitch += pitch_step;
            /*auto identity = glm::mat4(1);*/
            /*auto rot = glm::rotate(identity, glm::radians(pitch), glm::vec3(1.0f, 0, 0));*/
            /*bottom_pole *= rot;*/
            auto current_vert = bottom_pole;
            current_vert.y += std::sin(glm::radians(pitch));
            /*current_vert.x += std::cos(glm::radians(pitch));*/
            ret.push_back({current_vert});
            /*for(int j = 0; j < step; j++){*/
            /*    current_vert.x = std::cos(glm::radians(yaw));*/
            /*    current_vert.z = std::sin(glm::radians(yaw));*/
            /*    current_vert = glm::normalize(current_vert);*/
            /*    yaw += j;*/
            /*    ret.push_back({current_vert});*/
            /*}*/
        }
        glm::vec3 top_pole = glm::vec3(0, 1.0f, 0);
        ret.push_back({top_pole});
        for (auto& d : ret){
            auto& vert = d.vertex;
            std::printf("{ %f, %f, %f }\n", vert.x, vert.y, vert.z);
        }
        return ret;

    }

} // namespace obj
