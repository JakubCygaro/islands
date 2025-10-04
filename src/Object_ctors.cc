#include <Object.hpp>

namespace obj {

CelestialBody::CelestialBody()
    : m_sphere { nullptr }
    , m_normals_shader(nullptr)
{
}
CelestialBody::CelestialBody(std::shared_ptr<UnitSphere> sphere,
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
        m_normals_shader = get_normals_shader_instance();
    }
    m_trail = Trail(CelestialBody::DEFAULT_TRAIL_POINT_N);
    m_trail.fill(m_pos);
    // m_label = font::Text2D(m_name);
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
    , m_mass { other.m_mass }
    , m_radius { other.m_radius }
    , m_color(other.m_color)
    , m_trail(Trail(CelestialBody::DEFAULT_TRAIL_POINT_N))
    // , m_label(other.m_label)
{
    m_trail.fill(m_pos);
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
    // m_label = other.m_label;
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
    , m_mass { other.m_mass }
    , m_radius { other.m_radius }
    , m_color(other.m_color)
    , m_trail(std::move(other.m_trail))
    // , m_label(std::move(other.m_label))
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
    // m_label = std::move(other.m_label);
    other.m_selected = false;
    other.m_sphere = nullptr;
    other.m_normals_shader = nullptr;
    other.m_trail = Trail();
    return *this;
}
CelestialBody::~CelestialBody() { }

UnitSphere::UnitSphere()
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
}
// move constructor
UnitSphere::UnitSphere(UnitSphere&& other)
    : m_vao { other.m_vao }
    , m_vbo { other.m_vbo }
    , m_ebo { other.m_ebo }
    , m_num_indices { other.m_num_indices }
    , m_num_verticies { other.m_num_verticies }
{
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_num_verticies = 0;
    other.m_num_indices = 0;
}
// move assign
UnitSphere& UnitSphere::operator=(UnitSphere&& other)
{
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
// destructor
UnitSphere::~UnitSphere()
{
    if (m_vao)
        glDeleteVertexArrays(1, &m_vao);
    if (m_vbo)
        glDeleteBuffers(1, &m_vbo);
    if (m_ebo)
        glDeleteBuffers(1, &m_ebo);
}
}
