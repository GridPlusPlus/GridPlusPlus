#include "GridPlusPlus.h"

using gridpp::GridEngine;

int main() {
    GridEngine game(8, 8, 64);
    game.set_show_grid(true);
    game.Run();
    return 0;
}
