#ifndef OBJECT_HPP
#define OBJECT_HPP
#include "Font.hpp"
#include "shader/Shader.hpp"
#include "Util.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <tuple>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <GL/gl.h>
#include <cmath>
#include <cstddef>
#include <cstdint>
#ifdef DEBUG
#include <shader_files.hpp>
#else
#include <shaders.hpp>
#endif

namespace obj {

class Text3DShader {
    std::shared_ptr<Shader> m_shader;

    inline Text3DShader(){
#ifdef DEBUG
            //load directly from source tree -> works without whole project rebuild
            auto shader = Shader(
                        std::string(files::src::shaders::TEXT3D_VERT),
                        std::string(files::src::shaders::TEXT3D_FRAG)
                        );
#else
            auto shader = Shader(
                        shaders::TEXT3D_VERT,
                        shaders::TEXT3D_FRAG
                        );
#endif
            m_shader = std::make_shared<Shader>(std::move(shader));
    };

    Text3DShader(const Text3DShader&) = delete;
    Text3DShader& operator=(const Text3DShader&) = delete;

public:
    inline std::shared_ptr<Shader> shader() {
        return m_shader;
    }
    inline static Text3DShader& get_instance() {
        static Text3DShader instance;
        return instance;
    }
};

class SelectedMarker {
    inline static std::shared_ptr<Shader> s_shader_instance = nullptr;
    inline static std::shared_ptr<Shader> shader_instance(){
        if(s_shader_instance){
            return s_shader_instance;
        } else {
#ifdef DEBUG
            //load directly from source tree -> works without whole project rebuild
            s_shader_instance = std::make_shared<Shader>(Shader(
                        std::string(files::src::shaders::MARKER_VERT),
                        std::string(files::src::shaders::MARKER_FRAG)
                        ));
#else
            s_shader_instance = std::make_shared<Shader>(Shader(
                        shaders::MARKER_VERT,
                        shaders::MARKER_FRAG
                        ));
#endif
            return s_shader_instance;
        }
    }

    struct VAO {
        uint32_t id{}, vbo{};
    private:
        VAO();
        ~VAO();
        VAO(const VAO&) = delete;
        VAO& operator=(const VAO&) = delete;

    public:
        static inline VAO& instance(){
            static VAO instance;
            return instance;
        }
    };
private:
    SelectedMarker();
    // SelectedMarker(SelectedMarker&&);
    // SelectedMarker& operator=(SelectedMarker&&);
    SelectedMarker(const SelectedMarker&) = delete;
    SelectedMarker& operator=(const SelectedMarker&) = delete;
public:
    inline static SelectedMarker& instance(){
        static SelectedMarker instance;
        return instance;
    }
    void forward_render(const glm::vec3& camera_pos, glm::vec3 pos, float radius) const;
};

class Trail {
private:
    inline static std::shared_ptr<Shader> s_shader_instance = nullptr;
    inline static std::shared_ptr<Shader> shader_instance(){
        if(s_shader_instance){
            return s_shader_instance;
        } else {
#ifdef DEBUG
            //load directly from source tree -> works without whole project rebuild
            s_shader_instance = std::make_shared<Shader>(Shader(
                        std::string(files::src::shaders::TRAIL_VERT),
                        std::string(files::src::shaders::TRAIL_FRAG)
                        ));
#else
            s_shader_instance = std::make_shared<Shader>(Shader(
                        shaders::TRAIL_VERT,
                        shaders::TRAIL_FRAG
                        ));
#endif
            return s_shader_instance;
        }
    }

    uint32_t m_vao{}, m_vbo{}, m_ebo{};
    std::size_t m_size{};
    std::vector<glm::vec3> m_data{};
    glm::vec4 m_color{1.0};

public:
    Trail();
    Trail(uint32_t points);
    Trail(const Trail&) = delete;
    Trail& operator=(const Trail&) = delete;
    Trail(Trail&&);
    Trail& operator=(Trail&&);
    ~Trail();

