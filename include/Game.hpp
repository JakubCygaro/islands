#ifndef GAME_HPP
#define GAME_HPP
#include "Gui.hpp"
#include "Object.hpp"
#include <Camera.hpp>
#include <GLFW/glfw3.h>
#include <Font.hpp>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <glad/glad.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/fwd.hpp>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <vector>

struct UBO {
    uint32_t id{};
    uint32_t mount_point{};
    UBO(uint32_t id, uint32_t mp) : id(id), mount_point(mp){}
};

struct MatricesUBO : public UBO {
    glm::mat4 view{};
    glm::mat4 projection{};
    glm::mat4 text_projection{};
    MatricesUBO(uint32_t id, uint32_t mp) : UBO(id, mp){}
};
struct LightingGlobalsUBO : public UBO {
    union{
        float ambient_strength;
        glm::vec4 __ambient_s_pad{};
    };
    glm::vec4 camera_pos;
    LightingGlobalsUBO(uint32_t id, uint32_t mp) : UBO(id, mp){}
};

struct UniformBuffers {
    MatricesUBO matrices { 0, 0 };
    LightingGlobalsUBO lighting_globals { 0, 1 };
};

struct SSBO : public UBO {
    SSBO(uint32_t id, uint32_t mp) : UBO(id, mp){}
};

struct LightSourcesSSBO : public SSBO {
    size_t size{}, cap{};
    LightSourcesSSBO(uint32_t id, uint32_t mp) : SSBO(id, mp){}
};

struct LightSource {
    union{
        glm::vec3 position;
        glm::vec4 __pos_pad;
    };
    union{
        glm::vec3 color;
        glm::vec4 __color_pad;
    };
    union{
        float att_linear{0.09};
        glm::vec4 __att_l_pad;
    };
    union {
        float att_quadratic{0.032};
        glm::vec4 __att_q_pad;
    };
};
struct SSBuffers {
    LightSourcesSSBO light_sources { 0, 2 };
};

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
    size_t m_lightsources_cap {1};
    GLFWwindow* m_window_ptr { nullptr };
    Camera m_camera;

    UniformBuffers m_ubos {};
    SSBuffers m_ssbos{};
    std::vector<std::shared_ptr<obj::CelestialBody>> m_bodies {};
    std::unordered_map<obj::Star*, LightSource> m_stars{};
    gui::GameUI m_gui {};
    KeybindHandler m_keybinds {};

private:
    void initialize();
    void initialize_uniforms();
    void initialize_key_bindings();
    void update();
    void update_bodies_pos();
    void update_buffers();
    void render();
    void continuos_key_input();
    void framebuffer_size_handler(GLFWwindow* window, int width, int height);
    void draw_gui();
    void render_2d();

    void key_handler(GLFWwindow* window, int key, int scancode, int action,
        int mods);
    void mouse_handler(GLFWwindow* window, double x, double y);
    void scroll_handler(GLFWwindow* window, double xoffset, double yoffset);
    void mouse_button_handler(GLFWwindow* window, int button, int action,
        int mods);

    void add_planet(obj::Planet new_planet);
    void remove_planet(obj::Planet* planet);
    void add_star(obj::Star new_start);
    void remove_star(obj::Star* star);
    void buffer_light_source(size_t offset, obj::Star* star);
public:
    Game() = delete;
    ~Game();
    Game(int32_t window_width, int32_t window_height);
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    void run();
};
#endif
