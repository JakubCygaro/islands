#include "Game.hpp"
#include <iostream>
#include <stdexcept>

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

const unsigned int INTERNAL_WIDTH = 1920;
const unsigned int INTERNAL_HEIGHT = 1080;
int main(void) {
    try{
        Game game(SCR_WIDTH, SCR_HEIGHT, INTERNAL_WIDTH, INTERNAL_HEIGHT);
        game.run();
    } catch(const std::runtime_error& e){
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
