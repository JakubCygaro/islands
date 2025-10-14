#include "Singletons.hpp"
#include <Object.hpp>
#include <algorithm>
#include <cstdio>

using namespace gm::singl;

namespace obj {
    void CelestialBody::shadow_render() {
        auto* sh = shader_instances::get_instance(shader_instances::ShaderInstance::ShadowMap);
        sh->use_shader();
        auto model = glm::mat4(1.0);
        model = glm::translate(model, m_pos);
        model = glm::scale(model, glm::vec3(m_radius));
        sh->set_mat4("model", model);
        m_sphere->draw();
    }
    void CelestialBody::forward_render(bool, bool, bool render_trails){
        if(m_selected){
            auto model = glm::mat4(1.0);
            model = glm::translate(model, m_pos);
            auto s_sh = shader_instances::get_instance(shader_instances::ShaderInstance::Selected);
            s_sh->use_shader();
            s_sh->set_mat4(name_of(model), model);
            s_sh->set_vec3("move_vector", m_speed);
            s_sh->set_float(name_of(radius), m_radius);
            MoveVectorVAO::get_instance().draw();
        }
        if(render_trails)
            m_trail.forward_render();
    }
    void CelestialBody::fixed_update(){
        m_trail.push_point(m_pos);
    }
    const font::Text3D& CelestialBody::label(){
        return m_label;
    }
    void CelestialBody::update(double& delta_t){
        m_speed += m_acceleration;
        m_acceleration = glm::vec3(0);
        auto tmp_speed = m_speed;
        tmp_speed *= delta_t;
        m_pos += tmp_speed;

        m_rotation += m_rotation_speed * delta_t;

        m_label.set_pos(glm::vec3(m_pos.x, m_pos.y + m_radius + m_label.get_text_height() * 1.2, m_pos.z));
    }
    void CelestialBody::set_pos(glm::vec3 pos){
        m_pos = pos;
        m_trail.fill(m_pos);
    }
    glm::vec3 CelestialBody::get_pos() const{
        return m_pos;
    }
    float CelestialBody::get_mass() const {
        return m_mass;
    }
    void CelestialBody::set_mass(float m){
        m_mass = m;
    }
    float CelestialBody::get_radius() const {
        return m_radius;
    }
    glm::vec3 CelestialBody::get_color() const {
        return m_color;
    }
    void CelestialBody::set_color(glm::vec3 color){
        m_color = color;
        m_trail.set_color({m_color, 0.5});
    }
    glm::vec4 CelestialBody::get_trail_color() const {
        return m_trail.get_color();
    }
    void CelestialBody::set_trail_color(glm::vec4 color) {
        m_trail.set_color(color);
    }
    void CelestialBody::set_name(std::string&& name) {
        m_name = name;
        auto n = m_name;
        m_label.set_text(std::move(n));
    }
    const std::string& CelestialBody::get_name() const {
        return m_name;
    }
    void CelestialBody::set_texture(std::shared_ptr<Texture> texture){
        m_texture = texture;
    }
    std::shared_ptr<Texture> CelestialBody::get_texture() const {
        return m_texture;
    }
    float CelestialBody::get_axial_tilt() const{
        return m_axial_tilt;
    }
    void CelestialBody::set_axial_tilt(float tilt){
        m_axial_tilt = std::clamp(tilt, 0.0f, 180.0f);
    }
    float CelestialBody::get_rotation_speed() const{
        return m_rotation_speed;
    }
    void CelestialBody::set_rotation_speed(float rot_speed){
        m_rotation_speed = rot_speed;
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
                data.vertices.size() * sizeof(UnitSphereCreationData::VertexData),
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
        std::vector<UnitSphereCreationData::VertexData> vrt{};
        std::vector<int32_t> indices{};

        constexpr const float pitch_start_value = -90;
        float pitch = pitch_start_value, yaw = 0;
        const int step = 30;
        const float pitch_step = 180. / step;
        const float yaw_step = 360. / step;
        glm::vec3 bottom_pole = glm::vec3(0, -1.0f, 0);
        for(auto i = 0; i < step; i++){
            vrt.push_back({bottom_pole, bottom_pole, { (i * yaw_step) / 360., 0.0 }});
        }

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
                vrt.push_back({current_vert, glm::normalize(current_vert), { (yaw) / (360.), (pitch - pitch_start_value) / 180.  }});
            }
            yaw = 0;
        }
        glm::vec3 top_pole = glm::vec3(0, 1.0f, 0);

        for(auto i = 0; i < step; i++){
            vrt.push_back({top_pole, glm::normalize(top_pole), { (i * yaw_step) / 360., 1.0 }});
        }

        //indices for the bottom pole cap (triangles with the bottom pole)
        auto bottom_idx = 0;
        for(size_t idx = step; idx < step - 1; idx++){
            indices.push_back(bottom_idx++);
            indices.push_back(idx);
            indices.push_back(idx + 1);
            /*if(idx == step - 1){*/
            /*    indices.push_back(bottom_idx);*/
            /*    indices.push_back(step);*/
            /*    indices.push_back(start);*/
            /*}*/
        }
        const auto first_top_vert_idx = (step + 1);
        auto top_idx = vrt.size() - first_top_vert_idx;
        //body of the sphere up to the top pole cap, i guess it also does the cap? no idea why tho, but cool that it works i guess
        for(size_t idx = 0; idx < vrt.size() - first_top_vert_idx; idx++){
            indices.push_back(idx + step);
            indices.push_back(idx + 1);
            indices.push_back(idx);

            indices.push_back(std::clamp(idx + step - 1, idx, top_idx));
            indices.push_back(std::clamp(idx + step, idx, top_idx++));
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
            // .vertices = std::move(vertices),
            // .normals = std::move(normals),
            .vertices = std::move(vrt),
            .indices = std::move(indices)
        };

    }
    // get shared_ptr to singleton instane of a UnitSphere
    std::shared_ptr<UnitSphere> UnitSphere::instance() {
        if(s_instance) [[likely]] {
            return s_instance;
        } else {
            s_instance = std::make_shared<UnitSphere>(UnitSphere());
            return s_instance;
        }
    }
} // namespace obj
