#include "Game.hpp"
#include <cstdint>
#include <cstdio>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <ostream>
#include <stb_image/stb_image.h>
#include <shader/Shader.hpp>
#include <stdexcept>
#include <Camera.hpp>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main(void) {
    try{
        Game game(SCR_WIDTH, SCR_HEIGHT);
        game.run();
    } catch(const std::runtime_error& e){
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
