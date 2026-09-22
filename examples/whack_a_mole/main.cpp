#include <algorithm>
#include <cmath>
#include <string>

#include "GridPlusPlus.h"

using gridpp::GameEngine;
using gridpp::GridObject;
using gridpp::Overlay;

int score = 0;
double end_time = 0.0;
double next_move = 0.0;
Overlay score_label;
Overlay time_label;

void ResetGame(GameEngine game, GridObject mole) {
    score = 0;
    end_time = game.time() + 30.0;
    next_move = 0.0;
    mole.show();
    score_label.setText("Score: 0");
    time_label.setText("Time: 30");
}

void InitMole(GameEngine game, GridObject mole) { ResetGame(game, mole); }

void UpdateMole(GameEngine game, GridObject mole) {
    const double remaining = std::max(0.0, end_time - game.time());
    const int seconds = static_cast<int>(std::ceil(remaining));
    time_label.setText("Time: " + std::to_string(seconds));

    if (remaining <= 0.0) {
        mole.hide();
        if (game.keyPressed(KEY_R)) ResetGame(game, mole);
        return;
    }

    const int mouse_x = game.mouseX() / game.gridSize();
    const int mouse_y = game.mouseY() / game.gridSize();
    if (game.mousePressed(MOUSE_BUTTON_LEFT) && mouse_x == mole.x() && mouse_y == mole.y()) {
        ++score;
        score_label.setText("Score: " + std::to_string(score));
        next_move = 0.0;
    }

    if (game.time() >= next_move) {
        mole.setPosition(game.random(0, game.cols() - 1), game.random(0, game.rows() - 1));
        next_move = game.time() + 1.0;
    }
}

int main() {
    GameEngine game(8, 8, 64);
    game.showGrid(true);

    score_label = game.addTextOverlay("Score: 0", 12, 12, 24);
    time_label = game.addTextOverlay("Time: 30", 12, 44, 24);
    game.addObject("mole", InitMole, UpdateMole);

    game.run();
    return 0;
}
