#include "Font.hpp"
#include <cstdio>
#include <glm/gtc/type_ptr.hpp>
#include <shader/Shader.hpp>
#include <sstream>

constexpr const char* SHADER_DIR_PREFIX = "./game_data/shaders/";
Shader::Shader() {

}
// Shader::Shader(const Shader& other):
//     m_shader_id{other.m_shader_id}
// {
//
// }
// Shader& Shader::operator=(const Shader& other){
//     m_shader_id = other.m_shader_id;
//     return *this;
// }
Shader::Shader(Shader&& other):
    m_shader_id{other.m_shader_id}
{
    other.m_shader_id = 0;
}
Shader& Shader::operator=(Shader&& other){
    m_shader_id = other.m_shader_id;
    other.m_shader_id = 0;
    return *this;
}
Shader::~Shader(){
    if(m_shader_id)
        glDeleteShader(m_shader_id);
}
Shader::Shader(const char* vert, const char* frag, const char* geom) try {
    const char* vert_char_ptr = vert;
    const char* frag_char_ptr = frag;
    const char* geom_char_ptr = geom;

    std::uint32_t vert_id, frag_id, geom_id;
    int succ;
    char info_log[1024];

    vert_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_id, 1, &vert_char_ptr, NULL);
    glCompileShader(vert_id);
    glGetShaderiv(vert_id, GL_COMPILE_STATUS, &succ);
    if(!succ) {
        glGetShaderInfoLog(vert_id, sizeof info_log, NULL, info_log);
        std::stringstream err_stream;
        err_stream << "Vertex shader compile error: ";
        err_stream << info_log;
        std::string err_msg = err_stream.str();
        throw std::runtime_error{std::move(err_msg)};
    }
    frag_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_id, 1, &frag_char_ptr, NULL);
    glCompileShader(frag_id);
    glGetShaderiv(frag_id, GL_COMPILE_STATUS, &succ);
    if(!succ) {
        glGetShaderInfoLog(frag_id, sizeof info_log, NULL, info_log);
        std::stringstream err_stream;
        err_stream << "Fragment shader compile error: ";
        err_stream << info_log;
        std::string err_msg = err_stream.str();
        throw std::runtime_error{std::move(err_msg)};
    }
    if(geom){
        geom_id = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geom_id, 1, &geom_char_ptr, NULL);
        glCompileShader(geom_id);
        glGetShaderiv(geom_id, GL_COMPILE_STATUS, &succ);
        if(!succ) {
            glGetShaderInfoLog(geom_id, sizeof info_log, NULL, info_log);
            std::stringstream err_stream;
            err_stream << "Geom shader compile error: ";
            err_stream << info_log;
            std::string err_msg = err_stream.str();
            throw std::runtime_error{std::move(err_msg)};
        }
    }
    m_shader_id = glCreateProgram();
    glAttachShader(m_shader_id, vert_id);
    glAttachShader(m_shader_id, frag_id);
    if(geom){
        glAttachShader(m_shader_id, geom_id);
    }
    glLinkProgram(m_shader_id);

    glGetProgramiv(m_shader_id, GL_LINK_STATUS, &succ);
    if(!succ) {
        glGetProgramInfoLog(m_shader_id, sizeof info_log, NULL, info_log);
        std::stringstream err_stream;
        err_stream << "Shader link error: ";
        err_stream << info_log;
        std::string err_msg = err_stream.str();
        throw std::runtime_error{std::move(err_msg)};
    }

    glDeleteShader(vert_id);
    glDeleteShader(frag_id);
    glDeleteShader(geom_id);
} catch(const std::runtime_error& e){
    std::stringstream err_stream;
    err_stream << "Shader creation error: ";
    err_stream << e.what();
    std::string err_msg = err_stream.str();
    throw std::runtime_error{std::move(err_msg)};
}

