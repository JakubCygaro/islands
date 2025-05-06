#include "Object.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shader/Shader.hpp"
#include <GLFW/glfw3.h>
#include <Game.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <type_traits>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <memory>

const float GRAV_CONST = 6.674e-11;

static Game* get_game_instance_ptr_from_window(GLFWwindow* window);

Game::Game(int32_t window_width, int32_t window_height):
    m_width{window_width},
    m_height{window_height},
    m_camera{Camera(glm::vec3(0,0,3), glm::vec3(0))},
    m_uniform_buffer{0},
    m_fov{70}
{
    initialize();
}
Game::~Game(){
    glDeleteBuffers(1, &m_uniform_buffer);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    m_bodies.clear();
    glfwDestroyWindow(m_window_ptr);
    glfwTerminate();
}
void Game::initialize() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window_ptr = glfwCreateWindow(m_width, m_height, "Islands", NULL, NULL);
    if (m_window_ptr == NULL)
    {
        throw std::runtime_error("Failed to create the window");
        glfwTerminate();
    }
    glfwMakeContextCurrent(m_window_ptr);
    glfwSetWindowUserPointer(m_window_ptr, this);
    auto framebuffer_size_callback = [](GLFWwindow* window, int w, int h){
        Game* instance = get_game_instance_ptr_from_window(window);
        instance->framebuffer_size_handler(window, w, h);
    };
    glfwSetFramebufferSizeCallback(m_window_ptr, framebuffer_size_callback);
    auto mouse_callback = [](GLFWwindow* window, double x, double y){
        Game* instance = get_game_instance_ptr_from_window(window);
        instance->mouse_handler(window, x, y);
    };
    glfwSetCursorPosCallback(m_window_ptr, mouse_callback);
    auto key_callback = [](GLFWwindow* window, int key, int scancode, int action, int mods){
        Game* instance = get_game_instance_ptr_from_window(window);
        instance->key_handler(window, key, scancode, action, mods);
    };
    glfwSetKeyCallback(m_window_ptr, key_callback);
    auto scroll_callback = [](GLFWwindow* window, double xoffset, double yoffset){
        Game* instance = get_game_instance_ptr_from_window(window);
        instance->scroll_handler(window, xoffset, yoffset);
    };
    glfwSetScrollCallback(m_window_ptr, scroll_callback);

    // tell GLFW to capture our mouse
    /*glfwSetInputMode(m_window_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);*/

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    /*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/
    m_projection = glm::perspective(glm::radians(m_fov),
            (float)m_width / (float)m_height, 0.1f, 1000.0f);
    /*std::cout << "m_projection = " << glm::to_string(m_projection) << std::endl;*/

    glGenBuffers(1, &m_uniform_buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_buffer);
    glBufferData(GL_UNIFORM_BUFFER,
            // view                     projection
            sizeof(glm::mat4) + sizeof(glm::mat4),
            NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER,
            sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(m_projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_uniform_buffer);


    auto c_body = std::make_shared<obj::CelestialBody>(
                obj::CelestialBody(nullptr, {2.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 100)
                );
    c_body->set_color({1.0, .1, .1});
    m_bodies.push_back(c_body);

    c_body = std::make_shared<obj::CelestialBody>(
                obj::CelestialBody(nullptr, {-10.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, 10)
                );
    c_body->set_color({0.0, 1.0, .1});
    m_bodies.push_back(c_body);

    c_body = std::make_shared<obj::CelestialBody>(
                obj::CelestialBody(nullptr, {0.0f, 0.0f, 10.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 25)
                );
    c_body->set_color({0.0, 0.0, 1.0});
    m_bodies.push_back(c_body);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(m_window_ptr, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    m_gui.game_options_menu.camera_speed = m_camera.get_speed();
    m_gui.game_options_menu.fov = m_fov;
}
void Game::run() {
    while(!glfwWindowShouldClose(m_window_ptr)){
        m_current_frame_t = glfwGetTime();
        m_delta_t = m_current_frame_t - m_last_frame_t;
        m_last_frame_t = m_current_frame_t;
        update();
        render();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(m_window_ptr);
    }
}
void Game::update() {
    glfwPollEvents();
    m_view = m_camera.get_look_at();
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_view));

    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(m_projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    /*ImGui::ShowDemoWindow();*/
    continuos_key_input();
    if (m_gui_enabled) draw_gui();
    if(!m_paused){
        update_bodies_pos();
    }
}
void Game::update_bodies_pos() {
    for(size_t body = 0; body < m_bodies.size(); body++){
        for(size_t next_body = body + 1; next_body < m_bodies.size(); next_body++){
            //https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form
            auto b_1 = m_bodies[body];
            auto b_2 = m_bodies[next_body];

            auto m_1 = b_1->get_mass() * obj::CelestialBody::MASS_BOOST_FACTOR;
            auto m_2 = b_2->get_mass() * obj::CelestialBody::MASS_BOOST_FACTOR;

            auto r_21 = b_2->get_pos() - b_1->get_pos();
            auto r_21_hat = glm::normalize(r_21);
            auto distance = glm::distance(b_1->get_pos(), b_2->get_pos());
            //attraction force
            auto f_21 = -GRAV_CONST * ((m_1 * m_2) / (distance * distance)) * r_21_hat;
            auto f_12 = -f_21;

            b_1->set_speed(b_1->get_speed() + f_12);
            b_2->set_speed(b_2->get_speed() + f_21);
        }
    }
    for (auto& obj : m_bodies){
        obj->update(m_delta_t);
    }
}
void Game::render() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(auto& c_obj : m_bodies){
        c_obj->render();
    }
}
void Game::draw_gui() {
    if(m_gui.spawn_menu_enabled){
        ImGui::Begin("Spawn menu", &m_gui.spawn_menu_enabled);
        ImGui::ColorEdit3("Color", glm::value_ptr(m_gui.spawn_menu.color));
        ImGui::SliderFloat("Mass", &m_gui.spawn_menu.mass, 0.001, 100, NULL, 0);
        if(m_gui.spawn_menu.mass <= 0) m_gui.spawn_menu.mass = 0.001;
        ImGui::SliderFloat("Initial velocity", &m_gui.spawn_menu.initial_velocity, 0, 10, NULL, 0);
        ImGui::End();
    }

    if(m_gui.game_options_menu_enabled){
        ImGui::Begin("Game Options", &m_gui.game_options_menu_enabled, 0);
        if(ImGui::SliderFloat("Camera speed", &m_gui.game_options_menu.camera_speed, 0, 100, NULL, ImGuiSliderFlags_AlwaysClamp)){
            m_camera.set_speed(m_gui.game_options_menu.camera_speed);
        }
        if(ImGui::SliderFloat("Camera FOV", &m_gui.game_options_menu.fov, 30, 120, NULL, ImGuiSliderFlags_AlwaysClamp)){
            m_fov = m_gui.game_options_menu.fov;
            m_projection = glm::perspective(glm::radians(m_fov),
                    (float)m_width / (float)m_height, 0.1f, 1000.0f);
        }
        ImGui::End();
    }
}
void Game::key_handler(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
        glfwSetWindowShouldClose(m_window_ptr, true);
    }
    // toggle the gui with [E]dit
    if (key == GLFW_KEY_E && action == GLFW_PRESS){
        /*std::printf("editor mode\n");*/
        m_gui_enabled = !m_gui_enabled;
        if(m_gui_enabled)
            glfwSetInputMode(m_window_ptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(m_window_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if(key == GLFW_KEY_P && action == GLFW_PRESS){
        m_paused = !m_paused;
    }
    // Shift + S -> spawn object
    if(key == GLFW_KEY_S && action == GLFW_PRESS && glfwGetKey(m_window_ptr, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        auto front = m_camera.get_front();
        auto pos = m_camera.get_pos() + front;
        auto vel = glm::normalize(front) * m_gui.spawn_menu.initial_velocity;
        auto obj = std::make_shared<obj::CelestialBody>(obj::CelestialBody(nullptr, pos, vel, glm::vec3(0), m_gui.spawn_menu.mass));
        obj->set_color(m_gui.spawn_menu.color);
        m_bodies.emplace_back(std::move(obj));
    }
    // Editor mode specifyc keybinds
    if(m_gui_enabled){
        if(key == GLFW_KEY_O && action == GLFW_PRESS){
            m_gui.game_options_menu_enabled = !m_gui.game_options_menu_enabled;
        }
        if(key == GLFW_KEY_S && action == GLFW_PRESS){
            m_gui.spawn_menu_enabled = !m_gui.spawn_menu_enabled;
        }
    }
    // Simulation mode specific keybinds
    else {
        m_camera.keyboard_input(m_window_ptr, m_delta_t);
    }
}

void Game::continuos_key_input() {
    // Editor mode specifyc keybinds
    if(m_gui_enabled){

    }
    // Simulation mode specific keybinds
    else {
        m_camera.keyboard_input(m_window_ptr, m_delta_t);
    }
}
void Game::framebuffer_size_handler(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}
void Game::mouse_handler(GLFWwindow* window, double xpos, double ypos){
    if(m_first_mouse){
        m_last_mouse_x = xpos;
        m_last_mouse_y = ypos;
        m_first_mouse = false;
    }
    float x_offset = xpos - m_last_mouse_x;
    float y_offset = m_last_mouse_y - ypos;
    m_last_mouse_x = xpos;
    m_last_mouse_y = ypos;

    if(m_gui_enabled) return;
    const float sensitivity = 0.1f;
    x_offset *= sensitivity;
    y_offset *= sensitivity;

    auto yaw = m_camera.get_yaw();
    yaw += x_offset;
    m_camera.set_yaw(yaw);
    auto pitch = m_camera.get_pitch();
    pitch += y_offset;
    m_camera.set_pitch(pitch);
}

void Game::scroll_handler(GLFWwindow* window, double xoffset, double yoffset) {
    m_camera.set_speed(m_camera.get_speed() + yoffset);
}

static Game* get_game_instance_ptr_from_window(GLFWwindow* window){
    Game* instance = static_cast<Game*>(glfwGetWindowUserPointer(window));
    return instance;
}
