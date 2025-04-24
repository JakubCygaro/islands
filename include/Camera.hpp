#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <iostream>

class Camera final {
private:
  glm::vec3 m_pos{};
  glm::vec3 m_target{};
  glm::vec3 m_up{};
  glm::vec3 m_world_up{};
  glm::vec3 m_right{};
  glm::vec3 m_front{};
  float m_yaw;
  float m_pitch;

public:
  Camera(glm::vec3 pos = glm::vec3(0), glm::vec3 target = glm::vec3(0),
         glm::vec3 world_up = glm::vec3(0, 1, 0), float yaw = -90.0f,
         float pitch = 0.0f);
  void set_pos(glm::vec3 &&new_pos);
  glm::vec3 &get_pos();
  glm::mat4 get_look_at() const;
  void keyboard_input(GLFWwindow *window, double delta_t);
  /*void mouse_input(GLFWwindow *window, double delta_t);*/

  float yaw();
  float pitch();
  void set_yaw(float);
  void set_pitch(float);

private:
  void update_vectors();
};

#endif
