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

    inline static constexpr float cube_vbo_data[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    inline static constexpr size_t vert_count = sizeof(cube_vbo_data) / sizeof(float) / 3;

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
