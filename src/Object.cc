#include "Game.hpp"
#include "Util.hpp"
#include <GL/gl.h>
#include <Object.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <memory>

namespace obj {
    CelestialBody::CelestialBody(const char *shader): m_pos{glm::vec3(0)}, m_mass{1} {
        m_shader = std::make_shared<Shader>(Shader::from_shader_dir(shader));
        m_sphere = std::make_shared<UnitSphere>(UnitSphere());
        m_acceleration = glm::vec3(0, 0, 0);
        m_speed = glm::vec3(1, 0, 0);
    }

    CelestialBody::CelestialBody() : m_shader{nullptr}, m_sphere{nullptr} {}

    CelestialBody::CelestialBody(std::shared_ptr<Shader> shader,
                glm::vec3 pos,
                glm::vec3 speed,
                glm::vec3 acc,
                float mass)
        : m_shader{shader}, m_pos{pos}, m_speed{speed}, m_acceleration{acc}, m_mass{mass}
    {
        m_sphere = std::make_shared<UnitSphere>(UnitSphere());
    }
    CelestialBody::CelestialBody(const char *vert, const char *frag)
        : m_shader{std::make_unique<Shader>(Shader(vert, frag))} {}
    // copy constructor
    CelestialBody::CelestialBody(const CelestialBody &other)
        : m_pos{other.m_pos}, m_shader{other.m_shader}, m_sphere{other.m_sphere},
          m_speed{other.m_speed}, m_acceleration{other.m_acceleration}, m_mass{other.m_mass}
    {}
    // copy assign
    CelestialBody &CelestialBody::operator=(const CelestialBody &other) {
      m_pos = other.m_pos;
      m_shader = other.m_shader;
      m_sphere = other.m_sphere;
      m_acceleration = other.m_acceleration;
      m_speed = other.m_speed;
      m_mass = other.m_mass;
      return *this;
    }
    //move constructor
    CelestialBody::CelestialBody(CelestialBody &&other)
        : m_pos{other.m_pos}, m_shader{std::move(other.m_shader)}, m_sphere{std::move(other.m_sphere)},
          m_speed{other.m_speed}, m_acceleration{other.m_acceleration}, m_mass{other.m_mass}
    {
      other.m_shader = nullptr;
      other.m_sphere = nullptr;
    }
    // move assign
    CelestialBody &CelestialBody::operator=(CelestialBody &&other) {
      m_pos = other.m_pos;
      m_shader = std::move(other.m_shader);
      m_sphere = std::move(other.m_sphere);
      m_acceleration = other.m_acceleration;
      m_speed = other.m_speed;
      m_mass = other.m_mass;
      other.m_sphere = nullptr;
      other.m_shader = nullptr;
      return *this;
    }
    CelestialBody::~CelestialBody() {}

    void CelestialBody::update(){
        m_speed += m_acceleration;
        m_pos += m_speed;
        /*std::printf("pos: { %f, %f, %f }\n", m_pos.x, m_pos.y, m_pos.z);*/
        /*std::printf("acc: { %f, %f, %f }\n", m_acceleration.x, m_acceleration.y, m_acceleration.z);*/
    }
    void CelestialBody::render(){
        auto model = glm::mat4(1);
        model = glm::translate(model, m_pos);
        m_shader->use_shader();
        m_shader->set_mat4(name_of(model), model);
        m_sphere->draw();
    }