    void push_point(glm::vec3 point);
    void fill(glm::vec3 point);
    void forward_render();
    void copy_from_vector(const std::vector<glm::vec3>&);

    std::size_t size() const;

    glm::vec4 get_color() const;
    void set_color(glm::vec4);
};


// Wrapper around a sphere mesh stored in the GPU
class UnitSphere {
private:
    struct UnitSphereCreationData {
        using vec = std::vector<UnitSphereCreationData>;
        using vertex_t = glm::vec3;
        using normal_t = glm::vec3;
        struct VertexData{
            vertex_t vert{};
            normal_t normal{};
        };
        std::vector<VertexData> vertices;
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
protected:
    glm::vec3 m_pos{};
    PROTECTED_PROPERTY(glm::vec3, speed)
    PROTECTED_PROPERTY(glm::vec3, acceleration)
    PROTECTED_PROPERTY(bool, selected)
    std::string m_name{"Unnamed"};
protected:
    std::shared_ptr<UnitSphere> m_sphere = nullptr;
    std::shared_ptr<Shader> m_normals_shader = nullptr;
    float m_mass {};
    float m_radius {};
    glm::vec3 m_color;
    Trail m_trail{};
    // font::Text2D m_label;
    inline static constexpr uint32_t DEFAULT_TRAIL_POINT_N = 36;

private:
    inline static std::shared_ptr<Shader> s_normals_shader = nullptr;
    inline static std::shared_ptr<Shader> get_normals_shader_instance(){
        if (s_normals_shader){
            return s_normals_shader;
        } else {
#ifdef DEBUG
            auto geom = std::string(files::src::shaders::NORMALS_GEOM);
            s_normals_shader = std::make_shared<Shader>(Shader(
                        std::string(files::src::shaders::NORMALS_VERT),
                        std::string(files::src::shaders::NORMALS_FRAG),
                        &geom));
#else
            s_normals_shader = std::make_shared<Shader>(Shader(
                        shaders::NORMALS_VERT,
                        shaders::NORMALS_FRAG,
                        shaders::NORMALS_GEOM
                        ));
#endif
            return s_normals_shader;
        }
    }
protected:
    inline static std::shared_ptr<Shader> shadow_map_shader_instance() {
        if(s_shadow_map_shader){
            return s_shadow_map_shader;
        } else {
#ifdef DEBUG
            //load directly from source tree -> works without whole project rebuild
            auto geom = std::string(files::src::shaders::SHADOW_MAP_GEOM);
            s_shadow_map_shader = std::make_shared<Shader>(Shader(
                        std::string(files::src::shaders::SHADOW_MAP_VERT),
                        std::string(files::src::shaders::SHADOW_MAP_FRAG),
                        &geom
                        ));
#else
            s_shadow_map_shader = std::make_shared<Shader>(Shader(
                        shaders::SHADOW_MAP_VERT,
                        shaders::SHADOW_MAP_FRAG,
                        shaders::SHADOW_MAP_GEOM));
#endif
            return s_shadow_map_shader;
        }
    }
private:
    inline static std::shared_ptr<Shader> s_shadow_map_shader = nullptr;
protected:
    inline static std::shared_ptr<Shader> selected_shader_instance() {
        if(s_selected_shader){
            return s_selected_shader;
        } else {
#ifdef DEBUG
            //load directly from source tree -> works without whole project rebuild
            auto geom = std::string(files::src::shaders::SELECTED_GEOM);
            s_selected_shader = std::make_shared<Shader>(Shader(
                        std::string(files::src::shaders::SELECTED_VERT),
                        std::string(files::src::shaders::SELECTED_FRAG),
                        &geom
                        ));
#else
            s_selected_shader = std::make_shared<Shader>(Shader(
                        shaders::SELECTED_VERT,
                        shaders::SELECTED_FRAG,
                        shaders::SELECTED_GEOM));
#endif
            return s_selected_shader;
        }
    }
private:
    inline static std::shared_ptr<Shader> s_selected_shader = nullptr;

protected:
    struct MoveVectorVAO {
        uint32_t vao{}, vbo{};
    private:
        MoveVectorVAO(const MoveVectorVAO&) = delete;
        MoveVectorVAO& operator=(const MoveVectorVAO&) = delete;
        inline MoveVectorVAO(){
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            glm::vec3 center{};
            glBufferData(GL_ARRAY_BUFFER, sizeof(center), glm::value_ptr(center), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(center), (void*)0);
            glEnableVertexAttribArray(0);
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        };
        inline ~MoveVectorVAO(){
            if(vao)
                glDeleteVertexArrays(1, &vao);
            if(vbo)
                glDeleteBuffers(1, &vbo);
        }
    public:
        inline static MoveVectorVAO& get_instance(){
            static MoveVectorVAO instance;
            return instance;
        }
        inline void draw() const {
            glBindVertexArray(vao);
            glDrawArrays(GL_POINTS, 0, 1);
            glBindVertexArray(0);
        }
    };
public:
    // one unit of mass in the simulation is equal to 10 kg
    inline static const float MASS_BOOST_FACTOR = 1e4;
    CelestialBody();
    CelestialBody(std::shared_ptr<UnitSphere> sphere = nullptr,
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
    virtual void fixed_update();
    virtual void forward_render(bool render_normals = false, bool render_wireframe = false, bool render_trails = true);
    virtual void deferred_render() = 0;
    virtual void shadow_render();
    virtual glm::vec3 get_pos() const;
    virtual void set_pos(glm::vec3 pos);
    virtual float get_mass() const;
    virtual void set_mass(float m);
    virtual float get_radius() const;
    virtual glm::vec3 get_color() const;
    virtual void set_color(glm::vec3 color);
    virtual glm::vec4 get_trail_color() const;
    virtual void set_trail_color(glm::vec4 color);
    virtual void set_name(std::string&& name);
    virtual const std::string& get_name() const;
};

class Planet : public CelestialBody {
private:
    inline static std::shared_ptr<Shader> shader_instance() {
        if(s_planet_shader_deferred){
            return s_planet_shader_deferred;
        } else {
#ifdef DEBUG
            //load directly from source tree -> works without whole project rebuild
            s_planet_shader_deferred = std::make_shared<Shader>(Shader(std::string(files::src::shaders::PLANET_VERT),
                        std::string(files::src::shaders::PLANET_FRAG)));
#else
            s_planet_shader_deferred = std::make_shared<Shader>(Shader(shaders::PLANET_VERT, shaders::PLANET_FRAG));
#endif
            return s_planet_shader_deferred;
        }
    }
    inline static std::shared_ptr<Shader> s_planet_shader_deferred = nullptr;
    inline static float calculate_radius(float mass) {
        //get radius of a sphere from density equation,
        //assuming the density of a planet to be equal to the density of the earth
        return std::pow(mass/(((4./3.) * std::numbers::pi * 5.51)), 1./3.);
    }

