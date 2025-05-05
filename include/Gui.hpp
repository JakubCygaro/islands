#include "Object.hpp"
#include <memory>
#ifndef GUI_HPP
#define GUI_HPP

namespace gui {
    struct GameUI {
        std::weak_ptr<obj::CelestialBody> selected_body{};

        GameUI();
    };
}

#endif
