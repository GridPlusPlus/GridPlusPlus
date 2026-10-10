// 第 2 步：小精靈出發——用方向鍵控制小精靈。
#include "GridPlusPlus.h"

using gridpp::GameEngine;
using gridpp::GridObject;

void MovePacman(GameEngine game, GridObject self) {
    if (game.keyPressed(KEY_RIGHT)) {
        self.move(1, 0);
        self.setDirection(0);  // 0 右、1 上、2 左、3 下
    }
    if (game.keyPressed(KEY_UP)) {
        self.move(0, -1);
        self.setDirection(1);
    }
    if (game.keyPressed(KEY_LEFT)) {
        self.move(-1, 0);
        self.setDirection(2);
    }
    if (game.keyPressed(KEY_DOWN)) {
        self.move(0, 1);
        self.setDirection(3);
    }
}

int main() {
    GameEngine game(15, 11, 48);
    game.loadAssets("pacman.db");
    game.setBackgroundColor(BLACK);

    GridObject pacman = game.addObject("pacman", nullptr, MovePacman);
    pacman.setPosition(7, 5);

    game.run();
    return 0;
}
