#include "Font.hpp"
#include <files.hpp>
#include "Object.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "shader/Shader.hpp"
#include <GLFW/glfw3.h>
#include <Game.hpp>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <optional>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <memory>
#include "global_declarations.hpp"
const float GRAV_CONST = 6.674e-11;

namespace {
    Game* get_game_instance_ptr_from_window(GLFWwindow* window)
    {
        Game* instance = static_cast<Game*>(glfwGetWindowUserPointer(window));
        return instance;
    }
}

void Game::buffer_light_source(size_t offset, obj::Star* star){

    LightSource source_data{};

    source_data.position = star->get_pos();
    source_data.color = star->get_color();
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
        offset * sizeof(LightSource),
        sizeof(LightSource),
        &source_data);
}

void Game::initialize()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window_ptr = glfwCreateWindow(m_width, m_height, "Islands", NULL, NULL);
    if (m_window_ptr == NULL) {
        throw std::runtime_error("Failed to create the window");
        glfwTerminate();
    }
    glfwMakeContextCurrent(m_window_ptr);
    glfwSetWindowUserPointer(m_window_ptr, this);
    auto framebuffer_size_callback = [](GLFWwindow* window, int w, int h) {
        Game* instance = get_game_instance_ptr_from_window(window);
        instance->framebuffer_size_handler(window, w, h);
    };
    glfwSetFramebufferSizeCallback(m_window_ptr, framebuffer_size_callback);
    auto mouse_callback = [](GLFWwindow* window, double x, double y) {
        Game* instance = get_game_instance_ptr_from_window(window);
        instance->mouse_handler(window, x, y);
    };
    glfwSetCursorPosCallback(m_window_ptr, mouse_callback);
    auto key_callback = [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        Game* instance = get_game_instance_ptr_from_window(window);
        instance->key_handler(window, key, scancode, action, mods);
    };
    glfwSetKeyCallback(m_window_ptr, key_callback);
    auto scroll_callback = [](GLFWwindow* window, double xoffset, double yoffset) {
        Game* instance = get_game_instance_ptr_from_window(window);
        instance->scroll_handler(window, xoffset, yoffset);
    };
    glfwSetScrollCallback(m_window_ptr, scroll_callback);
    auto mouse_button_callback = [](GLFWwindow* window, int button, int action, int mods) {
        Game* instance = get_game_instance_ptr_from_window(window);
        instance->mouse_button_handler(window, button, action, mods);
    };
    glfwSetMouseButtonCallback(m_window_ptr, mouse_button_callback);

    // tell GLFW to capture our mouse
    /*glfwSetInputMode(m_window_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);*/

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initialize GLAD");
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    initialize_uniforms();

    auto c_body = obj::Planet(nullptr, { 2.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 100);
    c_body.set_color({ 1.0, .1, .1 });
    add_planet(c_body);

    c_body = obj::Planet(nullptr, { -10.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, 10);
    c_body.set_color({ 0.0, 1.0, .1 });
    add_planet(c_body);

    c_body = obj::Planet(nullptr, { 0.0f, 0.0f, 10.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 25);
    c_body.set_color({ 0.0, 0.0, 1.0 });
    add_planet(c_body);

    auto star = obj::Star(nullptr, { 0, 5, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 5);
    star.set_color({ 0.78, 0.52, 0.06 });
    add_star(star);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(m_window_ptr, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    m_gui.game_options_menu.camera_speed = m_camera.get_speed();
    m_gui.game_options_menu.fov = m_fov;
    m_gui.help_menu_enabled = true;

    auto font = std::make_shared<font::FontBitmap>(font::load_font(files::game_data::fonts::ARCADE_TTF, 48));
    auto font_shader = std::make_shared<Shader>(Shader(shaders::TEXT_VERT, shaders::TEXT_FRAG));

    m_gui.mode = font::Text2D(font, font_shader, "Edit");
    m_gui.mode.set_pos({ 0, 0 });
    m_gui.mode.set_color(gui::GameUI::EDIT_MODE_TEXT_COLOR);

    m_gui.paused = font::Text2D(font, font_shader, "PAUSED");
    m_gui.paused.set_color({ 1.0, .0, .0 });
    m_gui.paused.set_pos({ 0, m_height - m_gui.paused.get_text_height() });

    m_gui.game_version = font::Text2D(font, font_shader, global_decl::GAME_VERSION);
    m_gui.game_version.set_color({ 1.0, 1.0, 1.0 });
    m_gui.game_version.set_scale(.5f);
    m_gui.game_version.set_pos({ m_width - m_gui.game_version.get_text_width(), m_height - m_gui.game_version.get_text_height() });
}
void Game::initialize_uniforms(){
    m_ubos.matrices.projection = glm::perspective(glm::radians(m_fov),
        (float)m_width / (float)m_height, 0.1f, 1000.0f);

    m_ubos.matrices.text_projection = glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, 0.0f, 100.0f);

    glGenBuffers(1, &m_ubos.matrices.id);
    glBindBuffer(GL_UNIFORM_BUFFER, m_ubos.matrices.id);
    glBufferData(GL_UNIFORM_BUFFER,
        // view projection text_projection
        sizeof(MatricesUBO),
        NULL, GL_STATIC_DRAW);

    //buffer projection
    glBufferSubData(GL_UNIFORM_BUFFER,
        sizeof(MatricesUBO::view), sizeof(MatricesUBO::projection), glm::value_ptr(m_ubos.matrices.projection));

    //buffer text_projection
    glBufferSubData(GL_UNIFORM_BUFFER,
        2 * sizeof(MatricesUBO::view), sizeof(MatricesUBO::text_projection), glm::value_ptr(m_ubos.matrices.text_projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, m_ubos.matrices.mount_point, m_ubos.matrices.id);

    // lighting_globals uniform buffer
    m_ubos.lighting_globals.ambient_strength = 0.02f;
    glGenBuffers(1, &m_ubos.lighting_globals.id);
    glBindBuffer(GL_UNIFORM_BUFFER, m_ubos.lighting_globals.id);
    glBufferData(GL_UNIFORM_BUFFER,
        sizeof(LightingGlobalsUBO),
        &m_ubos.lighting_globals, GL_STATIC_DRAW);

    glBufferSubData(GL_UNIFORM_BUFFER,
        0,
        sizeof(LightingGlobalsUBO::ambient_strength),
        &m_ubos.lighting_globals.ambient_strength);

    glBufferSubData(GL_UNIFORM_BUFFER,
        sizeof(LightingGlobalsUBO::__ambient_s_pad),
        sizeof(LightingGlobalsUBO::camera_pos),
        m_camera.get_pos_ptr());

    glBindBufferBase(GL_UNIFORM_BUFFER, m_ubos.lighting_globals.mount_point, m_ubos.lighting_globals.id);

    // glm::vec4 ls[2] { { 0, 0, 0, 0 }, { 1, 1, 1, 0 } };
    m_ssbos.light_sources.cap = 1;
    glGenBuffers(1, &m_ssbos.light_sources.id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbos.light_sources.id);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        m_ssbos.light_sources.cap * sizeof(LightSource),
        NULL, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_ssbos.light_sources.mount_point, m_ssbos.light_sources.id);
}
void Game::run()
{
    while (!glfwWindowShouldClose(m_window_ptr)) {
        m_current_frame_t = glfwGetTime();
        m_delta_t = m_current_frame_t - m_last_frame_t;
        m_last_frame_t = m_current_frame_t;
        update_buffers();
        update();
        render();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(m_window_ptr);
    }
}
void Game::update()
{
    glfwPollEvents();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    continuos_key_input();
    if (m_gui_enabled)
        draw_gui();
    if (!m_paused) {
        update_bodies_pos();
    }
}
void Game::update_buffers() {

    m_ubos.matrices.view = m_camera.get_look_at();
    glBindBuffer(GL_UNIFORM_BUFFER, m_ubos.matrices.id);
    glBufferSubData(GL_UNIFORM_BUFFER,
            0, sizeof(MatricesUBO::view), glm::value_ptr(m_ubos.matrices.view));

    glBufferSubData(GL_UNIFORM_BUFFER,
            sizeof(MatricesUBO::view), sizeof(MatricesUBO::projection), glm::value_ptr(m_ubos.matrices.projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, m_ubos.lighting_globals.id);
    glBufferSubData(GL_UNIFORM_BUFFER,
        sizeof(LightingGlobalsUBO::__ambient_s_pad),
        sizeof(LightingGlobalsUBO::camera_pos),
        m_camera.get_pos_ptr());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void Game::update_bodies_pos()
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbos.light_sources.id);

    auto offset = 0;
    for (size_t body = 0; body < m_bodies.size(); body++) {
        if(auto star = dynamic_cast<obj::Star*>(m_bodies[body].get()); star){
            //update light source ssbo
            buffer_light_source(offset++, star);
        }
        for (size_t next_body = body + 1; next_body < m_bodies.size(); next_body++) {
            // https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form
            auto b_1 = m_bodies[body];
            auto b_2 = m_bodies[next_body];

            auto m_1 = b_1->get_mass();
            auto m_2 = b_2->get_mass();

            auto r_21 = b_2->get_pos() - b_1->get_pos();
            auto r_21_hat = glm::normalize(r_21);
            auto distance = glm::distance(b_1->get_pos(), b_2->get_pos());
            // attraction force
            auto f_21 = -GRAV_CONST * ((m_1 * m_2) / (distance * distance)) * r_21_hat;
            auto f_12 = -f_21;

            b_1->set_speed(b_1->get_speed() + f_12);
            b_2->set_speed(b_2->get_speed() + f_21);
        }
        m_bodies[body]->update(m_delta_t);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
void Game::render()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto& c_obj : m_bodies) {
        c_obj->render();
    }
    render_2d();
}
void Game::render_2d() {
    glDisable(GL_CULL_FACE);
    m_gui.mode.draw();
    if(m_paused){
        m_gui.paused.draw();
    }
    m_gui.game_version.draw();
#ifdef DEBUG
    if(m_gui.debug_menu.do_face_culling){
#endif
        glEnable(GL_CULL_FACE);

#ifdef DEBUG
    }
#endif
}
void Game::draw_gui()
{
    if (m_gui.spawn_menu_enabled) {
        ImGui::Begin("Spawn menu", &m_gui.spawn_menu_enabled);
        ImGui::ColorEdit3("Color", glm::value_ptr(m_gui.spawn_menu.color));
        ImGui::SliderFloat("Mass", &m_gui.spawn_menu.mass, 0.001, 100, NULL, 0);
        if (m_gui.spawn_menu.mass <= 0)
            m_gui.spawn_menu.mass = 0.001;
        ImGui::SliderFloat("Initial velocity", &m_gui.spawn_menu.initial_velocity, 0, 10, NULL, 0);
        ImGui::Checkbox("Star", &m_gui.spawn_menu.is_star);
        ImGui::End();
    }

    if (m_gui.game_options_menu_enabled) {
        ImGui::Begin("Game Options", &m_gui.game_options_menu_enabled, 0);
        if (ImGui::SliderFloat("Camera speed", &m_gui.game_options_menu.camera_speed, 0, 100, NULL, ImGuiSliderFlags_AlwaysClamp)) {
            m_camera.set_speed(m_gui.game_options_menu.camera_speed);
        }
        if (ImGui::SliderFloat("Camera FOV", &m_gui.game_options_menu.fov, 30, 120, NULL, ImGuiSliderFlags_AlwaysClamp)) {
            m_fov = m_gui.game_options_menu.fov;
            m_ubos.matrices.projection = glm::perspective(glm::radians(m_fov),
                (float)m_width / (float)m_height, 0.1f, 1000.0f);
        }
        ImGui::End();
    }
    if (m_gui.help_menu_enabled) {
        ImGui::Begin("Help", &m_gui.help_menu_enabled);
        ImGui::Text("Hello! Islands is a simple 3D gravity simulation.\n");
        ImGui::Text("%s", m_gui.help_menu.help_text.c_str());
        ImGui::End();
    }
    if (!m_gui.selected_body.expired()){
        bool discarded = true;
        ImGui::Begin("Selected Celestial Body", &discarded);
        if(ImGui::ColorEdit3("Object color", glm::value_ptr(m_gui.selected_body_menu.color))){
            m_gui.selected_body.lock()->set_color(m_gui.selected_body_menu.color);
        }
        if(ImGui::SliderFloat("Object mass", &m_gui.selected_body_menu.mass, 0.001, 1000)) {
            if (m_gui.selected_body_menu.mass <= 0)
                m_gui.selected_body_menu.mass = 0.001;
            m_gui.selected_body.lock()->set_mass(m_gui.selected_body_menu.mass);
        }
        if(ImGui::Button("Delete body")){
            auto body_as_star = dynamic_cast<obj::Star*>(m_gui.selected_body.lock().get());
            if(body_as_star){
                remove_star(body_as_star);
            } else {
                remove_planet(dynamic_cast<obj::Planet*>(m_gui.selected_body.lock().get()));
            }
            m_gui.selected_body.reset();
        }
        ImGui::End();
        if (!discarded){
            m_gui.selected_body = std::weak_ptr<obj::CelestialBody>();
        }
    }
#ifdef DEBUG
    if (m_gui.debug_menu_enabled) {
        ImGui::Begin("Debug menu", &m_gui.debug_menu_enabled);
        if(ImGui::Checkbox("Do face culling", &m_gui.debug_menu.do_face_culling)) {
            if(m_gui.debug_menu.do_face_culling){
                glEnable(GL_CULL_FACE);
            } else {
                glDisable(GL_CULL_FACE);
            }
        }
        if(ImGui::Checkbox("Do wireframe", &m_gui.debug_menu.do_wireframe)) {
            if(m_gui.debug_menu.do_wireframe){
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
        ImGui::End();
    }
#endif
}
void Game::add_planet(obj::Planet new_planet){
    auto planet = std::make_shared<obj::Planet>(new_planet);
    m_bodies.push_back(planet);
}
void Game::remove_planet(obj::Planet* planet){
    auto f = std::remove_if(m_bodies.begin(), m_bodies.end(), [&](auto ptr){
            return ptr.get() == planet;
    });
    m_bodies.erase(f);
}
void Game::add_star(obj::Star new_star){
    auto star = std::make_shared<obj::Star>(new_star);
    m_bodies.push_back(star);
    m_ssbos.light_sources.size++;
    if(m_ssbos.light_sources.size > m_ssbos.light_sources.cap){
        m_ssbos.light_sources.cap *= 2;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbos.light_sources.id);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                m_ssbos.light_sources.cap *
                    sizeof(LightSource),
                NULL,
                GL_DYNAMIC_DRAW);

    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbos.light_sources.id);
    auto offset = 0;
    std::for_each(m_bodies.begin(), m_bodies.end(), [&](auto b_ptr){
        if(auto star = dynamic_cast<obj::Star*>(b_ptr.get()); star)
            buffer_light_source(offset++, star);
    });
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
void Game::remove_star(obj::Star* star){
    auto f = std::remove_if(m_bodies.begin(), m_bodies.end(), [&](auto ptr){
            return ptr.get() == star;
    });
    if(f != m_bodies.end()){
        m_bodies.erase(f);
        m_ssbos.light_sources.size--;

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbos.light_sources.id);
        if(m_ssbos.light_sources.size < (m_ssbos.light_sources.cap / 2)){
            m_ssbos.light_sources.cap /= 2;
            glBufferData(GL_SHADER_STORAGE_BUFFER,
                    m_ssbos.light_sources.cap *
                        (sizeof(LightSource)),
                    NULL,
                    GL_DYNAMIC_DRAW);
        }

        glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R8, GL_RED, GL_UNSIGNED_BYTE, NULL);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }
}
void Game::key_handler(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)window;
    (void)scancode;
    m_keybinds.handle(key, action, m_gui_enabled ? BindMode::Editor : BindMode::Normal, mods);
    m_keybinds.handle(key, action, BindMode::Any, mods);
}

void Game::continuos_key_input()
{
    // Editor mode specifyc keybinds
    if (m_gui_enabled) {

    }
    // Simulation mode specific keybinds
    else {
        m_camera.keyboard_input(m_window_ptr, m_delta_t);
    }
}
void Game::framebuffer_size_handler(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
    m_width = width;
    m_height = height;

    m_ubos.matrices.text_projection = glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, 0.0f, 100.0f);

    m_ubos.matrices.projection = glm::perspective(glm::radians(m_fov),
        (float)m_width / (float)m_height, 0.1f, 1000.0f);

    glBindBuffer(GL_UNIFORM_BUFFER, m_ubos.matrices.id);
    //projection
    glBufferSubData(GL_UNIFORM_BUFFER,
        sizeof(MatricesUBO::view), sizeof(MatricesUBO::projection), glm::value_ptr(m_ubos.matrices.projection));

    //text_projection
    glBufferSubData(GL_UNIFORM_BUFFER,
        2 * sizeof(MatricesUBO::view), sizeof(MatricesUBO::text_projection), glm::value_ptr(m_ubos.matrices.text_projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    m_gui.paused.set_pos({ 0, m_height - m_gui.paused.get_text_height() });

    m_gui.game_version.set_pos({ m_width - m_gui.game_version.get_text_width(), m_height - m_gui.game_version.get_text_height() });
}
void Game::mouse_handler(GLFWwindow* window, double xpos, double ypos)
{
    (void)window;
    if (m_first_mouse) {
        m_last_mouse_x = xpos;
        m_last_mouse_y = ypos;
        m_first_mouse = false;
    }
    float x_offset = xpos - m_last_mouse_x;
    float y_offset = m_last_mouse_y - ypos;
    m_last_mouse_x = xpos;
    m_last_mouse_y = ypos;

    if (m_gui_enabled)
        return;
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

void Game::scroll_handler(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)window;
    (void)xoffset;
    m_camera.set_speed(std::max(m_camera.get_speed() + yoffset, 0.0));
}

void Game::mouse_button_handler(GLFWwindow* window, int button, int action, int mods) {
    (void)mods;
    double xpos = 0, ypos = 0;
    glfwGetCursorPos(window, &xpos, &ypos);
    if (m_gui_enabled && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        float x = (2.0f * xpos) / m_width - 1.0f;
        float y = 1.0f - (2.0f * ypos) / m_height;
        float z = 1.0f;
        glm::vec3 ray_nds = glm::vec3(x, y, z);
        glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
        glm::vec4 ray_eye = glm::inverse(m_ubos.matrices.projection) * ray_clip;
        ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
        glm::vec3 ray_world = (glm::inverse(m_ubos.matrices.view) * ray_eye);
        ray_world = glm::normalize(ray_world);
        glm::vec3 origin = m_camera.get_pos();
        std::optional<float> smallest_distance = std::nullopt;
        for (auto& obj : m_bodies) {
            glm::vec3 center = obj->get_pos();
            auto l = origin - center;
            auto b = 2 * glm::dot(ray_world, l);
            auto c = glm::dot(l, l) - std::pow(obj->get_radius(), 2);
            auto discr = (b * b) - 4 * c;
            std::optional<float> distance;
            if(discr < 0) continue;
            else if(discr > 0){
                distance = std::min(-b + std::sqrt(discr), -b - std::sqrt(discr)) / 2.0;
            } else {
                distance = -b / 2.0;
            }
            if (distance < smallest_distance.value_or(1000) && distance >= 0){
                smallest_distance = distance;
                m_gui.selected_body = obj;
                m_gui.selected_body_menu.mass = obj->get_mass();
                m_gui.selected_body_menu.color = obj->get_color();
            }
        }
    }
}
void Game::initialize_key_bindings() {
    m_keybinds.add_binding(GLFW_KEY_H, GLFW_PRESS, BindMode::Editor, [this](){
        this->m_gui.help_menu_enabled = !this->m_gui.help_menu_enabled;
    }, "Toggle this help menu");
    m_keybinds.add_binding(GLFW_KEY_ESCAPE, GLFW_PRESS, BindMode::Any, [this](){
        glfwSetWindowShouldClose(this->m_window_ptr, true);
    }, "Exit the simulation");
    // // toggle the gui with [E]dit
    m_keybinds.add_binding(GLFW_KEY_E, GLFW_PRESS, BindMode::Any, [this](){
        this->m_gui_enabled = !this->m_gui_enabled;
        if (this->m_gui_enabled){
            glfwSetInputMode(this->m_window_ptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            m_gui.mode.set_text("Edit");
            m_gui.mode.set_color(gui::GameUI::EDIT_MODE_TEXT_COLOR);
        } else {
            glfwSetInputMode(this->m_window_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            m_gui.mode.set_text("Normal");
            m_gui.mode.set_color(gui::GameUI::NORMAL_MODE_TEXT_COLOR);
        }
    }, "Toggle edit mode");
    // // [P]ause the game
    m_keybinds.add_binding(GLFW_KEY_P, GLFW_PRESS, BindMode::Any, [this](){
        this->m_paused = !this->m_paused;
    }, "Pause the simulation");
    // // Shift + S -> spawn object
    m_keybinds.add_binding(GLFW_KEY_S, GLFW_PRESS, BindMode::Any, [this](){
        auto front = this->m_camera.get_front();
        auto pos = this->m_camera.get_pos() + front;
        auto vel = glm::normalize(front) * this->m_gui.spawn_menu.initial_velocity;
        auto color = this->m_gui.spawn_menu.color;
        auto mass = this->m_gui.spawn_menu.mass;
        // auto obj = std::make_shared<obj::Planet>(obj::Planet(nullptr, pos, vel, glm::vec3(0), this->m_gui.spawn_menu.mass));
        // obj->set_color(this->m_gui.spawn_menu.color);
        if(m_gui.spawn_menu.is_star){
            auto star = obj::Star(nullptr, pos, vel, {}, mass);
            star.set_color(color);
            add_star(star);
        } else {
            auto planet = obj::Planet(nullptr, pos, vel, {}, mass);
            planet.set_color(color);
            add_planet(planet);
        }
    }, "Spawn a new celestial body in front of the camera", GLFW_MOD_SHIFT);
    //debug menu
#ifdef DEBUG
    m_keybinds.add_binding(GLFW_KEY_D, GLFW_PRESS, BindMode::Any, [this](){
        this->m_gui.debug_menu_enabled = !this->m_gui.debug_menu_enabled;
    }, "Open the debug menu", GLFW_MOD_SHIFT);
#endif
    // Editor mode specifyc keybinds
    m_keybinds.add_binding(GLFW_KEY_O, GLFW_PRESS, BindMode::Editor, [this](){
        this->m_gui.game_options_menu_enabled = !this->m_gui.game_options_menu_enabled;
    }, "Open game settings");
    m_keybinds.add_binding(GLFW_KEY_S, GLFW_PRESS, BindMode::Editor, [this](){
        this->m_gui.spawn_menu_enabled = !this->m_gui.spawn_menu_enabled;
    }, "Open spawn menu");
    m_keybinds.add_binding(GLFW_KEY_D, GLFW_PRESS, BindMode::Editor, [this](){
        this->m_gui.selected_body = std::weak_ptr<obj::CelestialBody>();
    }, "Deselect currently selected body");
    // Simulation mode specific keybinds
}