    UnitSphere::UnitSphere(){
        auto sphere = make_unit_sphere();
        m_num_verticies = sphere.vertices.size();
        m_num_indices = sphere.indices.size();
        m_vbo = make_unit_sphere_vbo(sphere);
        m_ebo = make_unit_sphere_ebo(sphere);
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
            (void*)0);
        glEnableVertexAttribArray(0);
    }
    //move constructor
    UnitSphere::UnitSphere(UnitSphere&& other):
        m_vao{other.m_vao}, m_vbo{other.m_vbo}, m_ebo{other.m_ebo},
        m_num_indices{other.m_num_indices}, m_num_verticies{other.m_num_verticies}
    {
        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;
        other.m_num_verticies = 0;
        other.m_num_indices = 0;
    }
    //move assign
    UnitSphere& UnitSphere::operator=(UnitSphere&& other){
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ebo = other.m_ebo;
        m_num_verticies = other.m_num_verticies;
        m_num_indices = other.m_num_indices;
        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;
        other.m_num_verticies = 0;
        other.m_num_indices = 0;
        return *this;
    }
    //destructor
    UnitSphere::~UnitSphere(){
        if(m_vao) glDeleteVertexArrays(1, &m_vao);
        if(m_vbo) glDeleteBuffers(1, &m_vbo);
        if(m_ebo) glDeleteBuffers(1, &m_ebo);
    }

    void UnitSphere::draw() const {
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    uint32_t UnitSphere::make_unit_sphere_vbo(const UnitSphere::UnitSphereCreationData& data) {
        uint32_t vbo = 0;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER,
                data.vertices.size() * sizeof(obj::UnitSphere::UnitSphereCreationData::vertex_t),
                data.vertices.data(),
                GL_STATIC_DRAW);
        return vbo;
    }
    uint32_t UnitSphere::make_unit_sphere_ebo(const UnitSphereCreationData& data){
        uint32_t ebo = 0;
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                data.indices.size() * sizeof(int32_t),
                data.indices.data(),
                GL_STATIC_DRAW);
        return ebo;
    }
    UnitSphere::UnitSphereCreationData UnitSphere::make_unit_sphere() {
        std::vector<glm::vec3> vertices{};
        std::vector<int32_t> indices{};

        float pitch = -90, yaw = 0;
        const int step = 30;
        const int pitch_step = 180 / step;
        const int yaw_step = 360 / step;
        glm::vec3 bottom_pole = glm::vec3(0, -1.0f, 0);
        vertices.push_back({bottom_pole});
        auto ring_base = glm::vec3(0);
        for(int i = 0; i < step; i++){
            pitch += pitch_step;
            ring_base.y = std::sin(glm::radians(pitch));
            ring_base.x = std::cos(glm::radians(pitch));
            for(int j = 0; j < step; j++){
                glm::vec3 current_vert = { ring_base };
                current_vert.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
                current_vert.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
                yaw += yaw_step;
                vertices.push_back({current_vert});
            }
        }
        glm::vec3 top_pole = glm::vec3(0, 1.0f, 0);
        vertices.push_back({top_pole});

        //indices for the bottom pole cap (triangles with the bottom pole)
        const auto bottom_idx = 0;
        const auto start = 1;
        for(size_t idx = 1; idx < step; idx++){
            indices.push_back(bottom_idx);
            indices.push_back(idx);
            indices.push_back(idx + 1);
            /*if(idx == step - 1){*/
            /*    indices.push_back(bottom_idx);*/
            /*    indices.push_back(step);*/
            /*    indices.push_back(start);*/
            /*}*/
        }
        const auto top_idx = vertices.size() - 1;
        //body of the sphere up to the top pole cap, i guess it also does the cap? no idea why tho, but cool that it works i guess
        for(size_t idx = 0; idx < vertices.size() - (step  + 1); idx++){
            indices.push_back(idx + step);
            indices.push_back(idx + 1);
            indices.push_back(idx);

            indices.push_back(std::clamp(idx + step - 1, idx, top_idx));
            indices.push_back(std::clamp(idx + step, idx, top_idx));
            indices.push_back(idx);
        }
        //indices for the bottom pole cap (triangles with the bottom pole), NOT NEEDED since the loop above magically does this


        /*std::printf("vertices.size(): %lld\n", vertices.size());*/
        /*std::printf("top_idx: %lld\n", top_idx);*/
        /**/
        /*start = top_idx - step - 1;*/
        /*start = vertices.size() - (step * 2 + 1);*/
        /**/
        /*std::printf("start: %d\n", start);*/

        /*for(size_t idx = start; idx < top_idx; idx++){*/
        /*    indices.push_back(idx + 1);*/
        /*    indices.push_back(idx);*/
        /*    indices.push_back(top_idx);*/
        /*    if(idx == top_idx - 1){*/
        /*        indices.push_back(top_idx);*/
        /*        indices.push_back(idx);*/
        /*        indices.push_back(start);*/
        /*    }*/
        /*}*/

        /*for (auto idx : indices | std::views::drop(indices.size() - 31)){*/
        /*    std::printf("{ %d }\n", idx);*/
        /*}*/
        return UnitSphereCreationData{
            .vertices = std::move(vertices),
            .indices = std::move(indices)
        };

    }

} // namespace obj