Shader::Shader(const std::string& vert_path, const std::string& frag_path, const std::string* geom_path) try {
    /*std::cout << "vert_path: " << vert_path << "\nfrag_path: " << frag_path << '\n';*/
    std::ifstream vert_file;
    std::ifstream frag_file;
    std::ifstream geom_file;

    vert_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    frag_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    geom_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    vert_file.open(vert_path.c_str());
    std::stringstream vert_stream;
    vert_stream << vert_file.rdbuf();

    frag_file.open(frag_path.c_str());
    std::stringstream frag_stream;
    frag_stream << frag_file.rdbuf();

    std::string geom_src;
    if(geom_path != nullptr){
        geom_file.open(geom_path->c_str());
        std::stringstream geom_stream;
        geom_stream << geom_file.rdbuf();
        geom_src = geom_stream.str();
    }

    std::string vert_src = vert_stream.str();
    std::string frag_src = frag_stream.str();

    const char* vert_char_ptr = vert_src.c_str();
    const char* frag_char_ptr = frag_src.c_str();
    const char* geom_char_ptr = geom_src.c_str();

    std::uint32_t vert_id, frag_id, geom_id;
    int succ;
    char info_log[1024];

    vert_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_id, 1, &vert_char_ptr, NULL);
    glCompileShader(vert_id);
    glGetShaderiv(vert_id, GL_COMPILE_STATUS, &succ);
    if(!succ) {
        glGetShaderInfoLog(vert_id, sizeof info_log, NULL, info_log);
        std::stringstream err_stream;
        err_stream << "Vertex shader compile error: ";
        err_stream << info_log;
        err_stream << " file: " << vert_path;
        std::string err_msg = err_stream.str();
        throw std::runtime_error{std::move(err_msg)};
    }
    frag_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_id, 1, &frag_char_ptr, NULL);
    glCompileShader(frag_id);
    glGetShaderiv(frag_id, GL_COMPILE_STATUS, &succ);
    if(!succ) {
        glGetShaderInfoLog(frag_id, sizeof info_log, NULL, info_log);
        std::stringstream err_stream;
        err_stream << "Fragment shader compile error: ";
        err_stream << info_log;
        err_stream << " file: " << frag_path;
        std::string err_msg = err_stream.str();
        throw std::runtime_error{std::move(err_msg)};
    }
    if(geom_path){
        geom_id = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geom_id, 1, &geom_char_ptr, NULL);
        glCompileShader(geom_id);
        glGetShaderiv(geom_id, GL_COMPILE_STATUS, &succ);
        if(!succ) {
            glGetShaderInfoLog(geom_id, sizeof info_log, NULL, info_log);
            std::stringstream err_stream;
            err_stream << "Geometry shader compile error: ";
            err_stream << info_log;
            err_stream << " file: " << geom_path;
            std::string err_msg = err_stream.str();
            throw std::runtime_error{std::move(err_msg)};
        }
    }

    m_shader_id = glCreateProgram();
    glAttachShader(m_shader_id, vert_id);
    glAttachShader(m_shader_id, frag_id);
    if(geom_path){
        glAttachShader(m_shader_id, geom_id);
    }
    glLinkProgram(m_shader_id);

    glGetProgramiv(m_shader_id, GL_LINK_STATUS, &succ);
    if(!succ) {
        glGetProgramInfoLog(m_shader_id, sizeof info_log, NULL, info_log);
        std::stringstream err_stream;
        err_stream << "Shader link error: ";
        err_stream << info_log;
        std::string err_msg = err_stream.str();
        throw std::runtime_error{std::move(err_msg)};
    }

    glDeleteShader(vert_id);
    glDeleteShader(frag_id);
    glDeleteShader(geom_id);
} catch(const std::runtime_error& e){
    std::stringstream err_stream;
    err_stream << "Shader creation error: ";
    err_stream << e.what();
    err_stream << " file: " << geom_path;
    std::string err_msg = err_stream.str();
    throw std::runtime_error{std::move(err_msg)};
}

Shader Shader::from_shader_dir(const std::string& name) {
    std::stringstream ss{};
    ss << SHADER_DIR_PREFIX;
    ss << name;

    auto vert = ss.str();
    vert += ".vert";
    auto frag = ss.str();
    frag += ".frag";
    return Shader(vert, frag);
}
void Shader::use_shader() const {
    glUseProgram(m_shader_id);
}
void Shader::set_vec3(const char* uniform_name, const glm::vec3& v) {
    int loc = glGetUniformLocation(m_shader_id, uniform_name);
    glUniform3fv(loc, 1, glm::value_ptr(v));
}
void Shader::set_mat4(const char* uniform_name, glm::mat4 m) {
    int loc = glGetUniformLocation(m_shader_id, uniform_name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
}
void Shader::set_mat3(const char* uniform_name, glm::mat3 m) {
    int loc = glGetUniformLocation(m_shader_id, uniform_name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
}
void Shader::set_int(const char* uniform_name, int i) {
    int loc = glGetUniformLocation(m_shader_id, uniform_name);
    glUniform1i(loc, i);
}
void Shader::set_float(const char* uniform_name, float f) {
    int loc = glGetUniformLocation(m_shader_id, uniform_name);
    glUniform1f(loc, f);
}
uint32_t Shader::get_uniform_block_index(const char* block_name){
    return glGetUniformBlockIndex(m_shader_id, block_name);
}
void Shader::set_uniform_block_binding(const char* block_name, uint32_t binding){
    glUniformBlockBinding(m_shader_id, get_uniform_block_index(block_name), binding);
}

