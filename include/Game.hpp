#ifndef GAME_HPP
#define GAME_HPP
#include "Gui.hpp"
#include "Object.hpp"
#include <Camera.hpp>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <glad/glad.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <map>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>

enum class BindMode {
    Any = 1,
    Normal,
    Editor,
};
struct BindKey {
private:
    int m_key {}, m_action {}, m_mods {};
    BindMode m_mode {};
public:
    BindKey(int key, int action, BindMode editor, int mods);
private:
    friend struct std::hash<BindKey>;
    friend std::ostream& operator<<(std::ostream &os, const BindKey &kb);
    friend std::ostream& operator>>(std::ostream &os, const BindKey &kb);
    friend bool operator==(const BindKey& a, const BindKey& b);
    friend class KeybindHandler;
};

std::ostream& operator<<(std::ostream& os, const BindKey& kb);
std::ostream& operator>>(std::ostream& os, const BindKey& kb);

namespace std {
template <>
struct hash<BindKey> {
    inline std::size_t operator()(const BindKey& bk) const
    {
        using std::size_t;
        using std::hash;
        return ((((bk.m_key << 1) + ((int)bk.m_mode << 3)) ^ bk.m_mods << bk.m_action) >> 1);
    }
};
} // namespace std

class KeybindHandler {
private:
    std::unordered_map<BindKey, std::pair<std::function<void(void)>, std::string>> m_bindings {};

public:
    void add_binding(const int& key, const int& action, const BindMode& mode,
        std::function<void(void)>&& handler, const std::string&& help_text, const int& mods = 0);
    void handle(const int& key, const int& action, const BindMode& mode, const int& mods) const;
    std::string gen_help_text() const;
};

class Game final {
private:
    int32_t m_width;
    int32_t m_height;
    double m_delta_t {};
    double m_last_frame_t {};
    double m_current_frame_t {};
    double m_last_mouse_x {};
    double m_last_mouse_y {};
    float m_fov {};
    bool m_first_mouse { true };
    bool m_wireframe { false };
    bool m_gui_enabled { true };
    bool m_paused { false };
    GLFWwindow* m_window_ptr { nullptr };
    Camera m_camera;

    uint32_t m_uniform_buffer {};
    glm::mat4 m_view {};
    glm::mat4 m_projection {};
    std::vector<std::shared_ptr<obj::CelestialBody>> m_bodies {};
    gui::GameUI m_gui {};
    KeybindHandler m_keybinds {};

private:
    void initialize();
    void initialize_key_bindings();
    void update();
    void update_bodies_pos();
    void render();
    void continuos_key_input();
    void framebuffer_size_handler(GLFWwindow* window, int width, int height);
    void draw_gui();

    void key_handler(GLFWwindow* window, int key, int scancode, int action,
        int mods);
    void mouse_handler(GLFWwindow* window, double x, double y);
    void scroll_handler(GLFWwindow* window, double xoffset, double yoffset);
    void mouse_button_handler(GLFWwindow* window, int button, int action,
        int mods);

public:
    Game() = delete;
    ~Game();
    Game(int32_t window_width, int32_t window_height);
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    void run();
};
#endif
