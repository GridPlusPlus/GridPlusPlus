// Grid++ minimal project. See docs/01-getting-started/02-hello-gridpp.md.
#include "GridPlusPlus.h"

using gridpp::Game;

int main() {
    Game game(8, 8, 64);
    game.showGrid(true);
    game.run();
    return 0;
}
