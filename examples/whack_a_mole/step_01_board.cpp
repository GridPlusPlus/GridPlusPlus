#include "GridPlusPlus.h"

using gridpp::Game;

int main() {
    Game game(8, 8, 64);
    game.showGrid(true);
    game.run();
    return 0;
}