    inline static std::shared_ptr<Shader> forward_shader_instance() {
        if(s_planet_shader_forward){
            return s_planet_shader_forward;
        } else {
#ifdef DEBUG
            //load directly from source tree -> works without whole project rebuild
            s_planet_shader_forward = std::make_shared<Shader>(Shader(
                        std::string(files::src::shaders::PLANET_FORWARD_VERT),
                        std::string(files::src::shaders::PLANET_FORWARD_FRAG)));
#else
            s_planet_shader_forward  = std::make_shared<Shader>(Shader(
                        shaders::PLANET_FORWARD_VERT,
                        shaders::PLANET_FORWARD_FRAG));
#endif
            return s_planet_shader_forward;
        }
    }
    inline static std::shared_ptr<Shader> s_planet_shader_forward = nullptr;
    std::shared_ptr<Shader> m_shader = nullptr;
public:
    Planet(std::shared_ptr<Shader> shader = nullptr,
        glm::vec3 pos = glm::vec3(0),
        glm::vec3 speed = glm::vec3(0),
        glm::vec3 acc = glm::vec3(0),
        float mass = 1.0);
    Planet(const Planet&);
    Planet& operator=(const Planet&);
    Planet(Planet&&);
    Planet& operator=(Planet&&);
    virtual ~Planet();
public:
    virtual void forward_render(bool render_normals = false, bool render_wireframe = false, bool render_trails = false) override;
    virtual void deferred_render() override;
    virtual void set_mass(float) override;
};
class Star : public CelestialBody {
public:
    inline static const float s_shadow_far_plane = 100.0f;
private:
private:
    inline static std::shared_ptr<Shader> shader_instance() {
        if(s_star_shader){
            return s_star_shader;
        } else {
#ifdef DEBUG
            //load directly from source tree -> works without whole project rebuild
            s_star_shader = std::make_shared<Shader>(Shader(std::string(files::src::shaders::STAR_VERT),
                        std::string(files::src::shaders::STAR_FRAG)));
#else
            s_star_shader = std::make_shared<Shader>(Shader(shaders::STAR_VERT, shaders::STAR_FRAG));
#endif
            return s_star_shader;
        }
    }
    inline static std::shared_ptr<Shader> s_star_shader = nullptr;
    inline static float calculate_radius(float mass) {
        //get radius of a sphere from density equation,
        //assuming the density of a star to be equal to the density of the sun
        return std::pow(mass/(((4./3.) * std::numbers::pi * 1.622)), 1./3.);
    }

