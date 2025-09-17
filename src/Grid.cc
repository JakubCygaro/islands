#include "Grid.hpp"
#include "shader/Shader.hpp"
#include <files.hpp>
#ifdef DEBUG
#include <shader_files.hpp>
#endif
#include <utility>
#include <vector>

namespace {
    std::vector<glm::vec2> gen_instance_offset_data(int32_t additional_segments, float offset = 1.0) {
        std::vector<glm::vec2> ret;
        for(auto c = -additional_segments; c <= additional_segments; c++){
            for (auto r = -additional_segments; r <= additional_segments; r++){
                ret.push_back(glm::vec2(
                            offset * c,
                            offset * r
                            ));
            }
        }
        return ret;
    }
    std::pair<std::vector<glm::vec3>, std::vector<uint32_t>> gen_grid_vertices_and_indicies(uint32_t side_len){
        const float spacing = 1.0f;
        const uint32_t jumps_from_center_to_side = side_len / 2;
        std::vector<glm::vec3> vert;
        std::vector<uint32_t> indc;

        glm::vec3 center = glm::vec3(0);

        glm::vec3 corner = center + glm::vec3(-spacing * jumps_from_center_to_side, 0.0, spacing * jumps_from_center_to_side);

        for(uint32_t i = 0; i < side_len; i++){
            for(uint32_t j = 0; j < side_len; j++ ){
                vert.push_back(corner + glm::vec3(j * spacing, 0, i * -spacing));

                const auto current_idx = j + (i * side_len);

                // connect to the next in the row
                if(j < side_len - 1){
                    indc.push_back(current_idx);
                    indc.push_back(current_idx + 1);
                }
                // connect to the row below
                if(i < side_len - 1){
                    indc.push_back(current_idx);
                    indc.push_back(j + ((i + 1) * side_len));
                }
            }
        }




        return { vert, indc };
    }
}

Grid::Grid(uint32_t side_len) :
    m_side_len(side_len)
{
    ::glGenVertexArrays(1, &m_vao);
    ::glBindVertexArray(m_vao);

    ::glGenBuffers(1, &m_vbo);
    ::glGenBuffers(1, &m_ebo);
    ::glGenBuffers(1, &m_offset);

    auto [vbo_data, ebo_data] = gen_grid_vertices_and_indicies(m_side_len);
    m_v_count = vbo_data.size();
    m_i_count = ebo_data.size();
    auto offset_buffer = gen_instance_offset_data(9, side_len - 1);
    m_instance_count = offset_buffer.size();

    ::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    ::glBufferData(GL_ARRAY_BUFFER, sizeof (decltype(vbo_data)::value_type) * vbo_data.size(), vbo_data.data(), GL_STATIC_DRAW);

    ::glBindBuffer(GL_ARRAY_BUFFER, m_offset);
    ::glBufferData(GL_ARRAY_BUFFER, sizeof (decltype(offset_buffer)::value_type) * offset_buffer.size(), offset_buffer.data(), GL_STATIC_DRAW);

    ::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    ::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof (decltype(ebo_data)::value_type) * ebo_data.size(), ebo_data.data(), GL_STATIC_DRAW);

    ::glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    ::glEnableVertexAttribArray(0);
    ::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    ::glBindBuffer(GL_ARRAY_BUFFER, 0);

    ::glBindBuffer(GL_ARRAY_BUFFER, m_offset);
    ::glEnableVertexAttribArray(1);
    ::glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    ::glBindBuffer(GL_ARRAY_BUFFER, 0);
    ::glVertexAttribDivisor(1, 1);

    ::glBindVertexArray(0);

#ifdef DEBUG
            //load directly from source tree -> works without whole project rebuild
            m_shader = Shader(
                        std::string(files::src::shaders::GRID_VERT),
                        std::string(files::src::shaders::GRID_FRAG)
                        );
#else
            m_shader = Shader(
                        shaders::GRID_VERT,
                        shaders::GRID_FRAG
                        );
#endif

}
Grid::Grid(Grid&& other):
    m_vao(other.m_vao)
    , m_vbo(other.m_vbo)
    , m_ebo(other.m_ebo)
    , m_offset(other.m_offset)
    , m_shader(std::move(other.m_shader))
    , m_scale(other.m_scale)
    , m_rotation(other.m_rotation)
    , m_side_len(other.m_side_len)
    , m_v_count(other.m_v_count)
    , m_i_count(other.m_i_count)
    , m_instance_count(other.m_instance_count)
{
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_offset = 0;
}
Grid& Grid::operator=(Grid&& other) {
    m_vao = other.m_vao;
    m_vbo = other.m_vbo;
    m_ebo = other.m_ebo;
    m_offset = other.m_offset;
    m_shader = std::move(other.m_shader);
    m_rotation = other.m_rotation;
    m_scale = other.m_scale;
    m_side_len = other.m_side_len;
    m_v_count = other.m_v_count;
    m_i_count = other.m_i_count;
    m_instance_count = other.m_instance_count;

    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_offset = 0;

    return *this;
}
Grid::~Grid(){
    if(m_vao){
        ::glDeleteVertexArrays(1, &m_vao);
    }
    if(m_vbo){
        ::glDeleteBuffers(1, &m_vbo);
    }
    if(m_ebo){
        ::glDeleteBuffers(1, &m_ebo);
    }
    if(m_offset){
        ::glDeleteBuffers(1, &m_offset);
    }
}
float Grid::get_scale() const{
    return m_scale;
}
void Grid::set_scale(float scale){
    m_scale = scale;
    auto offset_buffer = gen_instance_offset_data(9, (m_side_len -1 ) * m_scale);
    m_instance_count = offset_buffer.size();

    ::glBindBuffer(GL_ARRAY_BUFFER, m_offset);
    ::glBufferData(GL_ARRAY_BUFFER, sizeof (decltype(offset_buffer)::value_type) * offset_buffer.size(), offset_buffer.data(), GL_STATIC_DRAW);
    ::glBindBuffer(GL_ARRAY_BUFFER, 0);
}
glm::vec3 Grid::get_rotation() const{
    return m_rotation;
}
void Grid::set_rotation(glm::vec3 rotation){
    m_rotation = rotation;
}

void Grid::forward_render(glm::vec3 camera_pos) {
    m_shader.use_shader();
    auto model = glm::mat4(1);

    const auto half_len = (m_side_len * m_scale) / 2.0;

    auto grid_pos = glm::vec2(
                static_cast<int>(camera_pos.x / half_len) * half_len,
                static_cast<int>(camera_pos.z / half_len) * half_len
            );

    model = glm::scale(model, glm::vec3(m_scale, 1.0, m_scale));
    model = glm::translate(model, glm::vec3(grid_pos.x, 0.0, grid_pos.y));
    m_shader.set_mat4("model", model);
    ::glBindVertexArray(m_vao);
    // ::glDrawElements(GL_LINES, m_i_count, GL_UNSIGNED_INT, NULL);
    ::glDrawElementsInstanced(GL_LINES, m_i_count, GL_UNSIGNED_INT, NULL, m_instance_count);
    ::glBindVertexArray(0);
}
