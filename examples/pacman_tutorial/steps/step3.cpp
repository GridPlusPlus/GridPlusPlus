// 第 3 步：迷宮——撞到牆就走不過去。
#include "GridPlusPlus.h"

using gridpp::GameEngine;
using gridpp::GridObject;
using gridpp::Maze;

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

// 讓 who 往 (dx, dy) 走一格；前面是牆就不動。
void Walk(GridObject who, int dx, int dy) {
    if (maze.isWall(who.x() + dx, who.y() + dy)) return;
    who.move(dx, dy);
}

void MovePacman(GameEngine game, GridObject self) {
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

    GridObject pacman = game.addObject("pacman", nullptr, MovePacman);
    pacman.setPosition(1, 1);

    game.run();
    return 0;
}
