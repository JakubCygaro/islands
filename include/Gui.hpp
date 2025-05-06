#include "Object.hpp"
#include <memory>
#ifndef GUI_HPP
#define GUI_HPP

namespace gui {
    struct SpawnMenu {
        float mass{};
        float initial_velocity{};
        glm::vec3 color{glm::vec3(0.5, 0.5, 0.5)};
    };
    struct GameOptionsMenu {
        float camera_speed{};
        float fov{70.0};
    };
    struct GameUI {
        std::weak_ptr<obj::CelestialBody> selected_body{};
        bool spawn_menu_enabled{false};
        SpawnMenu spawn_menu{};
        bool game_options_menu_enabled{false};
        GameOptionsMenu game_options_menu{};
        GameUI();
    };
}

#endif
