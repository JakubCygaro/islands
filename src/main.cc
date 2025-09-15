#include "Game.hpp"
#include <iostream>
#include <stdexcept>

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 800;

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
