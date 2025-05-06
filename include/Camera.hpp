#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <cstdio>
#include <Util.hpp>

class Camera final {
private:
  glm::vec3 m_target{};
  glm::vec3 m_up{};
  glm::vec3 m_world_up{};
  glm::vec3 m_right{};
  glm::vec3 m_front{};
  PROPERTY(float, yaw);
  PROPERTY(float, pitch);
  PROPERTY(glm::vec3, pos);
  PROPERTY(float, speed);

public:
  Camera(glm::vec3 pos = glm::vec3(0), glm::vec3 target = glm::vec3(0),
         glm::vec3 world_up = glm::vec3(0, 1, 0), float yaw = -90.0f,
         float pitch = 0.0f);
  glm::mat4 get_look_at() ;
  void keyboard_input(GLFWwindow *window, double delta_t);
  /*void mouse_input(GLFWwindow *window, double delta_t);*/
  glm::vec3 get_front() const;

private:
  void update_vectors();
};

#endif
