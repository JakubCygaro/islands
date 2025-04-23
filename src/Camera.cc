#include <Camera.hpp>

void Camera::set_pos(glm::vec3&& new_pos){
    this->m_pos = new_pos;
}
glm::vec3& Camera::get_pos() {
    return m_pos;
}
