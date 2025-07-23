#include <Object.hpp>
#include "Util.hpp"
#include <GL/gl.h>
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

    void CelestialBody::update(double& delta_t){
        m_speed += m_acceleration;
        auto tmp_speed = m_speed;
        tmp_speed *= delta_t;
        m_pos += tmp_speed;
    }
    float CelestialBody::get_mass() const {
        return m_mass;
    }
    void CelestialBody::set_mass(float m){
        m_mass = m;
        update_radius();
    }
    float CelestialBody::get_radius() const {
        return m_radius;
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
    // get shared_ptr to singleton instane of a UnitSphere
    std::shared_ptr<UnitSphere> UnitSphere::instance() {
        if(s_instance) {
            return s_instance;
        } else [[likely]] {
            s_instance = std::make_shared<UnitSphere>(UnitSphere());
            return s_instance;
        }
    }
    void CelestialBody::update_radius(){

    }

} // namespace obj
