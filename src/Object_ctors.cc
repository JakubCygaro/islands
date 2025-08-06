#include <Object.hpp>
#include <GL/gl.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <memory>
namespace obj{

    CelestialBody::CelestialBody() : m_sphere{nullptr} {}
    CelestialBody::CelestialBody(std::shared_ptr<UnitSphere> sphere,
        glm::vec3 pos,
        glm::vec3 speed,
        glm::vec3 acc,
        float mass):
        m_pos(pos),
        m_speed(speed),
        m_acceleration(acc),
        m_sphere(sphere),
        m_mass(mass)
    {

    }

    // copy constructor
    CelestialBody::CelestialBody(const CelestialBody &other)
        : m_pos{other.m_pos}, m_sphere{other.m_sphere},
          m_speed{other.m_speed}, m_acceleration{other.m_acceleration}, m_mass{other.m_mass}, m_radius{other.m_radius}
    {}
    // copy assign
    CelestialBody &CelestialBody::operator=(const CelestialBody &other) {
      m_pos = other.m_pos;
      m_sphere = other.m_sphere;
      m_acceleration = other.m_acceleration;
      m_speed = other.m_speed;
      m_mass = other.m_mass;
      m_radius = other.m_radius;
      return *this;
    }
    //move constructor
    CelestialBody::CelestialBody(CelestialBody &&other)
        : m_pos{other.m_pos}, m_sphere{std::move(other.m_sphere)},
          m_speed{other.m_speed}, m_acceleration{other.m_acceleration}, m_mass{other.m_mass}, m_radius{other.m_radius}
    {
      other.m_sphere = nullptr;
    }
    // move assign
    CelestialBody &CelestialBody::operator=(CelestialBody &&other) {
      m_pos = other.m_pos;
      m_sphere = std::move(other.m_sphere);
      m_acceleration = other.m_acceleration;
      m_speed = other.m_speed;
      m_mass = other.m_mass;
      m_radius = other.m_radius;
      other.m_sphere = nullptr;
      return *this;
    }
    CelestialBody::~CelestialBody() {}

    UnitSphere::UnitSphere(){
        auto sphere = make_unit_sphere();
        m_num_verticies = sphere.vertices.size();
        m_num_indices = sphere.indices.size();
        m_num_normals = sphere.normals.size();
        m_vbo = make_unit_sphere_vbo(sphere);
        m_ebo = make_unit_sphere_ebo(sphere);
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

        constexpr size_t stride = sizeof(UnitSphereCreationData::vertex_t) + sizeof(UnitSphereCreationData::normal_t);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
            (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
            (void*)(sizeof(UnitSphereCreationData::vertex_t)));
        glEnableVertexAttribArray(1);
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
}
