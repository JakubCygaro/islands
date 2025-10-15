#include "Singletons.hpp"
#include <Object.hpp>

using namespace gm::singl;

namespace obj {

CelestialBody::CelestialBody()
    : m_sphere { nullptr }
    , m_normals_shader(nullptr)
{
}
CelestialBody::CelestialBody(UnitSphereVAO* sphere,
    glm::vec3 pos,
    glm::vec3 speed,
    glm::vec3 acc,
    float mass)
    : m_pos(pos)
    , m_speed(speed)
    , m_acceleration(acc)
    , m_selected(false)
    , m_sphere(sphere)
    , m_mass(mass)
{
    if (!m_normals_shader) {
        m_normals_shader = shader_instances::get_instance(shader_instances::ShaderInstance::Normals);
    }
    m_trail = Trail(CelestialBody::DEFAULT_TRAIL_POINT_N);
    m_trail.fill(m_pos);
    m_label = font::Text3D(m_name);
    m_label.set_scale(0.01);
}

// copy constructor
CelestialBody::CelestialBody(const CelestialBody& other)
    : m_pos { other.m_pos }
    , m_speed { other.m_speed }
    , m_acceleration { other.m_acceleration }
    , m_selected(other.m_selected)
    , m_name(other.m_name)
    , m_sphere { other.m_sphere }
    , m_normals_shader(other.m_normals_shader)
    , m_texture(other.m_texture)
    , m_mass { other.m_mass }
    , m_radius { other.m_radius }
    , m_color(other.m_color)
    , m_trail(Trail(CelestialBody::DEFAULT_TRAIL_POINT_N))
    , m_label(other.get_name())
    , m_axial_tilt(other.m_axial_tilt)
    , m_rotation_speed(other.m_rotation_speed)
    , m_rotation(other.m_rotation)
{
    m_trail.fill(m_pos);
    m_label.set_scale(other.m_label.get_scale());
}

// copy assign
CelestialBody& CelestialBody::operator=(const CelestialBody& other)
{
    m_pos = other.m_pos;
    m_sphere = other.m_sphere;
    m_acceleration = other.m_acceleration;
    m_speed = other.m_speed;
    m_mass = other.m_mass;
    m_radius = other.m_radius;
    m_color = other.m_color;
    m_normals_shader = other.m_normals_shader;
    m_selected = other.m_selected;
    m_trail = Trail(CelestialBody::DEFAULT_TRAIL_POINT_N);
    m_trail.fill(m_pos);
    m_name = other.m_name;
    m_texture = other.m_texture;
    m_label = other.m_label.get_text();;
    m_label.set_scale(other.m_label.get_scale());
    m_axial_tilt = other.m_axial_tilt;
    m_rotation_speed = other.m_rotation_speed;
    m_rotation = other.m_rotation;
    return *this;
}
// move constructor
CelestialBody::CelestialBody(CelestialBody&& other)
    : m_pos { other.m_pos }
    , m_speed { other.m_speed }
    , m_acceleration { other.m_acceleration }
    , m_selected(other.m_selected)
    , m_name(std::move(other.m_name))
    , m_sphere { std::move(other.m_sphere) }
    , m_normals_shader(other.m_normals_shader)
    , m_texture(other.m_texture)
    , m_mass { other.m_mass }
    , m_radius { other.m_radius }
    , m_color(other.m_color)
    , m_trail(std::move(other.m_trail))
    , m_label(std::move(other.m_label))
    , m_axial_tilt(other.m_axial_tilt)
    , m_rotation_speed(other.m_rotation_speed)
    , m_rotation(other.m_rotation)
{
    other.m_sphere = nullptr;
    other.m_normals_shader = nullptr;
    other.m_selected = false;
    other.m_trail = Trail();
}
// move assign
CelestialBody& CelestialBody::operator=(CelestialBody&& other)
{
    m_pos = other.m_pos;
    m_sphere = std::move(other.m_sphere);
    m_acceleration = other.m_acceleration;
    m_speed = other.m_speed;
    m_mass = other.m_mass;
    m_radius = other.m_radius;
    m_color = other.m_color;
    m_normals_shader = other.m_normals_shader;
    m_selected = other.m_selected;
    m_trail = std::move(other.m_trail);
    m_name = std::move(other.m_name);
    m_label = std::move(other.m_label);
    m_texture = other.m_texture;
    m_axial_tilt = other.m_axial_tilt;
    m_rotation_speed = other.m_rotation_speed;
    m_rotation = other.m_rotation;
    other.m_selected = false;
    other.m_sphere = nullptr;
    other.m_normals_shader = nullptr;
    other.m_trail = Trail();
    return *this;
}
CelestialBody::~CelestialBody() { }

UnitSphereVAO::UnitSphereVAO()
{
    auto sphere = make_unit_sphere();
    m_num_verticies = sphere.vertices.size();
    m_num_indices = sphere.indices.size();
    m_vbo = make_unit_sphere_vbo(sphere);
    m_ebo = make_unit_sphere_ebo(sphere);
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

    constexpr size_t STRIDE = sizeof(UnitSphereCreationData::VertexData);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE,
        (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, STRIDE,
        (void*)(sizeof(UnitSphereCreationData::vertex_t)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, STRIDE,
        (void*)(sizeof(UnitSphereCreationData::vertex_t) + sizeof(UnitSphereCreationData::normal_t)));
    glEnableVertexAttribArray(2);
}
// move constructor
UnitSphereVAO::UnitSphereVAO(UnitSphereVAO&& other) : VertexArrrayObject(std::move(other))
    , m_num_indices { other.m_num_indices }
    , m_num_verticies { other.m_num_verticies }
{
    other.m_num_verticies = 0;
    other.m_num_indices = 0;
}
// move assign
UnitSphereVAO& UnitSphereVAO::operator=(UnitSphereVAO&& other)
{
    VertexArrrayObject::operator=(std::move(other));
    m_num_verticies = other.m_num_verticies;
    m_num_indices = other.m_num_indices;
    other.m_num_verticies = 0;
    other.m_num_indices = 0;
    return *this;
}
// destructor
UnitSphereVAO::~UnitSphereVAO(){}
}
