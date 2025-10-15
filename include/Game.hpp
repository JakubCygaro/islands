#ifndef GAME_HPP
#define GAME_HPP
#include <deque>
#include <filesystem>
#include <queue>
#include <set>
#include <string>
#include <vector>
#include "Grid.hpp"
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
#include <Skybox.hpp>

namespace gm{

    struct Gbuffer {
    private:
        int32_t width{}, height{};
        uint32_t quad_vbo{}, quad_ebo{};
    public:
        uint32_t g_position{}, g_normal{}, g_color_spec{}, fbo{}, rbo{};
        uint32_t quad_vao{};
        Gbuffer();
        Gbuffer(int32_t width, int32_t height);
        Gbuffer(const Gbuffer& other);
        Gbuffer& operator=(const Gbuffer& other);
        Gbuffer(Gbuffer&& other);
        Gbuffer& operator=(Gbuffer&& other);
        ~Gbuffer();

        void bind() const;
        void unbind(uint32_t fbo = 0) const;
    };

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
        size_t size{};
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
        float att_linear{0.014};
        float att_quadratic{0.0007};
        float radius{0};
        float __att_pad{};
        uint32_t shadow_map_id{};
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
}

namespace std {
template <>
struct hash<gm::BindKey> {
    inline std::size_t operator()(const gm::BindKey& bk) const
    {
        using std::size_t;
        using std::hash;
        return ((((bk.m_key << 1) + ((int)bk.m_mode << 3)) ^ bk.m_mods << bk.m_action) >> 1);
    }
};
}
namespace gm {

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
        inline static const int DELTA_T_MEAN_SAMPLE_SIZE = 60;
        enum struct MaximizeState {
            Maximize,
            Minimize,
            DoNothing
        };
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
        MaximizeState m_maximize { MaximizeState::DoNothing };
        size_t m_lightsources_cap {1};
        Gbuffer m_gbuffer{};
        GLFWwindow* m_window_ptr { nullptr };
        Camera m_camera;

        UniformBuffers m_ubos {};
        SSBuffers m_ssbos{};
        std::vector<std::shared_ptr<obj::CelestialBody>> m_bodies {};
        std::vector<LightSource> m_light_data{};
        gui::GameUI m_gui {};
        KeybindHandler m_keybinds {};

        std::unique_ptr<Grid> m_grid = nullptr;

        int32_t m_fps{};
        double m_last_fixed_update_t {};
        double m_last_fps_update_t {};
        bool m_fixed_update = false;
        bool m_typing = false;

        std::deque<double> m_delta_t_record{};
        std::queue<glm::vec4> m_imgui_window_rects{};
        struct KeyEvent {
            int key;
            int scancode;
            int action;
            int mods;
        };
        std::queue<KeyEvent> m_key_events{};
        std::unique_ptr<Skybox> m_skybox = nullptr;

        // a map of textures that have been loaded by the game and are availabe for celestial bodies
        std::unordered_map<std::filesystem::path, std::shared_ptr<obj::Texture>> m_loaded_textures{};
        // a set of texture file paths that can be loaded
        std::set<std::filesystem::path> m_unloaded_textures{};

    private:
        void initialize();
        void initialize_uniforms();
        void initialize_key_bindings();
        void initialize_singletons();
        void update();
        void update_bodies();
        void update_buffers();
        void render();
        void render_gbuffer();
        void render_light_volumes();
        void remove_body(obj::CelestialBody* body);
        void continuos_key_input();
        void framebuffer_size_handler(GLFWwindow* window, int width, int height);
        void window_maximize_handler(GLFWwindow* window, int maximized);
        void window_refresh_handler(GLFWwindow* window);
        void draw_gui();
        void draw_game_options_gui();
        void draw_selected_body_gui();
        void draw_spawn_menu_gui();
        void draw_help_menu_gui();
        void draw_texture_menu_gui();
#ifdef DEBUG
        void draw_debug_menu_gui();
#endif
        void draw_body_list_gui();
        void render_2d();

        void key_handler(GLFWwindow* window, int key, int scancode, int action,
            int mods);
        void mouse_handler(GLFWwindow* window, double x, double y);
        void scroll_handler(GLFWwindow* window, double xoffset, double yoffset);
        void mouse_button_handler(GLFWwindow* window, int button, int action,
            int mods);

        void add_planet(obj::Planet new_planet);
        void remove_planet(obj::Planet* planet);
        void add_star(obj::Star&& new_start);
        void remove_star(obj::Star* star);
        void collect_light_sources();
        void buffer_light_data();
        void schedule_selected_body_trajectory_calc();
        void on_body_selected(std::shared_ptr<obj::CelestialBody> body);
        void load_custom_textures_paths();
        void load_texture_from_path(const std::filesystem::path&);
    public:
        ~Game();
        Game();
        Game(const Game&) = delete;
        Game& operator=(const Game&) = delete;
        void run();
    };
}
#endif
