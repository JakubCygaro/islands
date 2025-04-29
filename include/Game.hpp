#ifndef GAME_HPP
#define GAME_HPP
#include "Object.hpp"
#include <glad/glad.h>
#include <Camera.hpp>
#include <cstdint>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>
#include <stdexcept>
#include <vector>

class Game final {
private:
    int32_t m_width;
    int32_t m_height;
    double m_delta_t{};
    double m_last_frame_t{};
    double m_current_frame_t{};
    double m_last_mouse_x{};
    double m_last_mouse_y{};
    bool m_first_mouse{true};
    bool m_wireframe{false};
    GLFWwindow* m_window_ptr{nullptr};
    Camera m_camera;

    uint32_t m_uniform_buffer{};
    glm::mat4 m_view{};
    glm::mat4 m_projection{};
    std::vector<std::shared_ptr<obj::CelestialBody>> m_bodies{};
private:
    void initialize();
    void update();
    void render();
    void keyboard_input();
    void framebuffer_size_handler(GLFWwindow* window, int width, int height);
    void mouse_handler(GLFWwindow* window, double x, double y);
public:
    Game() = delete;
    ~Game();
    Game(int32_t window_width, int32_t window_height);
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    void run();
};
#endif
