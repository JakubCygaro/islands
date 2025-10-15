#include "Font.hpp"
#include "Object.hpp"
#include "Singletons.hpp"
#include <algorithm>
#include <cstdio>

using namespace gm::singl;
namespace obj {
    namespace {
        uint32_t gen_shadow_cube_map() {
            auto [width, height] = Star::get_shadow_map_size();
            // auto fbo = Star::get_shadow_map_fbo();
            uint32_t cubemap{};
            glGenTextures(1, &cubemap);

            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
            for(auto i = 0; i < 6; i++){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                        0,
                        GL_DEPTH_COMPONENT,
                        width,
                        height,
                        0,
                        GL_DEPTH_COMPONENT,
                        GL_FLOAT,
                        NULL);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            return cubemap;
        }
        uint32_t initialize_shadow_map_fbo(uint32_t shadow_map){
            uint32_t shadow_map_fbo{};
            glGenFramebuffers(1, &shadow_map_fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
                throw std::runtime_error("Failed to complete the shadow framebuffer");
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return shadow_map_fbo;
        }
    }
    Star::Star(Shader* shader,
        glm::vec3 pos,
        glm::vec3 speed,
        glm::vec3 acc,
        float mass) : CelestialBody(buffer_instances::get_instance<UnitSphereVAO>(buffer_instances::BufferInstance::UnitSphere)
            , pos, speed, acc, mass), shader(shader)
    {
        if (!shader) {
            shader = shader_instances::get_instance(shader_instances::ShaderInstance::Star);
        }
        m_radius = std::remove_reference<decltype(*this)>::type::calculate_radius(m_mass);
        m_attenuation_linear = calc_attenuation_linear(m_mass);
        m_attenuation_quadratic = calc_attenuation_quadratic(m_mass);
        m_light_source_radius = calc_light_source_radius(m_attenuation_linear, m_attenuation_quadratic, m_color);
        m_shadow_cube_map_id = gen_shadow_cube_map();
        m_shadow_map_fbo = initialize_shadow_map_fbo(m_shadow_cube_map_id);
    }
    // Star::Star(const Star& other) : CelestialBody(other),
    //     m_shader(other.m_shader),
    //     m_attenuation_linear(other.m_attenuation_linear),
    //     m_attenuation_quadratic(other.m_attenuation_quadratic),
    //     m_light_source_radius(other.m_light_source_radius),
    //     m_shadow_cube_map_id(other.m_shadow_cube_map_id),
    //     m_shadow_map_fbo(other.m_shadow_map_fbo)
    // {
    //     std::copy(std::begin(other.m_shadow_transforms), std::end(other.m_shadow_transforms), std::begin(m_shadow_transforms));
    // }
    // Star& Star::operator=(const Star& other) {
    //     CelestialBody::operator=(other);
    //     m_shader = other.m_shader;
    //     m_attenuation_linear = other.m_attenuation_linear;
    //     m_attenuation_quadratic = other.m_attenuation_quadratic;
    //     m_light_source_radius = other.m_light_source_radius;
    //     m_shadow_cube_map_id = other.m_shadow_cube_map_id;
    //     m_shadow_map_fbo = other.m_shadow_map_fbo;
    //     std::copy(std::begin(other.m_shadow_transforms), std::end(other.m_shadow_transforms), std::begin(m_shadow_transforms));
    //     return *this;
    // }
    Star::Star(Star&& other) : CelestialBody(other),
        shader(other.shader),
        m_attenuation_linear(other.m_attenuation_linear),
        m_attenuation_quadratic(other.m_attenuation_quadratic),
        m_light_source_radius(other.m_light_source_radius),
        m_shadow_cube_map_id(other.m_shadow_cube_map_id),
        m_shadow_map_fbo(other.m_shadow_map_fbo)
    {
        std::copy(std::begin(other.m_shadow_transforms), std::end(other.m_shadow_transforms), std::begin(m_shadow_transforms));
        other.shader = nullptr;
        other.m_shadow_cube_map_id = 0;
        other.m_shadow_map_fbo = 0;
    }
    Star& Star::operator=(Star&& other) {
        CelestialBody::operator=(other);
        shader = other.shader;
        m_attenuation_linear = other.m_attenuation_linear;
        m_attenuation_quadratic = other.m_attenuation_quadratic;
        m_light_source_radius = other.m_light_source_radius;
        m_shadow_cube_map_id = other.m_shadow_cube_map_id;
        m_shadow_map_fbo = other.m_shadow_map_fbo;
        std::copy(std::begin(other.m_shadow_transforms), std::end(other.m_shadow_transforms), std::begin(m_shadow_transforms));
        other.shader = nullptr;
        other.m_shadow_cube_map_id = 0;
        other.m_shadow_map_fbo = 0;
        return *this;
    }
    Star::~Star() {
        if(m_shadow_map_fbo){
            glDeleteFramebuffers(1, &m_shadow_map_fbo);
        }
        if(m_shadow_cube_map_id){
            glDeleteTextures(1, &m_shadow_cube_map_id);
        }
    }
}
