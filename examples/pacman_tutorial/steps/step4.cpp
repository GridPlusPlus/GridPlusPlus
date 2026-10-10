// 第 4 步：鬼來了——雙人對戰，被抓到就輸。
#include "GridPlusPlus.h"

using gridpp::GameEngine;
using gridpp::GridObject;
using gridpp::Maze;
using gridpp::Overlay;

const int kCols = 15;
const int kRows = 11;
const char* kMap[kRows] = {
    "###############",
    "#P............#",
    "#.#.###.###.#.#",
    "#.............#",
    "#.#.#.#.#.#.#.#",
    "#.............#",
    "#.#.#.#.#.#.#.#",
    "#.............#",
    "#.#.###.###.#.#",
    "#............G#",
    "###############",
};  // '#' 是牆，'P' 是小精靈的起點，'G' 是鬼的起點

Maze maze;
Overlay message;
bool game_over = false;

// 讓 who 往 (dx, dy) 走一格；前面是牆就不動。
void Walk(GridObject who, int dx, int dy) {
    if (maze.isWall(who.x() + dx, who.y() + dy)) return;
    who.move(dx, dy);
}

void GameOver() {
    game_over = true;
    message.setText("GAME OVER");
    message.show();
}

// 小精靈和別的東西撞在同一格時，Engine 會呼叫這個函式。
void PacmanHit(GameEngine, GridObject, GridObject other) {
    if (other.image() == "ghost") GameOver();
}

void MovePacman(GameEngine game, GridObject self) {
    if (game_over) return;

    if (game.keyPressed(KEY_RIGHT)) {
        self.setDirection(0);
        Walk(self, 1, 0);
    }
    if (game.keyPressed(KEY_UP)) {
        self.setDirection(1);
        Walk(self, 0, -1);
    }
    if (game.keyPressed(KEY_LEFT)) {
        self.setDirection(2);
        Walk(self, -1, 0);
    }
    if (game.keyPressed(KEY_DOWN)) {
        self.setDirection(3);
        Walk(self, 0, 1);
    }
}

// 第二位玩家用 W、A、S、D 控制鬼。
void MoveGhost(GameEngine game, GridObject self) {
    if (game_over) return;

    if (game.keyPressed(KEY_D)) Walk(self, 1, 0);
    if (game.keyPressed(KEY_W)) Walk(self, 0, -1);
    if (game.keyPressed(KEY_A)) Walk(self, -1, 0);
    if (game.keyPressed(KEY_S)) Walk(self, 0, 1);
}

int main() {
    GameEngine game(kCols, kRows, 48);
    game.loadAssets("pacman.db");
    game.setBackgroundColor(BLACK);

    maze = game.addMaze(kCols, kRows);
    maze.setWallImages("wall_iso", "wall_end", "wall_straight", "wall_corner", "wall_tee", "wall_cross");
    for (int y = 0; y < kRows; ++y) {
        for (int x = 0; x < kCols; ++x) {
            if (kMap[y][x] == '#') maze.setWall(x, y);
        }
    }

    GridObject pacman = game.addObject("pacman", nullptr, MovePacman, PacmanHit);
    pacman.setPosition(1, 1);

    GridObject ghost = game.addObject("ghost", nullptr, MoveGhost);
    ghost.setPosition(13, 9);
    ghost.setColor(RED);

    message = game.addTextOverlay("GAME OVER", 195, 230, 60, RED);
    message.hide();

    game.run();
    return 0;
}
