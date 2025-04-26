#include "Game.hpp"
#include "Util.hpp"
#include <GL/gl.h>
#include <Object.hpp>
#include <cstdio>
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>
GameObject::GameObject(const char *shader) {
  m_shader = std::make_shared<Shader>(Shader::from_shader_dir(shader));
  m_vao = 0;
  m_vbo = {};
  /*std::printf("Object constructor called\n");*/
  /*std::uint32_t vbo, vao;*/
  /*glGenBuffers(1, &vbo);*/
  /*glGenVertexArrays(1, &vao);*/
  /*glBindVertexArray(vao);*/
  /*glBindBuffer(GL_ARRAY_BUFFER, vbo);*/
  /*float verts[] = {*/
  /*    -0.5f, -0.5f, -0.5f,*/
  /*     0.5f, -0.5f, -0.5f,*/
  /*     0.5f,  0.5f, -0.5f,*/
  /*     0.5f,  0.5f, -0.5f,*/
  /*    -0.5f,  0.5f, -0.5f,*/
  /*    -0.5f, -0.5f, -0.5f,*/
  /**/
  /*    -0.5f, -0.5f,  0.5f,*/
  /*     0.5f, -0.5f,  0.5f,*/
  /*     0.5f,  0.5f,  0.5f,*/
  /*     0.5f,  0.5f,  0.5f,*/
  /*    -0.5f,  0.5f,  0.5f,*/
  /*    -0.5f, -0.5f,  0.5f,*/
  /**/
  /*    -0.5f,  0.5f,  0.5f,*/
  /*    -0.5f,  0.5f, -0.5f,*/
  /*    -0.5f, -0.5f, -0.5f,*/
  /*    -0.5f, -0.5f, -0.5f,*/
  /*    -0.5f, -0.5f,  0.5f,*/
  /*    -0.5f,  0.5f,  0.5f,*/
  /**/
  /*     0.5f,  0.5f,  0.5f,*/
  /*     0.5f,  0.5f, -0.5f,*/
  /*     0.5f, -0.5f, -0.5f,*/
  /*     0.5f, -0.5f, -0.5f,*/
  /*     0.5f, -0.5f,  0.5f,*/
  /*     0.5f,  0.5f,  0.5f,*/
  /**/
  /*    -0.5f, -0.5f, -0.5f,*/
  /*     0.5f, -0.5f, -0.5f,*/
  /*     0.5f, -0.5f,  0.5f,*/
  /*     0.5f, -0.5f,  0.5f,*/
  /*    -0.5f, -0.5f,  0.5f,*/
  /*    -0.5f, -0.5f, -0.5f,*/
  /**/
  /*    -0.5f,  0.5f, -0.5f,*/
  /*     0.5f,  0.5f, -0.5f,*/
  /*     0.5f,  0.5f,  0.5f,*/
  /*     0.5f,  0.5f,  0.5f,*/
  /*    -0.5f,  0.5f,  0.5f,*/
  /*    -0.5f,  0.5f, -0.5f,*/
  /*};*/
  /*glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);*/
  /*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
   * (void*)0);*/
  /*glEnableVertexAttribArray(0);*/
  /**/
  /*std::printf("VAO: %d VBO: %d\n", vao, vbo);*/
  /**/
  /*Shader test_shader;*/
  /*test_shader = Shader::from_shader_dir(shader);*/
  /*std::cout << "shader loaded successfully\n";*/
  /*m_shader = test_shader;*/
  /*m_vao = vao;*/
  /*m_vbo = vbo;*/
}

GameObject::GameObject() : m_vao{0}, m_vbo{{}}, m_shader{nullptr} {}

GameObject::GameObject(std::shared_ptr<Shader> shader)
    : m_vao{0}, m_vbo{{}}, m_shader{shader} {}
GameObject::GameObject(const char *vert, const char *frag)
    : m_vao{0}, m_vbo{{}},
      m_shader{std::make_unique<Shader>(Shader(vert, frag))} {}
GameObject::GameObject(const GameObject &other)
    : m_vbo{other.m_vbo}, m_vao{other.m_vao}, m_pos{other.m_pos},
      m_shader{other.m_shader} {}
GameObject &GameObject::operator=(const GameObject &other) {
  m_pos = other.m_pos;
  m_vao = other.m_vao;
  m_pos = other.m_pos;
  m_shader = other.m_shader;
  return *this;
}
GameObject::GameObject(GameObject &&other)
    : m_vbo{std::move(other.m_vbo)}, m_vao{other.m_vao}, m_pos{other.m_pos},
      m_shader{std::move(other.m_shader)} {
  other.m_vao = 0;
  other.m_shader = nullptr;
}
GameObject &GameObject::operator=(GameObject &&other) {
  m_vbo = std::move(other.m_vbo);
  m_vao = other.m_vao;
  other.m_vao = 0;
  m_pos = other.m_pos;
  m_shader = std::move(other.m_shader);
  return *this;
}
GameObject::~GameObject() {
  glDeleteVertexArrays(1, &m_vao);
  for(const auto& vbo : m_vbo)
      glDeleteBuffers(1, &vbo);
}

CelestialBody::CelestialBody(std::shared_ptr<Shader> shader):
    GameObject(shader)
{

}

void CelestialBody::render() {
    m_shader->use_shader();
    auto model = glm::mat4(1);
    model = glm::translate(model, m_pos);
    m_shader->set_mat4(name_of(model), model);
    glBindVertexArray(m_vao);
}
