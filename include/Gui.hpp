#include "Font.hpp"
#include "Object.hpp"
#include <cstdint>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <sstream>
#ifndef GUI_HPP
#define GUI_HPP

namespace gui {
struct DebugMenu {
    bool do_face_culling {true};
    bool draw_wireframe {false};
    bool draw_normals {false};
};
struct SpawnMenu {
    float mass {};
    float initial_velocity {};
    glm::vec3 color { glm::vec3(0.5, 0.5, 0.5) };
    bool is_star {false};
};
struct GameOptionsMenu {
    float camera_speed {};
    float fov { 70.0 };
    struct Resolution {
        uint32_t width{}, height{};
        std::string str{};
        inline Resolution( uint32_t w, uint32_t h ) : width(w), height(h) {
            std::stringstream ss;
            ss << w << 'x' << h;
            str = ss.str();
        }
    };
    int current = 5;
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
    inline static const char* get_resolution(void* data, int idx){
        (void)data;
        return resolutions[idx].str.c_str();
    }
};
struct HelpMenu {
    std::string help_text{};
};
struct SelectedBodyMenu {
    glm::vec3 color;
    float mass;
    glm::vec3 velocity;
    float speed;
};
struct GameUI {
    inline static const glm::vec3 EDIT_MODE_TEXT_COLOR = { .0, .7, .0 };
    inline static const glm::vec3 NORMAL_MODE_TEXT_COLOR = { .0, .5, .8 };

    std::weak_ptr<obj::CelestialBody> selected_body {};
    SelectedBodyMenu selected_body_menu{};
    bool spawn_menu_enabled { false };
    SpawnMenu spawn_menu {};
    bool game_options_menu_enabled { false };
    GameOptionsMenu game_options_menu {};
    bool help_menu_enabled { false };
    HelpMenu help_menu {};
#ifdef DEBUG
    bool debug_menu_enabled { false };
    DebugMenu debug_menu {};
#endif
    font::Text2D mode{};
    font::Text2D paused{};
    font::Text2D game_version{};
    GameUI();
};
}

#endif
