// 你的第一個 Grid++ 程式：小精靈出現在畫面中央。
#include "GridPlusPlus.h"

using gridpp::GameEngine;
using gridpp::GridObject;

int main() {
    GameEngine game(15, 11, 48);  // 15 格寬、11 格高，每格 48 像素
    game.loadAssets("pacman.db");
    game.setBackgroundColor(BLACK);

    GridObject pacman = game.addObject("pacman");
    pacman.setPosition(7, 5);

    game.addTextOverlay("Hello, Grid++!", 20, 20, 30, YELLOW);

    game.run();
    return 0;
}
