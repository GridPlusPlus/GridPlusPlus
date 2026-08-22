#include "GridPlusPlus.h"

#include <algorithm>
#include <cmath>
#include <string>

using gridpp::GridEngine;
using gridpp::GridObject;
using gridpp::Label;

int score = 0;
double end_time = 0.0;
double next_move = 0.0;
Label* score_label = nullptr;
Label* time_label = nullptr;

void ResetGame(GridObject* mole) {
    score = 0;
    end_time = GetTime() + 30.0;
    next_move = 0.0;
    mole->set_visible(true);
    score_label->set_text("Score: 0");
    time_label->set_text("Time: 30");
}

void UpdateMole(GridObject* mole) {
    const double remaining = std::max(0.0, end_time - GetTime());
    const int seconds = static_cast<int>(std::ceil(remaining));
    time_label->set_text("Time: " + std::to_string(seconds));

    if (remaining <= 0.0) {
        mole->set_visible(false);
        if (IsKeyPressed(KEY_R)) ResetGame(mole);
        return;
    }

    const Vector2 mouse = GetMousePosition();
    const int grid_size = mole->engine()->grid_size();
    const int mouse_x = static_cast<int>(mouse.x) / grid_size;
    const int mouse_y = static_cast<int>(mouse.y) / grid_size;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouse_x == mole->x() && mouse_y == mole->y()) {
        ++score;
        score_label->set_text("Score: " + std::to_string(score));
        next_move = 0.0;
    }

    if (GetTime() >= next_move) {
        mole->set_x(GetRandomValue(0, mole->engine()->cols() - 1));
        mole->set_y(GetRandomValue(0, mole->engine()->rows() - 1));
        next_move = GetTime() + 1.0;
    }
}

int main() {
    GridEngine game(8, 8, 64);
    game.set_show_grid(true);

    score_label = new Label("Score: 0", 12, 12, 24, BLACK);
    time_label = new Label("Time: 30", 12, 44, 24, BLACK);
    game.AddOverlay(score_label);
    game.AddOverlay(time_label);

    GridObject* mole = game.Spawn("mole", 0, 0, UpdateMole);
    ResetGame(mole);
    game.Run();
    return 0;
}
