#ifndef GRID_HPP
#define GRID_HPP
#include "shader/Shader.hpp"
#include <cstdint>
#include <glm/fwd.hpp>

class Grid final {
    uint32_t m_vao{}, m_vbo{}, m_ebo{}, m_offset{};
    Shader m_shader;

    float m_scale{1.0};
    glm::vec3 m_rotation{};

    uint32_t m_side_len{};
    uint32_t m_v_count{};
    uint32_t m_i_count{};
    uint32_t m_instance_count{};

public:
    Grid(uint32_t side_len);
    ~Grid();
    Grid(const Grid&) = delete;
    Grid& operator=(const Grid&) = delete;
    Grid(Grid&&);
    Grid& operator=(Grid&&);

    void forward_render(glm::vec3 camera_pos) ;

    float get_scale() const;
    void set_scale(float);
    glm::vec3 get_rotation() const;
    void set_rotation(glm::vec3);
};

#endif