    std::shared_ptr<Shader> m_shader = nullptr;

    float m_attenuation_linear{};
    float m_attenuation_quadratic{};
    float m_light_source_radius{};

    uint32_t m_shadow_cube_map_id{};

    inline static constexpr uint32_t SHADOW_MAP_W = 1024;
    inline static constexpr uint32_t SHADOW_MAP_H = 1024;
    inline static uint32_t s_shadow_map_width{SHADOW_MAP_W}, s_shadow_map_height{SHADOW_MAP_H};
    inline static glm::mat4 s_shadow_projection = glm::perspective(
            glm::radians(90.0f),
            (float)s_shadow_map_width/(float)s_shadow_map_height, //aspect
            1.0f, //near
            s_shadow_far_plane);//far
    glm::mat4 m_shadow_transforms[6] = {
        {1},
        {1},
        {1},
        {1},
        {1},
        {1}
    };
    uint32_t m_shadow_map_fbo{};

public:
    Star(std::shared_ptr<Shader> shader = nullptr,
        glm::vec3 pos = glm::vec3(0),
        glm::vec3 speed = glm::vec3(0),
        glm::vec3 acc = glm::vec3(0),
        float mass = 1.0);
    Star(const Star&) = delete;
    Star& operator=(const Star&) = delete;
    Star(Star&&);
    Star& operator=(Star&&);
    virtual ~Star();
public:
    virtual void forward_render(bool render_normals = false, bool render_wireframe = false, bool render_trails = false) override;
    virtual void deferred_render() override;
    virtual void update(double& delta_t) override;
    virtual void set_mass(float) override;
    float get_attenuation_linear() const;
    float get_attenuation_quadratic() const;
    float get_light_source_radius() const;
    uint32_t get_shadow_map_id() const;
    virtual void set_color(glm::vec3 color) override;
    const glm::mat4* get_shadow_transforms_ptr() const;
    void load_shadow_transforms_uniform();

    inline static void set_shadow_map_size(uint32_t width, uint32_t height){
        s_shadow_map_width = width;
        s_shadow_map_height = height;
        s_shadow_projection = glm::perspective(
            glm::radians(90.0f),
            (float)s_shadow_map_width/(float)s_shadow_map_height,
            1.0f, //near
            s_shadow_far_plane);
    }
    inline static std::tuple<uint32_t, uint32_t> get_shadow_map_size(){
        return {s_shadow_map_width, s_shadow_map_height};
    }
    uint32_t get_shadow_map_fbo() const;
private:
    static float calc_attenuation_linear(float);
    static float calc_attenuation_quadratic(float);
    static float calc_light_source_radius(float, float, const glm::vec3&);
};
}

#endif
