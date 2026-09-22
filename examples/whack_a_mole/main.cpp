#include "GridPlusPlus.h"

#include <algorithm>
#include <cmath>
#include <string>

using gridpp::Game;
using gridpp::ObjectHandler;
using gridpp::OverlayHandler;

int score = 0;
double end_time = 0.0;
double next_move = 0.0;
OverlayHandler score_label;
OverlayHandler time_label;

void ResetGame(Game game, ObjectHandler mole) {
    score = 0;
    end_time = game.time() + 30.0;
    next_move = 0.0;
    mole.show();
    score_label.setText("Score: 0");
    time_label.setText("Time: 30");
}

void InitMole(Game game, ObjectHandler mole) { ResetGame(game, mole); }

void UpdateMole(Game game, ObjectHandler mole) {
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
    Game game(8, 8, 64);
    game.showGrid(true);

    score_label = game.addTextOverlay("Score: 0", 12, 12, 24);
    time_label = game.addTextOverlay("Time: 30", 12, 44, 24);
    game.addObject("mole", InitMole, UpdateMole);

    game.run();
    return 0;
}
