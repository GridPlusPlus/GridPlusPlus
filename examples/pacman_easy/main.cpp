// Pac-Man 入門版：只使用 GameEngine、Handler、全域變數與普通函式。
#include <cstdio>
#include <string>

#include "GridPlusPlus.h"

using gridpp::GameEngine;
using gridpp::GridObject;
using gridpp::Maze;

namespace {

constexpr int kWidth = 11;
constexpr int kHeight = 9;

// 1=牆  0=豆子  2=玩家起點  3=鬼魂起點
constexpr int kMap[kHeight][kWidth] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}, {1, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}, {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

int game_state = 0;  // 0=遊戲中  1=獲勝  2=失敗
int pellets_left = 0;
int ghost_timer = 0;
Maze maze;
GridObject player;

bool IsWall(int x, int y) { return maze.isWall(x, y); }

void MovePlayer(GameEngine game, GridObject self) {
    if (game_state != 0) return;

    const int x = self.x();
    const int y = self.y();
    if (game.keyPressed(KEY_RIGHT) && !IsWall(x + 1, y)) self.move(1, 0);
    if (game.keyPressed(KEY_LEFT) && !IsWall(x - 1, y)) self.move(-1, 0);
    if (game.keyPressed(KEY_DOWN) && !IsWall(x, y + 1)) self.move(0, 1);
    if (game.keyPressed(KEY_UP) && !IsWall(x, y - 1)) self.move(0, -1);
}

void HitPlayer(GameEngine, GridObject, GridObject other) {
    std::string type;
    other.get("type", type);
    if (type != "ghost") return;

    game_state = 2;
    std::printf("被鬼抓到了，失敗！\n");
}

void EatPellet(GameEngine, GridObject self, GridObject other) {
    std::string type;
    other.get("type", type);
    if (type != "player") return;

    self.remove();
    --pellets_left;
    if (pellets_left <= 0) {
        game_state = 1;
        std::printf("豆子吃光了，獲勝！\n");
    }
}

void MoveGhost(GameEngine, GridObject self) {
    if (game_state != 0 || !player.exists()) return;
    if (++ghost_timer < 15) return;
    ghost_timer = 0;

    const int ghost_x = self.x();
    const int ghost_y = self.y();
    if (player.x() > ghost_x && !IsWall(ghost_x + 1, ghost_y))
        self.move(1, 0);
    else if (player.x() < ghost_x && !IsWall(ghost_x - 1, ghost_y))
        self.move(-1, 0);
    else if (player.y() > ghost_y && !IsWall(ghost_x, ghost_y + 1))
        self.move(0, 1);
    else if (player.y() < ghost_y && !IsWall(ghost_x, ghost_y - 1))
        self.move(0, -1);
}

}  // namespace

int main() {
    GameEngine game(kWidth, kHeight, 40);
    game.loadAssets("pacman.db");
    game.setBackgroundColor(BLACK);

    maze = game.addMaze(kWidth, kHeight);
    maze.setWallImage("wall_cross");

    for (int y = 0; y < kHeight; ++y) {
        for (int x = 0; x < kWidth; ++x) {
            const int tile = kMap[y][x];
            if (tile == 1) {
                maze.setWall(x, y);
            } else if (tile == 0) {
                GridObject pellet = game.addObject("pellet", nullptr, nullptr, EatPellet);
                pellet.setPosition(x, y);
                pellet.set("type", "pellet");
                ++pellets_left;
            } else if (tile == 2) {
                player = game.addObject("pacman", nullptr, MovePlayer, HitPlayer);
                player.setPosition(x, y);
                player.set("type", "player");
            } else if (tile == 3) {
                GridObject ghost = game.addObject("ghost", nullptr, MoveGhost);
                ghost.setPosition(x, y);
                ghost.setColor(RED);
                ghost.set("type", "ghost");
            }
        }
    }

    game.run();
    return 0;
}
