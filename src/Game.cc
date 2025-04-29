#include "Object.hpp"
#include "shader/Shader.hpp"
#include <GLFW/glfw3.h>
#include <Game.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <type_traits>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <memory>

static Game* get_game_instance_ptr_from_window(GLFWwindow* window);

Game::Game(int32_t window_width, int32_t window_height):
    m_width{window_width},
    m_height{window_height},
    m_camera{Camera(glm::vec3(0,0,3), glm::vec3(0))},
    m_uniform_buffer{0}
{
    initialize();
}
Game::~Game(){
    glDeleteBuffers(1, &m_uniform_buffer);
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
    /*auto scroll_callback = [](GLFWwindow* window, double x, double y){*/
    /*    Game* instance = get_game_instance_ptr_from_window(window);*/
    /*    instance->mouse_handler(window, x, y);*/
    /*};*/
    /*glfwSetScrollCallback(window, scroll_callback);*/

    // tell GLFW to capture our mouse
    glfwSetInputMode(m_window_ptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD");
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    /*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/
    m_projection = glm::perspective(glm::radians(70.0f),
            (float)m_width / (float)m_height, 0.1f, 100.0f);
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
    m_bodies.push_back(std::make_shared<obj::CelestialBody>(obj::CelestialBody("test")));
}
void Game::run() {
    while(!glfwWindowShouldClose(m_window_ptr)){
        m_current_frame_t = glfwGetTime();
        m_delta_t = m_current_frame_t - m_last_frame_t;
        m_last_frame_t = m_current_frame_t;
        update();
        render();
        glfwSwapBuffers(m_window_ptr);
    }
}
void Game::update() {
    m_view = m_camera.get_look_at();
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    keyboard_input();
    for (auto& obj : m_bodies){
        obj->update(m_delta_t);
    }
    glfwPollEvents();
}
void Game::render() {
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(auto& c_obj : m_bodies){
        c_obj->render();
    }
}
void Game::keyboard_input(){
    if (glfwGetKey(m_window_ptr, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(m_window_ptr, true);
    }
    /*if (glfwGetKey(m_window_ptr, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(m_window_ptr, GLFW_KEY_W) == GLFW_PRESS){*/
    /*    if(m_wireframe)*/
    /*        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/
    /*    else*/
    /*        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/
    /*}*/
    m_camera.keyboard_input(m_window_ptr, m_delta_t);
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

static Game* get_game_instance_ptr_from_window(GLFWwindow* window){
    Game* instance = static_cast<Game*>(glfwGetWindowUserPointer(window));
    return instance;
}
