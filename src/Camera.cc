#include <Camera.hpp>
#include <cstdio>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

Camera::Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 world_up, float yaw,
         float pitch):
    m_pos{pos},
    m_target{target},
    m_world_up{world_up},
    m_front{-glm::normalize(pos - target)},
    m_yaw{yaw},
    m_pitch{pitch}
{
    update_vectors();
}
void Camera::set_pos(glm::vec3&& new_pos){
    this->m_pos = new_pos;
}
glm::vec3& Camera::get_pos() {
    return m_pos;
}
glm::mat4 Camera::get_look_at() const {
    return glm::lookAt(m_pos, m_pos + m_front, m_up);
}
void Camera::keyboard_input(GLFWwindow* window, double delta_t) {
    const float camera_speed = 2.5f * delta_t; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_pos += camera_speed * m_front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_pos -= camera_speed * m_front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_pos -= glm::normalize(glm::cross(m_front, m_world_up)) * camera_speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_pos += glm::normalize(glm::cross(m_front, m_world_up)) * camera_speed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        m_pos += m_up * camera_speed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        m_pos -= m_up * camera_speed;
    /*std::printf("Camera::m_pos: { %f, %f, %f }\n", m_pos.x, m_pos.y, m_pos.z);*/
}
void Camera::update_vectors() {
    if(m_pitch > 89.0f)
        m_pitch = 89.0f;
    if(m_pitch < -89.0f)
        m_pitch = -89.0f;
    glm::vec3 new_front;
    new_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    new_front.y = sin(glm::radians(m_pitch));
    new_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(new_front);
    m_right = glm::normalize(glm::cross(m_front, m_world_up));

    m_up = glm::normalize(glm::cross(m_right, m_front));
}
float Camera::yaw(){
    return m_yaw;
}
float Camera::pitch(){
    return m_pitch;
}
void Camera::set_yaw(float v){
    m_yaw = v;
    update_vectors();
}
void Camera::set_pitch(float v){
    m_pitch = v;
    update_vectors();
}
