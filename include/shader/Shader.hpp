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
class Shader final {
private:
    std::uint32_t m_shader_id{};

public:
    Shader();
    Shader(const std::string& vert_path, const std::string& frag_path);
    static Shader from_shader_dir(const std::string& name);
    void use_shader() const;
};

#endif
