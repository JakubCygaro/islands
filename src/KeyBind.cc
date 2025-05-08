#include "Game.hpp"
#include <GLFW/glfw3.h>

KeyBind::KeyBind(int key, int action, BindMode mode, int mods)
    : m_key { key }
    , m_action { action }
    , m_mode { mods }
    , m_mods {mods}
{
}

void KeybindHandler::add_binding(const int& key, const int& action,
    const BindMode& mode,
    std::function<void(void)>&& handler, const int& mods)
{
    KeyBind bind{key, action, mode, mods};
    m_bindings[bind] = handler;
}
void KeybindHandler::handle(const int& key, const int& action, const BindMode& mode, const int& mods) const {
    auto kb = (KeyBind){key, action, mode, mods};
    if (m_bindings.find(kb) != m_bindings.end())
        m_bindings.at(kb)();
}
