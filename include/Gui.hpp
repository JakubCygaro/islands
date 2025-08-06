#include "Font.hpp"
#include "Object.hpp"
#include <glm/ext/vector_float3.hpp>
#include <memory>
#ifndef GUI_HPP
#define GUI_HPP

namespace gui {
struct DebugMenu {
    bool do_face_culling {true};
    bool do_wireframe {false};
};
struct SpawnMenu {
    float mass {};
    float initial_velocity {};
    glm::vec3 color { glm::vec3(0.5, 0.5, 0.5) };
};
struct GameOptionsMenu {
    float camera_speed {};
    float fov { 70.0 };
};
struct HelpMenu {
    std::string help_text{};
};
struct SelectedBodyMenu {
    glm::vec3 color;
    float mass;
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
