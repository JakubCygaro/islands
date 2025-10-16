#include "Font.hpp"
#include "Object.hpp"
#include "Singletons.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <Game.hpp>
#include <algorithm>
#include <array>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <files.hpp>
#include <filesystem>
#include <functional>
#include <glm/common.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <limits>
#include <numeric>
#include <optional>
#include <thread>
#include <tuple>
#define GLM_ENABLE_EXPERIMENTAL
#include "global_declarations.hpp"
#include <glm/gtx/string_cast.hpp>
#include <memory>

namespace gm{

    namespace {
    const float GRAV_CONST = 6.674e-11;
    const float PROJECTION_FAR_PLANE = 500.0f;
    Game* get_game_instance_ptr_from_window(GLFWwindow* window)
    {
        Game* instance = static_cast<Game*>(glfwGetWindowUserPointer(window));
        return instance;
    }
    // for path prediction
    struct Gravdata {
        float mass {};
        glm::vec3 pos {};
        glm::vec3 vel {};
        glm::vec3 acc {};
        float radius {};
        bool is_star {};
    };
    // step through one frame of gravity simulation
    void gravity_step(std::vector<Gravdata>& gravdata, double delta_t, bool do_collision, const gui::CancelationToken& cancel)
    {
        for (size_t body = 0; body < gravdata.size() && !cancel.is_cancelled(); body++) {
            for (size_t next_body = body + 1; next_body < gravdata.size() && !cancel.is_cancelled(); next_body++) {
                auto& b_1 = gravdata[body];
                auto& b_2 = gravdata[next_body];

                if (do_collision && glm::distance(b_1.pos, b_2.pos) <= b_1.radius + b_2.radius) {
                    auto [eater, eaten] = b_1.mass > b_2.mass ? std::make_tuple(std::ref(b_1), std::ref(b_2)) : std::make_tuple(std::ref(b_2), std::ref(b_1));
                    if (eaten.is_star) {
                        std::swap(eater, eaten);
                    }
                    eaten.mass = 0.0;
                    eaten.radius = 0.0;
                    eaten.vel = glm::vec3 { 0.0 };
                }
                // https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form
                auto m_1 = b_1.mass * obj::CelestialBody::MASS_BOOST_FACTOR;
                auto m_2 = b_2.mass * obj::CelestialBody::MASS_BOOST_FACTOR;
                ;

                auto r_21 = b_2.pos - b_1.pos;
                auto r_21_hat = glm::normalize(r_21);
                auto distance = glm::distance(b_1.pos, b_2.pos);
                // attraction force
                auto f_21 = -GRAV_CONST * ((m_1 * m_2) / (distance * distance)) * r_21_hat;
                auto f_12 = -f_21;

                b_1.acc += f_12;
                b_2.acc += f_21;
            }
        }
        for (auto& b : gravdata) {
            if (cancel.is_cancelled())
                break;
            b.vel += b.acc;
            b.acc = glm::vec3(0);
            auto tmp_speed = b.vel;
            tmp_speed *= delta_t;
            b.pos += tmp_speed;
        }
    }
    }

    void Game::collect_light_sources()
    {
        auto offset = 0;
        if (m_ssbos.light_sources.size != m_light_data.size()) {
            m_light_data.resize(m_ssbos.light_sources.size);
        }
        // std::vector<LightSource> ls(m_ssbos.light_sources.size);
        std::for_each(m_bodies.begin(), m_bodies.end(), [&](auto b_ptr) {
            if (auto star = dynamic_cast<obj::Star*>(b_ptr.get()); star) {
                m_light_data[offset++] = {
                    .position = star->get_pos(),
                    .color = star->get_color(),
                    .att_linear = star->get_attenuation_linear(),
                    .att_quadratic = star->get_attenuation_quadratic(),
                    .radius = star->get_light_source_radius(),
                };
            }
        });
        // return ls;
    }

