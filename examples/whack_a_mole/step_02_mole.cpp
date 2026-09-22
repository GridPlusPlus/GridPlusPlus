#include "GridPlusPlus.h"

using gridpp::GameEngine;
using gridpp::GridObject;

double next_move = 0.0;

void UpdateMole(GameEngine game, GridObject mole) {
    if (game.time() < next_move) return;

    mole.setPosition(game.random(0, game.cols() - 1), game.random(0, game.rows() - 1));
    next_move = game.time() + 1.0;
}

int main() {
    GameEngine game(8, 8, 64);
    game.showGrid(true);
    game.addObject("mole", nullptr, UpdateMole);
    game.run();
    return 0;
}
