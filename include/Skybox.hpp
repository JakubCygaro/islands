#ifndef SKYBOX_HPP
#define SKYBOX_HPP
#include "shader/Shader.hpp"
#include <array>
#include <cstdint>
#include <string>
#include <files.hpp>
#ifdef DEBUG
#include <shader_files.hpp>
#else
#include <shaders.hpp>
#endif

class Skybox final {
    uint32_t m_cubemap{};
    uint32_t m_cube_vao{};
    uint32_t m_cube_vbo{};
    uint32_t m_cube_ebo{};

    inline static constexpr glm::vec3 cube_vbo_data[] = {
        { -0.5, -0.5, -0.5 }, // bottom left back 0
        { 0.5, -0.5, -0.5 }, // bottom right back 1
        { -0.5, -0.5, 0.5 }, // bottom left front 2
        { 0.5, -0.5, 0.5 }, // bottom right front 3
        { -0.5, 0.5, -0.5 }, // top left back 4
        { 0.5, 0.5, -0.5 }, // top right back 5
        { -0.5, 0.5, 0.5 }, // top left front 6
        { 0.5, 0.5, 0.5 }, // top right front 7
    };
    inline static constexpr uint32_t cube_ebo_data[] = {
        // left face
        0, 6, 4,
        0, 2, 6,
        // front face
        2, 3, 7,
        2, 7, 6,
        // right face
        3, 1, 5,
        3, 5, 7,
        // back face
        1, 0, 4,
        1, 4, 5,
        // bottom face
        0, 2, 3,
        0, 3, 1,
        // top face
        4, 6, 7,
        4, 7, 5
    };
    inline static constexpr size_t index_count = sizeof(cube_ebo_data);

    struct ShaderInstance  {
        Shader shader;

    private:
        inline ShaderInstance(){
#ifdef DEBUG
            //load directly from source tree -> works without whole project rebuild
            shader = Shader(
                        std::string(files::src::shaders::SKYBOX_VERT),
                        std::string(files::src::shaders::SKYBOX_FRAG));
#else
            shader  = Shader(
                        shaders::SKYBOX_VERT,
                        shaders::SKYBOX_FRAG);
#endif
        }
        ShaderInstance(const ShaderInstance& other) = delete;
        ShaderInstance& operator=(const ShaderInstance& other) = delete;
    public:
        inline static ShaderInstance& get_instance(){
            static ShaderInstance instance;
            return instance;
        }

    };

public:
    Skybox(const std::array<std::string, 6>& path_to_faces);
    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;
    Skybox(Skybox&&);
    Skybox& operator=(Skybox&&);
    ~Skybox();

    void forward_render() const;
};

#endif
