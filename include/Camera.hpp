#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>

class Camera final {
private:
    glm::vec3 m_pos{};
    glm::vec3 m_target{};
    glm::vec3 m_direction{};

public:

    void set_pos(glm::vec3&& new_pos);
    glm::vec3& get_pos();
};

#endif
