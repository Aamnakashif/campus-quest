// main.cpp - entry point of Campus Quest
#include <exception>
#include <iostream>
#include "Game.h"

int main() {
    try {
        Game game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Campus Quest could not start: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
