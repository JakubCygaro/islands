#include "Game.hpp"
#include <iostream>
#include <stdexcept>

int main(void) {
    try{
        gm::Game game;
        game.run();
    } catch(const std::runtime_error& e){
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
