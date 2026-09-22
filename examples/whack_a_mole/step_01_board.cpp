#include "GridPlusPlus.h"

using gridpp::GameEngine;

int main() {
    GameEngine game(8, 8, 64);
    game.showGrid(true);
    game.run();
    return 0;
}
