#include <Game.hpp>
#include "Font.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <Game.hpp>
#include <cmath>
#include <cstdio>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

Game::Game(int32_t window_width, int32_t window_height)
    : m_width { window_width }
    , m_height { window_height }
    , m_fov { 70 }
    , m_camera { Camera(glm::vec3(0, 0, 3), glm::vec3(0)) }
    , m_uniform_buffer { 0 }
{
    initialize();
    initialize_key_bindings();
    m_gui.help_menu.help_text = m_keybinds.gen_help_text();
}
Game::~Game()
{
    glDeleteBuffers(1, &m_uniform_buffer);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    m_bodies.clear();
    glfwDestroyWindow(m_window_ptr);
    glfwTerminate();
}
