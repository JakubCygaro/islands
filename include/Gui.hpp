#ifndef GUI_HPP
#define GUI_HPP
#include "Font.hpp"
#include "Object.hpp"
#include <atomic>
#include <cstdint>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <sstream>

#define DEFAULT_RESOLUTION 5

namespace gui {

enum class TrailCompStatus : int {
    Idle,
    Running,
    Finished,
    Terminated
};
class CancelationToken {
    std::atomic<bool> m_is_cancelled = false;

public:
    inline CancelationToken() { }
    inline CancelationToken(CancelationToken&& other)
        : m_is_cancelled(other.m_is_cancelled.load())
    {
    }
    inline CancelationToken& operator=(CancelationToken&& other)
    {
        m_is_cancelled.store(other.m_is_cancelled.load());
        return *this;
    }
    inline void cancel()
    {
        m_is_cancelled.store(true);
    }
    inline bool is_cancelled() const
    {
        return m_is_cancelled.load();
    }
};
struct DebugMenu {
    bool do_face_culling { true };
    bool draw_wireframe { false };
    bool draw_normals { false };
};
struct SpawnMenu {
    float mass {};
    float initial_velocity {};
    glm::vec3 color { glm::vec3(0.5, 0.5, 0.5) };
    bool is_star { false };
    char name[256] = "";
};
struct GameOptionsMenu {
    bool draw_selection_marker {true};
    bool draw_labels {true};
    bool draw_skybox {true};
    bool do_collision {true};
    float camera_speed {};
    float fov { 70.0 };
    bool draw_grid { true };
    bool draw_trails { true };
    float grid_scale { 30.0 };
    glm::vec4 grid_color { 1.0, 1.0, 1.0, 0.025 };
    struct Resolution {
        int32_t width {}, height {};
        std::string str {};
        inline Resolution(uint32_t w, uint32_t h)
            : width(w)
            , height(h)
        {
            std::stringstream ss;
            ss << w << 'x' << h;
            str = ss.str();
        }
    };
    inline static Resolution get_default_resolution() { return resolutions[DEFAULT_RESOLUTION]; }
    int current = DEFAULT_RESOLUTION;
    inline static const std::vector<Resolution> resolutions = {
        { 640, 360 },
        { 640, 480 },
        { 800, 600 },
        { 1024, 768 },
        { 1280, 720 },
        { 1280, 800 },
        { 1280, 1024 },
        { 1440, 900 },
        { 1600, 900 },
        { 1600, 1200 },
        { 1680, 1050 },
        { 1920, 1080 },
        { 1920, 1200 },
    };
    inline static const char* get_resolution(void* data, int idx)
    {
        (void)data;
        return resolutions[idx].str.c_str();
    }
};
using resolution_t = GameOptionsMenu::Resolution;
struct HelpMenu {
    std::string help_text {};
};
struct SelectedBodyMenu {
    bool track{false};
    glm::vec3 color{};
    float mass{};
    glm::vec3 velocity{};
    float speed{};
    glm::vec4 trail_color;
    glm::vec3 position;
    glm::vec4 trajectory_color;
    obj::Trail trajectory_trail;
    std::atomic<TrailCompStatus> trajectory_status { TrailCompStatus::Idle };
    std::vector<glm::vec3> trajectory_data {};
    CancelationToken calc_cancellation {};
    char name[sizeof(SpawnMenu::name)] = "";
    std::string texture_name{};
    float rotation_speed{};
    float axial_tilt{};
};
struct GameUI {
    inline static const glm::vec3 EDIT_MODE_TEXT_COLOR = { .0, .7, .0 };
    inline static const glm::vec3 NORMAL_MODE_TEXT_COLOR = { .0, .5, .8 };

    std::weak_ptr<obj::CelestialBody> selected_body {};
    SelectedBodyMenu selected_body_menu {};
    bool spawn_menu_enabled { false };
    SpawnMenu spawn_menu {};
    bool game_options_menu_enabled { false };
    GameOptionsMenu game_options_menu {};
    bool help_menu_enabled { false };
    HelpMenu help_menu {};
    bool texture_menu_enabled { false };
    bool bodies_list_enabled { false };
    bool exit_notification { false };
#ifdef DEBUG
    bool debug_menu_enabled { false };
    DebugMenu debug_menu {};
#endif
    font::Text2D mode;
    font::Text2D paused;
    font::Text2D game_version;
    font::Text2D fps_count;
    GameUI();
};
}

#endif
