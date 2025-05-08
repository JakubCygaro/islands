#include "Game.hpp"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <utility>

BindKey::BindKey(int key, int action, BindMode mode, int mods)
    : m_key { key }
    , m_action { action }
    , m_mods { mods }
    , m_mode { mode }
{
}

std::ostream& operator<<(std::ostream& os, const BindKey& kb)
{
    os << "KeyBind { key: " << (char)kb.m_key << " action: " << kb.m_action
       << " mode: " << (int)kb.m_mode << " mods: " << kb.m_mods << " }"
       << std::endl;
    return os;
}
std::ostream& operator>>(std::ostream& os, const BindKey& kb)
{
    os << kb;
    return os;
}
bool operator==(const BindKey& a, const BindKey& b){
    return (a.m_key == b.m_key) && (a.m_action == b.m_action) && (a.m_mods == b.m_mods) && (a.m_mode == b.m_mode);
}
void KeybindHandler::add_binding(const int& key, const int& action,
    const BindMode& mode,
    std::function<void(void)>&& handler,
    const std::string&& help_text,
    const int& mods)
{
    BindKey bind { key, action, mode, mods };
    auto hndl = std::make_pair(handler, help_text);
    m_bindings[bind] = hndl;
}
void KeybindHandler::handle(const int& key, const int& action,
    const BindMode& mode, const int& mods) const
{
    auto kb = (BindKey) { key, action, mode, mods };
    if (m_bindings.contains(kb))
        m_bindings.at(kb).first();
}
std::string KeybindHandler::gen_help_text() const {
    std::stringstream ss;

    for (auto&[k, v] : m_bindings){
        const char* key = glfwGetKeyName(k.m_key, glfwGetKeyScancode(k.m_key));
        if (!key) {
            switch (k.m_key) {
            case GLFW_KEY_SPACE:
                key = "space";
                break;
            case GLFW_KEY_ESCAPE:
                key = "esc";
                break;
            case GLFW_KEY_ENTER:
                key = "enter";
                break;
            default:
                continue;
            }
        }
        ss << "[" << key;
        switch (k.m_mods) {
        case GLFW_MOD_SHIFT:
            ss << " + SHIFT";
            break;
        case GLFW_MOD_CONTROL:
            ss << " + CTRL";
            break;
        case GLFW_MOD_ALT:
            ss << " + ALT";
            break;
        }
        ss << "] : " << v.second << std::endl;
    }

    return ss.str();
}
