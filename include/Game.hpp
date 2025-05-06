#ifndef GAME_HPP
#define GAME_HPP
#include "Object.hpp"
#include "Gui.hpp"
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
    float m_fov{};
    bool m_first_mouse{true};
    bool m_wireframe{false};
    bool m_gui_enabled{true};
    bool m_paused{false};
    GLFWwindow* m_window_ptr{nullptr};
    Camera m_camera;

    uint32_t m_uniform_buffer{};
    glm::mat4 m_view{};
    glm::mat4 m_projection{};
    std::vector<std::shared_ptr<obj::CelestialBody>> m_bodies{};
    gui::GameUI m_gui{};
private:
    void initialize();
    void update();
    void update_bodies_pos();
    void render();
    void continuos_key_input();
    void framebuffer_size_handler(GLFWwindow* window, int width, int height);
    void draw_gui();

    void key_handler(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mouse_handler(GLFWwindow* window, double x, double y);
    void scroll_handler(GLFWwindow* window, double xoffset, double yoffset);
public:
    Game() = delete;
    ~Game();
    Game(int32_t window_width, int32_t window_height);
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    void run();
};
#endif
