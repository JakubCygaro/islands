#ifndef SHADER_HPP
#define SHADER_HPP
#include <cstdint>
#include <string>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <sstream>
#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
class Shader final {
private:
    std::uint32_t m_shader_id{};

public:
    Shader();
    Shader(const Shader& other);
    Shader& operator=(const Shader& other);
    Shader(Shader&& other);
    Shader& operator=(Shader&& other);
    ~Shader();
    Shader(const std::string& vert_path, const std::string& frag_path);
    Shader(const char* vert, const char* frag);
    static Shader from_shader_dir(const std::string& name);
    void use_shader() const;
    void set_vec3(const char* uniform_name, const glm::vec3& v);
    void set_mat4(const char* uniform_name, glm::mat4 m);
    void set_mat3(const char* uniform_name, glm::mat3 m);
    uint32_t get_uniform_block_index(const char* block_name);
    void set_uniform_block_binding(const char* block_name, uint32_t binding);
};

#endif
