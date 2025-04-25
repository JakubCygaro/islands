#include <Object.hpp>
#include <cstdio>
#include <glm/ext/matrix_float4x4.hpp>
Object::Object(const char* shader){
    std::printf("Object constructor called\n");
    std::uint32_t vbo, vao;
    glGenBuffers(1, &vbo);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    float verts[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    std::printf("VAO: %d VBO: %d\n", vao, vbo);

    Shader test_shader;
    test_shader = Shader::from_shader_dir(shader);
    std::cout << "shader loaded successfully\n";
    m_shader = test_shader;
    m_vao = vao;
    m_vbo = vbo;
}
Object::Object() : m_vao{0}, m_vbo{0}{
    std::printf("Object default constructor\n");
    std::printf("VAO: %d VBO: %d\n", m_vao, m_vbo);
}
Object::Object(const Object& other): m_vbo{other.m_vbo}, m_vao{other.m_vao}, m_pos{other.m_pos}, m_shader{other.m_shader}{

}
Object& Object::operator=(const Object& other){
    m_pos = other.m_pos;
    m_vao = other.m_vao;
    m_pos = other.m_pos;
    m_shader = other.m_shader;
    return *this;
}
Object::Object(Object&& other): m_vbo{other.m_vbo}, m_vao{other.m_vao}, m_pos{other.m_pos}, m_shader{std::move(other.m_shader)}{
    other.m_vao = 0;
    other.m_vbo = 0;
}
Object& Object::operator=(Object&& other){
    m_vbo = other.m_vbo;
    other.m_vbo = 0;
    m_vao = other.m_vao;
    other.m_vao = 0;
    m_pos = other.m_pos;
    m_shader = std::move(other.m_shader);
    return *this;
}
Object::~Object(){
    std::printf("DESTRUCTOR VAO: %d VBO: %d\n", m_vao, m_vbo);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}
void Object::update(){

}
void Object::render(glm::mat4& view, glm::mat4& projection){
    auto model = glm::mat4(1);
    model = glm::translate(model, m_pos);

    m_shader.use_shader();
    m_shader.set_mat4("model", model);
    m_shader.set_mat4("view", view);
    m_shader.set_mat4("projection", projection);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}
