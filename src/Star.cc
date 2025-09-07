#include "Object.hpp"
#include <cstddef>

namespace obj {
    void Star::forward_render(bool render_normals, bool) {
        auto model = glm::mat4(1);
        model = glm::translate(model, m_pos);
        model = glm::scale(model, glm::vec3(1) * m_radius);
        m_shader->use_shader();
        m_shader->set_mat4(name_of(model), model);
        m_shader->set_vec3(name_of(color), m_color);
        m_sphere->draw();
        if(render_normals){
            m_normals_shader->use_shader();
            m_normals_shader->set_mat4(name_of(model), model);
            m_sphere->draw();
        }
    }
    void Star::deferred_render() {

    }
    void Star::load_shadow_transforms_uniform() {
        m_shadow_transforms[0] = (s_shadow_projection *
                         glm::lookAt(m_pos, m_pos + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
        m_shadow_transforms[1] = (s_shadow_projection *
                         glm::lookAt(m_pos, m_pos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0)));
        m_shadow_transforms[2] = (s_shadow_projection *
                         glm::lookAt(m_pos, m_pos + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        m_shadow_transforms[3] = (s_shadow_projection *
                         glm::lookAt(m_pos, m_pos + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0)));
        m_shadow_transforms[4] = (s_shadow_projection *
                         glm::lookAt(m_pos, m_pos + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0)));
        m_shadow_transforms[5] = (s_shadow_projection *
                         glm::lookAt(m_pos, m_pos + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0)));
        auto sh = CelestialBody::shadow_map_shader_instance();
        sh->use_shader();
        for(size_t i = 0; i < 6; i++){
            auto name = "shadow_trans[" + std::to_string(i) + "]";
            sh->set_mat4(name.c_str(), m_shadow_transforms[i]);
        }
        sh->set_vec3("current_light_pos", m_pos);
        sh->set_float("far_plane", s_shadow_far_plane);
    }
    void Star::update(double& delta_t) {
        CelestialBody::update(delta_t);
    }
    void Star::set_mass(float m) {
        m_mass = m;
        m_radius = std::remove_reference<decltype(*this)>::type::calculate_radius(m_mass);

        m_attenuation_linear = calc_attenuation_linear(m_mass);
        m_attenuation_quadratic = calc_attenuation_quadratic(m_mass);
        m_light_source_radius = calc_light_source_radius(m_attenuation_linear, m_attenuation_quadratic, m_color);
    }
    void Star::set_color(glm::vec3 color) {
        m_color = color;
        m_light_source_radius = calc_light_source_radius(m_attenuation_linear, m_attenuation_quadratic, m_color);
    }
    float Star::get_attenuation_linear() const {
        return m_attenuation_linear;
    }
    float Star::get_attenuation_quadratic() const {
        return m_attenuation_quadratic;
    }
    uint32_t Star::get_shadow_map_id() const {
        return m_shadow_cube_map_id;
    }
    float Star::get_light_source_radius() const {
        return m_light_source_radius;
    }
    const glm::mat4* Star::get_shadow_transforms_ptr() const {
        return &m_shadow_transforms[0];
    }
    uint32_t Star::get_shadow_map_fbo() const {
        return m_shadow_map_fbo;
    }
    float Star::calc_attenuation_linear(float mass) {
        return 1.0 / std::pow(mass + 2.0, 1);
    }
    float Star::calc_attenuation_quadratic(float mass) {
        return 1.0 / std::pow(mass + 2.0, 2);
    }
    float Star::calc_light_source_radius(float lin, float quad, const glm::vec3& color) {
        float constant  = 1.0;
        float linear    = lin;
        float quadratic = quad;
        float light_max  = std::fmaxf(std::fmaxf(color.r, color.g), color.b);
        float radius    =
          (-linear +  std::sqrtf(linear * linear - 4 * quadratic * (constant - (256.0 / 5.0) * light_max)))
          / (2 * quadratic);
        return radius;
    }
}
