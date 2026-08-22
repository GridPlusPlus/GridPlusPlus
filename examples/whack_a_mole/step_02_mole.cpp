#include "GridPlusPlus.h"

using gridpp::GridEngine;
using gridpp::GridObject;

double next_move = 0.0;

void UpdateMole(GridObject* mole) {
    if (GetTime() < next_move) return;

    mole->set_x(GetRandomValue(0, mole->engine()->cols() - 1));
    mole->set_y(GetRandomValue(0, mole->engine()->rows() - 1));
    next_move = GetTime() + 1.0;
}

int main() {
    GridEngine game(8, 8, 64);
    game.set_show_grid(true);
    game.Spawn("mole", 0, 0, UpdateMole);
    game.Run();
    return 0;
}
