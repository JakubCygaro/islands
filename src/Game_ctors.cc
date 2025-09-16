#include <Game.hpp>
#include "Font.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <Game.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

Game::Game()
    : m_width { m_gui.game_options_menu.get_default_resolution().width }
    , m_height {  m_gui.game_options_menu.get_default_resolution().height  }
    , m_fov { 70 }
    , m_camera { Camera(glm::vec3(0, 0, 3), glm::vec3(0)) }
    , m_ubos {}
{
    initialize();
    initialize_key_bindings();
    m_gui.help_menu.help_text = m_keybinds.gen_help_text();
}
Game::~Game()
{
    glDeleteBuffers(1, &m_ubos.matrices.id);
    glDeleteBuffers(1, &m_ubos.lighting_globals.id);
    glDeleteBuffers(1, &m_ssbos.light_sources.id);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    m_bodies.clear();
    glfwDestroyWindow(m_window_ptr);
    glfwTerminate();
}