    void Game::buffer_light_data()
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbos.light_sources.id);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
            m_light_data.size() * sizeof(LightSource),
            m_light_data.data(),
            GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    }

    void Game::initialize()
    {
        glfwInit();
        GLFWmonitor* primary = glfwGetPrimaryMonitor();
        if(!primary){
            throw std::runtime_error("no monitors found");
        }
        const GLFWvidmode* video_mode = glfwGetVideoMode(primary);
        int w{}, h{};
        w = video_mode->width * 0.80;
        h = video_mode->height * 0.80;

        auto& resolutions = gui::GameOptionsMenu::resolutions;
        size_t idx{};
        size_t min_diff = std::numeric_limits<size_t>::max();
        for(size_t i = 0; i < resolutions.size(); i++){
            auto& res = resolutions[i];
            auto diff = static_cast<size_t>(std::abs(w - res.width) + std::abs(h - res.height));
            if(diff < min_diff){
                min_diff = diff;
                idx = i;
            }
        }
        m_width = resolutions[idx].width;
        m_height = resolutions[idx].height;
        m_gui.game_options_menu.current = idx;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);

        m_window_ptr = glfwCreateWindow(m_width, m_height, "Islands", NULL, NULL);
        if (m_window_ptr == NULL) {
            throw std::runtime_error("Failed to create the window");
            glfwTerminate();
        }
        glfwMakeContextCurrent(m_window_ptr);
        glfwSetWindowUserPointer(m_window_ptr, this);
        glfwSetWindowSizeLimits(m_window_ptr, m_width, m_height, m_width, m_height);
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
        auto window_maximize_callback = [](GLFWwindow* window, int maximized) {
            Game* instance = get_game_instance_ptr_from_window(window);
            instance->window_maximize_handler(window, maximized);
        };
        glfwSetWindowMaximizeCallback(m_window_ptr, window_maximize_callback);

        auto window_refresh_callback = [](GLFWwindow* window) {
            Game* instance = get_game_instance_ptr_from_window(window);
            instance->window_refresh_handler(window);
        };
        glfwSetWindowRefreshCallback(m_window_ptr, window_refresh_callback);


        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize GLAD");
        }
        glfwGetFramebufferSize(m_window_ptr, &m_width, &m_height);

        glViewport(0, 0, m_width, m_height);

        initialize_singletons();
        initialize_uniforms();
        m_gbuffer = Gbuffer { this->m_width, this->m_height };

        m_grid = std::make_unique<Grid>(Grid(9));

        m_grid->set_scale(m_gui.game_options_menu.grid_scale);
        m_grid->set_color(m_gui.game_options_menu.grid_color);

        const std::array<std::string, 6> cube_map = {
            files::game_data::textures::background::BKG1_RIGHT_PNG,
            files::game_data::textures::background::BKG1_LEFT_PNG,
            files::game_data::textures::background::BKG1_BOT_PNG,
            files::game_data::textures::background::BKG1_TOP_PNG,
            files::game_data::textures::background::BKG1_FRONT_PNG,
            files::game_data::textures::background::BKG1_BACK_PNG,
        };

        m_skybox = std::make_unique<Skybox>(cube_map);

        auto c_body = obj::Planet(nullptr, { 2.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, 100);
        c_body.set_color({ 1.0, .1, .1 });
        c_body.set_rotation_speed(100.0);
        add_planet(c_body);

        c_body = obj::Planet(nullptr, { -10.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, 10);
        c_body.set_color({ 0.0, 1.0, .1 });
        c_body.set_rotation_speed(40.0);
        add_planet(c_body);

        c_body = obj::Planet(nullptr, { 0.0f, 0.0f, 10.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, 25);
        c_body.set_color({ 0.0, 0.0, 1.0 });
        c_body.set_rotation_speed(-40.0);
        add_planet(c_body);

        auto star = obj::Star(nullptr, { 0, 5, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 5);
        star.set_color({ 0.78, 0.52, 0.06 });
        star.set_rotation_speed(5.0);
        star.set_axial_tilt(0);
        add_star(std::move(star));

        star = obj::Star(nullptr, { 0, -10, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, 5);
        star.set_color({ 0.78, 0.52, 0.06 });
        star.set_rotation_speed(-30.0);
        star.set_axial_tilt(20);
        add_star(std::move(star));

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui_ImplGlfw_InitForOpenGL(m_window_ptr, true);
        ImGui_ImplOpenGL3_Init("#version 460");
        m_gui.game_options_menu.camera_speed = m_camera.get_speed();
        m_gui.game_options_menu.fov = m_fov;
        m_gui.help_menu_enabled = true;

        m_gui.mode = font::Text2D("Edit");
        m_gui.mode.set_pos({ 0, 0 });
        m_gui.mode.set_color(gui::GameUI::EDIT_MODE_TEXT_COLOR);

        m_gui.paused = font::Text2D("PAUSED");
        m_gui.paused.set_color({ 1.0, .0, .0 });
        m_gui.paused.set_pos({ 0, m_height - m_gui.paused.get_text_height() });

        m_gui.game_version = font::Text2D(global_decl::GAME_VERSION);
        m_gui.game_version.set_color({ 1.0, 1.0, 1.0 });
        m_gui.game_version.set_scale(.5f);
        m_gui.game_version.set_pos({ m_width - m_gui.game_version.get_text_width(), m_height - m_gui.game_version.get_text_height() });

        m_gui.fps_count = font::Text2D("0");
        m_gui.fps_count.set_color({ 1.0, 1.0, 1.0 });
        m_gui.fps_count.set_scale(.5f);
        m_gui.fps_count.set_pos({ m_width - m_gui.game_version.get_text_width(), m_gui.game_version.get_text_height() });

        m_gui.selected_body_menu.trajectory_trail = obj::Trail(1024);
        m_gui.selected_body_menu.trajectory_data.resize(m_gui.selected_body_menu.trajectory_trail.size());
        m_gui.selected_body_menu.trajectory_color = { 0.1, 0.6, 0.4, 0.5 };
        m_gui.selected_body_menu.trajectory_trail.set_color(m_gui.selected_body_menu.trajectory_color);

        load_custom_textures_paths();
    }
    void Game::initialize_uniforms()
    {
        m_ubos.matrices.projection = glm::perspective(glm::radians(m_fov),
            (float)m_width / (float)m_height, 0.1f, PROJECTION_FAR_PLANE);

        m_ubos.matrices.text_projection = glm::ortho(
            0.0f,
            static_cast<float>(m_width),
            static_cast<float>(m_height),
            0.0f,
            0.0f,
            100.0f);

        glGenBuffers(1, &m_ubos.matrices.id);
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubos.matrices.id);
        glBufferData(GL_UNIFORM_BUFFER,
            sizeof(MatricesUBO),
            NULL, GL_STATIC_DRAW);

        // buffer projection
        glBufferSubData(GL_UNIFORM_BUFFER,
            sizeof(MatricesUBO::view), sizeof(MatricesUBO::projection), glm::value_ptr(m_ubos.matrices.projection));

        // buffer text_projection
        glBufferSubData(GL_UNIFORM_BUFFER,
            2 * sizeof(MatricesUBO::view), sizeof(MatricesUBO::text_projection), glm::value_ptr(m_ubos.matrices.text_projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, m_ubos.matrices.mount_point, m_ubos.matrices.id);

        // lighting_globals uniform buffer
        m_ubos.lighting_globals.ambient_strength = 0.010f;
        glGenBuffers(1, &m_ubos.lighting_globals.id);
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubos.lighting_globals.id);
        glBufferData(GL_UNIFORM_BUFFER,
            sizeof(LightingGlobalsUBO),
            &m_ubos.lighting_globals, GL_STATIC_DRAW);

        glBufferSubData(GL_UNIFORM_BUFFER,
            sizeof(LightingGlobalsUBO::__ambient_s_pad),
            sizeof(LightingGlobalsUBO::camera_pos),
            m_camera.get_pos_ptr());
    }
    void Game::initialize_singletons(){
        singl::shader_instances::load_all();
        singl::buffer_instances::load_all();
        singl::font_instances::load_default_font();
    }
    void Game::run()
    {
        while (!glfwWindowShouldClose(m_window_ptr)) {
            m_current_frame_t = glfwGetTime();
            m_fps++;
            m_delta_t = m_current_frame_t - m_last_frame_t;

            if (m_delta_t_record.size() >= DELTA_T_MEAN_SAMPLE_SIZE) {
                m_delta_t_record.pop_front();
            }
            m_delta_t_record.push_back(m_delta_t);
            // a fixed update happens every 0.2 second
            if (m_current_frame_t - m_last_fixed_update_t >= 0.2) {
                m_last_fixed_update_t = m_current_frame_t;
                m_fixed_update = true;
            }
            if (m_current_frame_t - m_last_fps_update_t >= 1.0) {
                m_last_fps_update_t = m_current_frame_t;
                m_gui.fps_count.set_text(std::to_string(m_fps));
                m_fps = 0;
            }
            m_last_frame_t = m_current_frame_t;

            update_buffers();
            update();
            render();
            glDisable(GL_FRAMEBUFFER_SRGB);
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glEnable(GL_FRAMEBUFFER_SRGB);
            glfwSwapBuffers(m_window_ptr);
            m_fixed_update = false;
        }
    }
    void Game::update()
    {
        auto selected_pos_before_update = glm::vec3(0);
        auto selected_pos_after_update = glm::vec3(0);
        // in case the body was selected during the update loop, so the initial camera offset is not equal to the bodies position
        auto had_selected = !m_gui.selected_body.expired();
        if (had_selected && m_gui.selected_body_menu.track) {
            auto selected = m_gui.selected_body.lock();
            selected_pos_before_update = selected->get_pos();
        }

        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // this has to happen before the while loop
        if (m_gui_enabled) {
            draw_gui();
        }
        while (!m_key_events.empty()) {
            auto [key, sc, act, mds] = m_key_events.front();
            if (!m_typing) {
                m_keybinds.handle(key, act, m_gui_enabled ? BindMode::Editor : BindMode::Normal, mds);
                m_keybinds.handle(key, act, BindMode::Any, mds);
            }
            m_key_events.pop();
        }
        if (m_typing) {
            m_typing = false;
        }
        continuos_key_input();

        if (!m_paused) {
            update_bodies();
        }
        if (m_maximize != MaximizeState::DoNothing) {
            glfwSetWindowSizeLimits(m_window_ptr, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE);
            glfwSetWindowAttrib(m_window_ptr, GLFW_RESIZABLE, GLFW_TRUE);
        }
        switch (m_maximize) {
        case MaximizeState::Maximize:
            glfwSetWindowSizeLimits(m_window_ptr, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE, GLFW_DONT_CARE);
            glfwMaximizeWindow(m_window_ptr);
            break;
        case MaximizeState::Minimize: {
            glfwRestoreWindow(m_window_ptr);
            glfwSetWindowAttrib(m_window_ptr, GLFW_DECORATED, GLFW_TRUE);
            auto resolution = m_gui.game_options_menu.resolutions[m_gui.game_options_menu.current];
            glfwSetWindowSize(m_window_ptr, resolution.width, resolution.height);
            glfwSetWindowSizeLimits(m_window_ptr, m_width, m_height, m_width, m_height);
        } break;
        case MaximizeState::DoNothing:
            break;
        }

        auto status = m_gui.selected_body_menu.trajectory_status.load();
        if (status == gui::TrailCompStatus::Finished) {
            m_gui.selected_body_menu.trajectory_trail.copy_from_vector(m_gui.selected_body_menu.trajectory_data);
            m_gui.selected_body_menu.trajectory_status.store(gui::TrailCompStatus::Idle);
        }
        if (status == gui::TrailCompStatus::Terminated) {
            schedule_selected_body_trajectory_calc();
        }

        if (had_selected && !m_gui.selected_body.expired() && m_gui.selected_body_menu.track) {
            auto selected = m_gui.selected_body.lock();
            selected_pos_after_update = selected->get_pos() - selected_pos_before_update;
            m_camera.set_pos(m_camera.get_pos() + selected_pos_after_update);
        }
    }
    void Game::update_buffers()
    {

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
    void Game::update_bodies()
    {
        std::unordered_map<std::shared_ptr<obj::CelestialBody>, size_t> to_delete {};
        // std::vector<std::shared_ptr<obj::CelestialBody>> to_delete{};
        auto offset = 0;
        for (size_t body = 0; body < m_bodies.size(); body++) {
            // if this celestial body is a star, collect its light data
            if (auto star = dynamic_cast<obj::Star*>(m_bodies[body].get()); star) {
                m_light_data[offset++] = {
                    .position = star->get_pos(),
                    .color = star->get_color(),
                    .att_linear = star->get_attenuation_linear(),
                    .att_quadratic = star->get_attenuation_quadratic(),
                    .radius = star->get_light_source_radius(),
                };
            }
            for (size_t next_body = body + 1; next_body < m_bodies.size(); next_body++) {
                auto b_1 = m_bodies[body];
                auto b_2 = m_bodies[next_body];

                auto& do_collision = m_gui.game_options_menu.do_collision;
                // check if collision first
                if (do_collision && glm::distance(b_1->get_pos(), b_2->get_pos()) <= b_1->get_radius() + b_2->get_radius()) {
                    auto [eater, eaten] = b_1->get_mass() > b_2->get_mass() ? std::make_tuple(b_1, b_2) : std::make_tuple(b_2, b_1);
                    if (dynamic_cast<obj::Star*>(eaten.get())) {
                        std::swap(eater, eaten);
                    }
                    if (to_delete.find(eaten) == to_delete.end()) {
                        eater->set_mass(eater->get_mass() + eaten->get_mass());
                        to_delete.insert({ eaten, next_body });
                        break;
                    }
                }
                // https://en.wikipedia.org/wiki/Newton%27s_law_of_universal_gravitation#Vector_form
                auto m_1 = b_1->get_mass() * obj::CelestialBody::MASS_BOOST_FACTOR;
                auto m_2 = b_2->get_mass() * obj::CelestialBody::MASS_BOOST_FACTOR;
                ;

                auto r_21 = b_2->get_pos() - b_1->get_pos();
                auto r_21_hat = glm::normalize(r_21);
                auto distance = glm::distance(b_1->get_pos(), b_2->get_pos());
                // attraction force
                auto f_21 = -GRAV_CONST * ((m_1 * m_2) / (distance * distance)) * r_21_hat;
                auto f_12 = -f_21;

                b_1->set_acceleration(b_1->get_acceleration() + f_12);
                b_2->set_acceleration(b_2->get_acceleration() + f_21);
            }
            m_bodies[body]->update(m_delta_t);
            if (m_fixed_update)
                m_bodies[body]->fixed_update();
        }
        for (auto [ptr, idx] : to_delete) {
            remove_body(ptr.get());
        }
        buffer_light_data();
    }
    void Game::remove_body(obj::CelestialBody* body)
    {
        if (auto star = dynamic_cast<obj::Star*>(body); star) {
            remove_star(star);
        } else {
            remove_planet(dynamic_cast<obj::Planet*>(body));
        }
    }
    void Game::render_gbuffer()
    {
        glClearColor(0, 0, 0, 0);
        m_gbuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_BLEND);
        glDisable(GL_FRAMEBUFFER_SRGB);
        // glDisable(GL_CULL_FACE);
        std::size_t i = 0;
        for (auto& c_obj : m_bodies) {
            if (auto planet = dynamic_cast<obj::Planet*>(c_obj.get()); planet) {
                planet->deferred_render();
            }
            if (auto star = dynamic_cast<obj::Star*>(c_obj.get()); star) {
                m_gbuffer.unbind();
                glBindFramebuffer(GL_FRAMEBUFFER, star->get_shadow_map_fbo());
                auto [w, h] = obj::Star::get_shadow_map_size();
                glViewport(0, 0, w, h);
                glClear(GL_DEPTH_BUFFER_BIT);
                glCullFace(GL_FRONT);

                star->load_shadow_transforms_uniform();
                for (auto& obj : m_bodies) {
                    if (obj.get() == star)
                        continue;
                    obj->shadow_render();
                }
                glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
                m_gbuffer.bind();
                m_light_data[i++].shadow_map_id = star->get_shadow_map_id();
                glCullFace(GL_BACK);
            }
        }
        m_gbuffer.unbind();
    }
    void Game::render_light_volumes()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);
        glCullFace(GL_FRONT);
        auto* uv = singl::buffer_instances::get_instance<obj::UnitSphereVAO>(singl::buffer_instances::BufferInstance::UnitSphere);
        using namespace singl;
        auto lv_shader = shader_instances::get_instance(shader_instances::ShaderInstance::LightPassSphere);
        lv_shader->use_shader();
        lv_shader->set_int("g_position", 0);
        lv_shader->set_int("g_normal", 1);
        lv_shader->set_int("g_albedo_spec", 2);
        lv_shader->set_float("far_plane", obj::Star::s_shadow_far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_gbuffer.g_position);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_gbuffer.g_normal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_gbuffer.g_color_spec);
        for (auto& ls : m_light_data) {
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_CUBE_MAP, ls.shadow_map_id);
            lv_shader->set_int("shadow_map", 3);
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, ls.position);
            model = glm::scale(model, glm::vec3(ls.radius));
            lv_shader->set_mat4("model", model);
            lv_shader->set_vec3("color", ls.color);
            lv_shader->set_float("att_linear", ls.att_linear);
            lv_shader->set_float("att_quadratic", ls.att_quadratic);
            lv_shader->set_vec3("light_source_pos", ls.position);
            uv->draw();
        }
        glDisable(GL_CULL_FACE);
        glBlendFunc(GL_ONE, GL_ZERO);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        // blit the depth buffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gbuffer.fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(
            0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_FRAMEBUFFER_SRGB);
    }
    void Game::render()
    {
        const bool normals_draw =
#ifdef DEBUG
            m_gui.debug_menu.draw_normals;
#else
            false;
#endif
        const bool wireframe_draw =
#ifdef DEBUG
            m_gui.debug_menu.draw_wireframe;
#else
            false;
#endif
        if (!wireframe_draw)
            render_gbuffer();

        if (!wireframe_draw)
            render_light_volumes();
        else {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if(m_gui.game_options_menu.draw_skybox)
            m_skybox->forward_render();
        for (auto& c_obj : m_bodies) {
            c_obj->forward_render(normals_draw, wireframe_draw, m_gui.game_options_menu.draw_trails);
        }
        if (m_gui.game_options_menu.draw_grid) {
            m_grid->forward_render(m_camera.get_pos());
        }
        if (m_paused && m_gui.selected_body_menu.trajectory_status.load() == gui::TrailCompStatus::Idle && !m_gui.selected_body.expired()) {
            m_gui.selected_body_menu.trajectory_trail.forward_render();
        }
        if (!m_gui.selected_body.expired() && m_gui.game_options_menu.draw_selection_marker) {
            auto slc = m_gui.selected_body.lock();
            obj::SelectedMarker::instance().forward_render(m_camera.get_pos(), slc->get_pos(), slc->get_radius());
        }
        if (m_gui.game_options_menu.draw_labels) {
            for (auto& obj : m_bodies) {
                auto& lab = obj->label();
                lab.draw();
            }
        }
        glDisable(GL_BLEND);

        render_2d();
    }
    void Game::render_2d()
    {
        glDisable(GL_CULL_FACE);
#ifdef DEBUG
        if (m_gui.debug_menu.draw_wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
#endif
        m_gui.mode.draw();
        if (m_paused) {
            m_gui.paused.draw();
        }
        m_gui.game_version.draw();
        m_gui.fps_count.draw();
#ifdef DEBUG
        if (m_gui.debug_menu.draw_wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        if (m_gui.debug_menu.do_face_culling) {
#endif
            glEnable(GL_CULL_FACE);
#ifdef DEBUG
        }
#endif
    }
    void Game::draw_gui()
    {
        if (m_gui.spawn_menu_enabled)
            draw_spawn_menu_gui();
        if (m_gui.game_options_menu_enabled)
            draw_game_options_gui();
        if (m_gui.help_menu_enabled)
            draw_help_menu_gui();
        if (m_gui.bodies_list_enabled)
            draw_body_list_gui();
        if (!m_gui.selected_body.expired())
            draw_selected_body_gui();
        if (m_gui.texture_menu_enabled)
            draw_texture_menu_gui();
#ifdef DEBUG
        if (m_gui.debug_menu_enabled)
            draw_debug_menu_gui();
#endif
    }
    static glm::vec4 get_current_imgui_window_rect()
    {
        auto pos = ImGui::GetWindowPos();
        auto size = ImGui::GetWindowSize();
        return glm::vec4(pos.x, pos.y, size.x, size.y);
    }
    void Game::draw_game_options_gui()
    {
        ImGui::Begin("Game Options", &m_gui.game_options_menu_enabled, 0);
        m_imgui_window_rects.push(get_current_imgui_window_rect());
        if (ImGui::SliderFloat("Camera speed", &m_gui.game_options_menu.camera_speed, 0, 100, NULL, ImGuiSliderFlags_AlwaysClamp)) {
            m_camera.set_speed(m_gui.game_options_menu.camera_speed);
        }
        if (ImGui::SliderFloat("Camera FOV", &m_gui.game_options_menu.fov, 30, 120, NULL, ImGuiSliderFlags_AlwaysClamp)) {
            m_fov = m_gui.game_options_menu.fov;
            m_ubos.matrices.projection = glm::perspective(glm::radians(m_fov),
                (float)m_width / (float)m_height, 0.1f, PROJECTION_FAR_PLANE);
        }
        if (ImGui::Combo("Window size",
                &m_gui.game_options_menu.current,
                &m_gui.game_options_menu.get_resolution,
                nullptr,
                m_gui.game_options_menu.resolutions.size())) {
            auto& selected = m_gui.game_options_menu.resolutions[m_gui.game_options_menu.current];
            glfwSetWindowSize(m_window_ptr, selected.width, selected.height);
        }
        ImGui::Checkbox("Draw grid", &m_gui.game_options_menu.draw_grid);
        ImGui::Checkbox("Draw trails", &m_gui.game_options_menu.draw_trails);
        ImGui::Checkbox("Draw selection marker", &m_gui.game_options_menu.draw_selection_marker);
        ImGui::Checkbox("Draw labels", &m_gui.game_options_menu.draw_labels);
        ImGui::Checkbox("Draw skybox", &m_gui.game_options_menu.draw_skybox);
        ImGui::Checkbox("Enable collisions", &m_gui.game_options_menu.do_collision);
        if (ImGui::SliderFloat("Grid scale", &m_gui.game_options_menu.grid_scale, 1.0, 50.0)) {
            m_grid->set_scale(m_gui.game_options_menu.grid_scale);
        };
        if (ImGui::SliderFloat4("Grid color", glm::value_ptr(m_gui.game_options_menu.grid_color), 0.0, 1.0)) {
            m_grid->set_color(m_gui.game_options_menu.grid_color);
        }
        if (ImGui::ColorEdit3("Predicted trajectory color", glm::value_ptr(m_gui.selected_body_menu.trajectory_color))) {
            m_gui.selected_body_menu.trajectory_trail.set_color(m_gui.selected_body_menu.trajectory_color);
        }
        ImGui::End();
    }
    static bool validate_new_name(const std::string& new_name)
    {
        auto res = !std::any_of(new_name.begin(), new_name.end(), [](char c) {
            return !std::isalpha(c) || std::isspace(c);
        });
        res &= new_name.length() > 0;
        return res;
    }
    void Game::draw_spawn_menu_gui()
    {
        ImGui::Begin("Spawn menu", &m_gui.spawn_menu_enabled);
        m_imgui_window_rects.push(get_current_imgui_window_rect());
        ImGui::ColorEdit3("Color", glm::value_ptr(m_gui.spawn_menu.color));
        ImGui::SliderFloat("Mass", &m_gui.spawn_menu.mass, 0.001, 100, NULL, 0);
        if (m_gui.spawn_menu.mass <= 0)
            m_gui.spawn_menu.mass = 0.001;
        ImGui::SliderFloat("Initial velocity", &m_gui.spawn_menu.initial_velocity, 0, 10, NULL, 0);
        static bool show_incorrect_msg = false;
        m_typing |= ImGui::InputText("Name", m_gui.spawn_menu.name, IM_ARRAYSIZE(m_gui.spawn_menu.name));
        show_incorrect_msg &= !m_typing;

        auto new_name = std::string(m_gui.spawn_menu.name);
        if (!validate_new_name(new_name)) {
            show_incorrect_msg = true;
        }
        if (show_incorrect_msg) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(.8, .1, .0, 1.), "Invalid name");
        }
        ImGui::Checkbox("Is star", &m_gui.spawn_menu.is_star);
        ImGui::End();
    }
    void Game::draw_help_menu_gui()
    {
        ImGui::Begin("Help", &m_gui.help_menu_enabled);
        m_imgui_window_rects.push(get_current_imgui_window_rect());
        ImGui::Text("Hello! Islands is a simple 3D gravity simulation.\n");
        ImGui::Text("%s", m_gui.help_menu.help_text.c_str());
        ImGui::End();
    }
    void Game::draw_texture_menu_gui(){
        static std::unique_ptr<std::string> error_message = nullptr;

        ImGui::Begin("Textures", &m_gui.texture_menu_enabled);
        m_imgui_window_rects.push(get_current_imgui_window_rect());
        ImGui::BeginListBox("Loaded:");
        for (auto it = m_loaded_textures.begin(); it != m_loaded_textures.end(); ++it) {
            auto& [path, texture] = *it;
            ImGui::Text("%s", path.filename().generic_string().c_str());
            ImGui::PushID(path.c_str());
            ImGui::SameLine();
            if(ImGui::Button("Reload")){
                load_texture_from_path(path);
            }
            ImGui::PopID();
        }
        ImGui::EndListBox();
        std::optional<std::filesystem::path> to_load = std::nullopt;

        ImGui::BeginListBox("Detected but not loaded");
        for (auto it = m_unloaded_textures.begin(); it != m_unloaded_textures.end(); ++it) {
            const auto& path = *it;
            ImGui::PushID(path.c_str());
            ImGui::Text("%s", path.filename().generic_string().c_str());
            ImGui::SameLine();
            if(ImGui::Button("Load")){
                to_load = path;
            }
            ImGui::PopID();
        }
        ImGui::EndListBox();
        if(to_load){
            auto path = to_load.value();
            try{
                load_texture_from_path(path);
                (void)m_unloaded_textures.extract(path);
                to_load = std::nullopt;
            } catch (const std::runtime_error& err){
                error_message = std::make_unique<std::string>(err.what());
            }
        }
        if(ImGui::Button("Refresh")){
            load_custom_textures_paths();
        }
        if(ImGui::IsItemHovered()){
            ImGui::SetItemTooltip("Refresh the list of available textures from the game_data/textures/custom directory");
        }
        if(error_message){
            ImGui::TextColored(ImVec4(.8, .2, .0, 1.0), "Error while trying to load a texture: %s", error_message->c_str());
        }
        ImGui::End();
    }
#ifdef DEBUG
    void Game::draw_debug_menu_gui()
    {
        ImGui::Begin("Debug menu", &m_gui.debug_menu_enabled);
        m_imgui_window_rects.push(get_current_imgui_window_rect());
        if (ImGui::Checkbox("Do face culling", &m_gui.debug_menu.do_face_culling)) {
            if (m_gui.debug_menu.do_face_culling) {
                glEnable(GL_CULL_FACE);
            } else {
                glDisable(GL_CULL_FACE);
            }
        }
        if (ImGui::Checkbox("Do wireframe", &m_gui.debug_menu.draw_wireframe)) {
            if (m_gui.debug_menu.draw_wireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
        if (ImGui::Checkbox("Draw normals", &m_gui.debug_menu.draw_normals)) { }
        ImGui::End();
    }
#endif
    void Game::draw_selected_body_gui()
    {
        bool discarded = true;
        auto star = dynamic_cast<obj::Star*>(m_gui.selected_body.lock().get());
        ImGui::Begin("Selected Celestial Body", &discarded);
        m_imgui_window_rects.push(get_current_imgui_window_rect());
        auto slc = m_gui.selected_body.lock();
        ImGui::Text("%s", slc->get_name().c_str());

        static bool show_incorrect_msg = false;
        m_typing |= ImGui::InputText("Name: ", m_gui.selected_body_menu.name, IM_ARRAYSIZE(m_gui.selected_body_menu.name));
        show_incorrect_msg &= !m_typing;

        if (ImGui::Button("Save name change")) {
            auto new_name = std::string(m_gui.selected_body_menu.name);
            if (validate_new_name(new_name)) {
                slc->set_name(std::move(new_name));
                std::copy(slc->get_name().c_str(), slc->get_name().c_str() + sizeof(m_gui.selected_body_menu.name),
                    m_gui.selected_body_menu.name);
            } else {
                show_incorrect_msg = true;
                std::copy(slc->get_name().begin(), slc->get_name().end(), m_gui.selected_body_menu.name);
            }
        }
        if (show_incorrect_msg) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(.8, .1, .0, 1.), "Invalid name");
        }
        if(ImGui::BeginCombo("Texture", m_gui.selected_body_menu.texture_name.c_str())){
            if(ImGui::Selectable("none")){
                slc->set_texture(nullptr);
                m_gui.selected_body_menu.texture_name = "";
            }
            for(auto& [path, texture] : m_loaded_textures){
                if(ImGui::Selectable(path.filename().generic_string().c_str())){
                    slc->set_texture(texture);
                    m_gui.selected_body_menu.texture_name = path.filename().generic_string();
                }
            }
            ImGui::EndCombo();
        }

        if (ImGui::ColorEdit3("Object color", glm::value_ptr(m_gui.selected_body_menu.color))) {
            slc->set_color(m_gui.selected_body_menu.color);
            if (star) {
                collect_light_sources();
            }
        }
        if (ImGui::SliderFloat("Object mass", &m_gui.selected_body_menu.mass, 0.001, 1000)) {
            if (m_gui.selected_body_menu.mass <= 0)
                m_gui.selected_body_menu.mass = 0.001;
            slc->set_mass(m_gui.selected_body_menu.mass);
            schedule_selected_body_trajectory_calc();
            if (star) {
                collect_light_sources();
            }
        }
        auto slc_vel = slc->get_speed();
        ImGui::Text("Object velocity vector: %f, %f, %f", slc_vel.x, slc_vel.y, slc_vel.z);
        ImGui::Text("Object speed: %lf", glm::length(slc_vel));
        auto& vel = m_gui.selected_body_menu.velocity;
        if (ImGui::SliderFloat3("Velocity to add", glm::value_ptr(vel), -50, 50)) {}
        if(ImGui::Button("Add velocity vector")){
            slc->set_speed(slc_vel + vel);
            schedule_selected_body_trajectory_calc();
        };
        if (ImGui::SliderFloat("Speed", &m_gui.selected_body_menu.speed, -50, 50)) {}
        if(ImGui::Button("Add speed")){
            slc->set_speed(slc->get_speed() + glm::normalize(slc->get_speed()) * m_gui.selected_body_menu.speed);
            schedule_selected_body_trajectory_calc();
        }
        if (ImGui::SliderFloat("Object rotation speed", &m_gui.selected_body_menu.rotation_speed, -100, 100)) {
            slc->set_rotation_speed(
                m_gui.selected_body_menu.rotation_speed);
        }
        if (ImGui::SliderFloat("Object axial tilt", &m_gui.selected_body_menu.axial_tilt, 0, 180)) {
            slc->set_axial_tilt(
                m_gui.selected_body_menu.axial_tilt);
        }
        if (ImGui::ColorEdit3("Trail color", glm::value_ptr(m_gui.selected_body_menu.trail_color))) {
            slc->set_trail_color(m_gui.selected_body_menu.trail_color);
        }
        m_gui.selected_body_menu.position = m_gui.selected_body.lock()->get_pos();
        if (ImGui::InputFloat3("Object position", glm::value_ptr(m_gui.selected_body_menu.position))) {
            slc->set_pos(m_gui.selected_body_menu.position);
            schedule_selected_body_trajectory_calc();
        }
        if (ImGui::Checkbox("Track", &m_gui.selected_body_menu.track)) {
        }
        if (ImGui::Button("Jump to")) {
            auto slc = m_gui.selected_body.lock();
            auto pos = slc->get_pos();
            auto camera_front = m_camera.get_front();
            auto new_camera_pos = pos + (-camera_front * (slc->get_radius() * 2));
            m_camera.set_pos(new_camera_pos);
        }
        if (ImGui::Button("Deselect")) {
            slc->set_selected(false);
            m_gui.selected_body.reset();
        }
        if (ImGui::Button("Delete body")) {
            auto body_as_star = dynamic_cast<obj::Star*>(m_gui.selected_body.lock().get());
            if (body_as_star) {
                remove_star(body_as_star);
            } else {
                remove_planet(dynamic_cast<obj::Planet*>(m_gui.selected_body.lock().get()));
            }
            m_gui.selected_body.reset();
        }
        ImGui::End();
        if (!discarded) {
            m_gui.selected_body = std::weak_ptr<obj::CelestialBody>();
        }
    }
    void Game::draw_body_list_gui()
    {
        ImGui::Begin("Celestial bodies list", &m_gui.bodies_list_enabled);
        m_imgui_window_rects.push(get_current_imgui_window_rect());

        ImGui::BeginListBox("Celestial bodies");

        for (size_t i = 0; i < m_bodies.size(); i++) {
            auto body = m_bodies[i];
            auto& name = body->get_name();
            ImGui::PushID(i);
            if (ImGui::Selectable(name.c_str())) {
                on_body_selected(body);
            }
            ImGui::PopID();
        }
        ImGui::EndListBox();
        if (ImGui::Button("DELETE ALL")) {
            m_bodies.clear();
            collect_light_sources();
            buffer_light_data();
        }

        ImGui::End();
    }
    void Game::add_planet(obj::Planet new_planet)
    {
        auto planet = std::make_shared<obj::Planet>(std::move(new_planet));
        m_bodies.push_back(planet);
        collect_light_sources();
    }
    void Game::remove_planet(obj::Planet* planet)
    {
        auto f = std::remove_if(m_bodies.begin(), m_bodies.end(), [&](auto ptr) {
            return ptr.get() == planet;
        });
        if (f != m_bodies.end()) {
            m_bodies.erase(f);
            collect_light_sources();
        }
    }
    void Game::add_star(obj::Star&& new_star)
    {
        auto star = std::make_shared<obj::Star>(std::move(new_star));
        m_bodies.emplace_back(std::move(star));
        m_ssbos.light_sources.size++;
        collect_light_sources();
    }
    void Game::remove_star(obj::Star* star)
    {
        auto f = std::remove_if(m_bodies.begin(), m_bodies.end(), [&](auto ptr) {
            return ptr.get() == star;
        });
        if (f != m_bodies.end()) {
            m_bodies.erase(f);
            m_ssbos.light_sources.size--;
            collect_light_sources();
        }
    }
    void Game::key_handler(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        (void)window;
        m_key_events.push(KeyEvent {
            .key = key,
            .scancode = scancode,
            .action = action,
            .mods = mods,
        });
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
        m_gbuffer = Gbuffer(width, height);
        m_ubos.matrices.text_projection = glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, 0.0f, 100.0f);

        m_ubos.matrices.projection = glm::perspective(glm::radians(m_fov),
            (float)m_width / (float)m_height, 0.1f, PROJECTION_FAR_PLANE);

        glBindBuffer(GL_UNIFORM_BUFFER, m_ubos.matrices.id);
        // projection
        glBufferSubData(GL_UNIFORM_BUFFER,
            sizeof(MatricesUBO::view), sizeof(MatricesUBO::projection), glm::value_ptr(m_ubos.matrices.projection));

        // text_projection
        glBufferSubData(GL_UNIFORM_BUFFER,
            2 * sizeof(MatricesUBO::view), sizeof(MatricesUBO::text_projection), glm::value_ptr(m_ubos.matrices.text_projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        m_gui.paused.set_pos({ 0, m_height - m_gui.paused.get_text_height() });
        m_gui.fps_count.set_pos({ m_width - m_gui.game_version.get_text_width(), m_gui.game_version.get_text_height() });
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
    void Game::window_maximize_handler(GLFWwindow* window, int maximized)
    {
        (void)window;
        if (maximized) {
            glfwSetWindowAttrib(m_window_ptr, GLFW_DECORATED, GLFW_FALSE);
        } else {
            glfwSetWindowAttrib(m_window_ptr, GLFW_DECORATED, GLFW_TRUE);
        }
        m_maximize = MaximizeState::DoNothing;
    }
    static bool is_mouse_pos_in_imgui_window(glm::vec2 mouse, std::queue<glm::vec4>& q)
    {
        bool test = false;
        while (!q.empty()) {
            auto rect = q.front();
            q.pop();
            test |= mouse.x >= rect.x && mouse.x <= rect.x + rect.z && mouse.y >= rect.y && mouse.y <= rect.y + rect.w;
        }
        return test;
    }
    void Game::mouse_button_handler(GLFWwindow* window, int button, int action, int mods)
    {
        (void)mods;
        double xpos = 0, ypos = 0;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (is_mouse_pos_in_imgui_window(glm::vec2(xpos, ypos), m_imgui_window_rects))
            return;
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
                if (discr < 0)
                    continue;
                else if (discr > 0) {
                    distance = std::min(-b + std::sqrt(discr), -b - std::sqrt(discr)) / 2.0;
                } else {
                    distance = -b / 2.0;
                }
                if (distance < smallest_distance.value_or(1000) && distance >= 0) {
                    smallest_distance = distance;
                    on_body_selected(obj);
                }
            }
        }
    }
    void Game::on_body_selected(std::shared_ptr<obj::CelestialBody> obj)
    {
        if (!m_gui.selected_body.expired()) {
            m_gui.selected_body.lock()->set_selected(false);
        }
        obj->set_selected(true);
        m_gui.selected_body = obj;
        m_gui.selected_body_menu.mass = obj->get_mass();
        m_gui.selected_body_menu.color = obj->get_color();
        m_gui.selected_body_menu.velocity = obj->get_speed();
        m_gui.selected_body_menu.speed = m_gui.selected_body_menu.velocity.length();
        m_gui.selected_body_menu.trail_color = obj->get_trail_color();
        m_gui.selected_body_menu.track = false;
        m_gui.selected_body_menu.rotation_speed = obj->get_rotation_speed();
        m_gui.selected_body_menu.axial_tilt = obj->get_axial_tilt();
        if(obj->get_texture()){
            for(auto& [path, tex] : m_loaded_textures){
                if(tex == obj->get_texture()){
                    m_gui.selected_body_menu.texture_name = path.filename().generic_string();
                }
            }
        } else {
            m_gui.selected_body_menu.texture_name = "";
        }
        std::fill(m_gui.selected_body_menu.name, m_gui.selected_body_menu.name + sizeof(m_gui.selected_body_menu.name), '\0');
        auto len = std::min(obj->get_name().length(), sizeof(m_gui.selected_body_menu.name));
        std::copy(obj->get_name().c_str(), obj->get_name().c_str() + len, m_gui.selected_body_menu.name);
        schedule_selected_body_trajectory_calc();
    }
    void Game::load_custom_textures_paths(){
        auto custom_txt_path = files::game_data::textures::custom::__DIRECTORY_PATH;
        std::filesystem::create_directory(custom_txt_path);
        m_unloaded_textures.clear();
        auto dir_it = std::filesystem::directory_iterator(custom_txt_path);
        for(auto& entry : dir_it){
            if(!entry.is_regular_file() || m_loaded_textures.find(entry) != m_loaded_textures.end()) continue;
            auto& path = entry.path();
            auto ext = path.extension();
            if(ext == std::filesystem::path(".png") || ext == std::filesystem::path(".jpg")){
                m_unloaded_textures.insert(path);
            }
        }
    }
    void Game::load_texture_from_path(const std::filesystem::path& path){
        auto path_as_string = path.generic_string();
        auto texture = obj::Texture(path_as_string);
        m_loaded_textures[path] = std::make_shared<obj::Texture>(std::move(texture));
    }
    void Game::schedule_selected_body_trajectory_calc()
    {
        auto status = static_cast<gui::TrailCompStatus>(m_gui.selected_body_menu.trajectory_status.load());
        if (status == gui::TrailCompStatus::Running) {
            m_gui.selected_body_menu.calc_cancellation.cancel();
            // m_gui.selected_body_menu.trail_status.wait(gui::TrailCompStatus::Running);
            // status = static_cast<gui::TrailCompStatus>(m_gui.selected_body_menu.trail_status.load());
        }
        if (status == gui::TrailCompStatus::Terminated) {
            m_gui.selected_body_menu.trajectory_status.store(gui::TrailCompStatus::Idle);
            m_gui.selected_body_menu.calc_cancellation = gui::CancelationToken();
        }
        if (status == gui::TrailCompStatus::Finished) {
            m_gui.selected_body_menu.trajectory_status.store(gui::TrailCompStatus::Idle);
            status = static_cast<gui::TrailCompStatus>(m_gui.selected_body_menu.trajectory_status.load());
        }
        // collect bodies into gravdata and schedule the simulation
        if (status == gui::TrailCompStatus::Idle) {
            auto gravdata = std::vector<Gravdata>(m_bodies.size());
            auto selected = m_gui.selected_body.lock().get();
            size_t selected_idx {};
            for (size_t i = 0; i < m_bodies.size(); i++) {
                gravdata[i] = Gravdata {
                    .mass = m_bodies[i]->get_mass(),
                    .pos = m_bodies[i]->get_pos(),
                    .vel = m_bodies[i]->get_speed(),
                    .acc = m_bodies[i]->get_acceleration(),
                    .radius = m_bodies[i]->get_radius(),
                    .is_star = dynamic_cast<obj::Star*>(m_bodies[i].get()) != nullptr,
                };
                selected_idx = m_bodies[i].get() == selected ? i : selected_idx;
            }

            auto mean_delta_t = std::accumulate(m_delta_t_record.begin(), m_delta_t_record.end(), 0.0) / m_delta_t_record.size();

            std::thread([this](std::vector<Gravdata> gd, size_t s_idx, double mean_dt, bool do_collision) {
                auto& cancel = m_gui.selected_body_menu.calc_cancellation;
                m_gui.selected_body_menu.trajectory_status.store(gui::TrailCompStatus::Running);
                auto clock = std::chrono::steady_clock {};
                auto dt = mean_dt;
                auto last_frame_t = clock.now();
                auto gravd = std::move(gd);
                auto& res = m_gui.selected_body_menu.trajectory_data;
                const auto sz = res.size();
                for (size_t i = 0; i < sz && !cancel.is_cancelled(); i++) {
                    auto current_frame_t = clock.now();
                    std::chrono::duration<double> delta_t = current_frame_t - last_frame_t;
                    m_last_frame_t = m_current_frame_t;
                    for (auto i = 0; i < 2 && !cancel.is_cancelled(); i++) {
                        gravity_step(gravd, dt, do_collision, cancel);
                    }
                    res[i] = gravd[s_idx].pos;
                }
                if (cancel.is_cancelled()) {
                    m_gui.selected_body_menu.trajectory_status.store(gui::TrailCompStatus::Terminated);
                } else {
                    m_gui.selected_body_menu.trajectory_status.store(gui::TrailCompStatus::Finished);
                }
            },
                std::move(gravdata), selected_idx, mean_delta_t, m_gui.game_options_menu.do_collision)
                .detach();
        }
    }
    void Game::initialize_key_bindings()
    {
        m_keybinds.add_binding(GLFW_KEY_H, GLFW_PRESS, BindMode::Editor, [this]() { this->m_gui.help_menu_enabled = !this->m_gui.help_menu_enabled; }, "Toggle this help menu");
        m_keybinds.add_binding(GLFW_KEY_ESCAPE, GLFW_PRESS, BindMode::Any, [this]() { glfwSetWindowShouldClose(this->m_window_ptr, true); }, "Exit the simulation");
        // // toggle the gui with [E]dit
        m_keybinds.add_binding(GLFW_KEY_E, GLFW_PRESS, BindMode::Any, [this]() {
            this->m_gui_enabled = !this->m_gui_enabled;
            if (this->m_gui_enabled){
                glfwSetInputMode(this->m_window_ptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                m_gui.mode.set_text("Edit");
                m_gui.mode.set_color(gui::GameUI::EDIT_MODE_TEXT_COLOR);
            } else {
                glfwSetInputMode(this->m_window_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                m_gui.mode.set_text("Normal");
                m_gui.mode.set_color(gui::GameUI::NORMAL_MODE_TEXT_COLOR);
            } }, "Toggle edit mode");
        // // [P]ause the game
        m_keybinds.add_binding(GLFW_KEY_P, GLFW_PRESS, BindMode::Any, [this]() {
            this->m_paused = !this->m_paused;
            if(m_paused && !m_gui.selected_body.expired()){
                schedule_selected_body_trajectory_calc();
            }
            }, "Pause the simulation");
        // // [F]ullscreen
        m_keybinds.add_binding(GLFW_KEY_F, GLFW_PRESS, BindMode::Any, [this]() {
            int maximized = glfwGetWindowAttrib(m_window_ptr, GLFW_MAXIMIZED);
            this->m_maximize = maximized ? MaximizeState::Minimize : MaximizeState::Maximize;
            },
            "Fullscreen");
        // // Shift + S -> spawn object
        m_keybinds.add_binding(GLFW_KEY_S, GLFW_PRESS, BindMode::Any, [this]() {
            auto front = this->m_camera.get_front();
            auto pos = this->m_camera.get_pos() + front;
            auto vel = glm::normalize(front) * this->m_gui.spawn_menu.initial_velocity;
            auto color = this->m_gui.spawn_menu.color;
            auto mass = this->m_gui.spawn_menu.mass;
            auto name = std::string(this->m_gui.spawn_menu.name);
            if(name.empty()){
                name = "Unnamed";
            }
            if(m_gui.spawn_menu.is_star){
                auto star = obj::Star(nullptr, pos, vel, {}, mass);
                star.set_color(color);
                star.set_name(std::move(name));
                add_star(std::move(star));
            } else {
                auto planet = obj::Planet(nullptr, pos, vel, {}, mass);
                planet.set_color(color);
                planet.set_name(std::move(name));
                add_planet(planet);
            } }, "Spawn a new celestial body in front of the camera", GLFW_MOD_SHIFT);
        // debug menu
#ifdef DEBUG
        m_keybinds.add_binding(GLFW_KEY_D, GLFW_PRESS, BindMode::Any, [this]() { this->m_gui.debug_menu_enabled = !this->m_gui.debug_menu_enabled; }, "Open the debug menu", GLFW_MOD_SHIFT);
#endif
        // Editor mode specific keybinds
        m_keybinds.add_binding(GLFW_KEY_O, GLFW_PRESS, BindMode::Editor, [this]() { this->m_gui.game_options_menu_enabled = !this->m_gui.game_options_menu_enabled; }, "Open game settings");
        m_keybinds.add_binding(GLFW_KEY_S, GLFW_PRESS, BindMode::Editor, [this]() { this->m_gui.spawn_menu_enabled = !this->m_gui.spawn_menu_enabled; }, "Open spawn menu");
        m_keybinds.add_binding(GLFW_KEY_D, GLFW_PRESS, BindMode::Editor, [this]() {
            if(!m_gui.selected_body.expired()){
                this->m_gui.selected_body.lock()->set_selected(false);
                this->m_gui.selected_body = std::weak_ptr<obj::CelestialBody>();
            } }, "Deselect currently selected body");
        m_keybinds.add_binding(GLFW_KEY_L, GLFW_PRESS, BindMode::Editor, [this]() { this->m_gui.bodies_list_enabled = !this->m_gui.bodies_list_enabled; }, "Open celestial bodies list");
        // // [T]exture menu
        m_keybinds.add_binding(GLFW_KEY_T, GLFW_PRESS, BindMode::Editor, [this]() {
            this->m_gui.texture_menu_enabled = !this->m_gui.texture_menu_enabled;
            }, "Toggle texture menu");
        // Simulation mode specific keybinds
    }
    void Game::window_refresh_handler(GLFWwindow* window)
    {
        glfwSwapBuffers(window);
    }
}
