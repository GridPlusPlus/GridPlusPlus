#include "GridPlusPlus.h"

using gridpp::Game;
using gridpp::ObjectHandler;

double next_move = 0.0;

void UpdateMole(Game game, ObjectHandler mole) {
    if (game.time() < next_move) return;

    mole.setPosition(game.random(0, game.cols() - 1), game.random(0, game.rows() - 1));
    next_move = game.time() + 1.0;
}

int main() {
    Game game(8, 8, 64);
    game.showGrid(true);
    game.addObject("mole", nullptr, UpdateMole);
    game.run();
    return 0;
}
