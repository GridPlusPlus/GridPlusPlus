// Pac-Man 完整版：和教學版用相同的寫法，再加上檔案地圖、持續移動、四隻鬼與開始／暫停／重玩按鈕。
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include "GridPlusPlus.h"
#include "LevelMap.h"

using gridpp::GameEngine;
using gridpp::GridObject;
using gridpp::Maze;
using gridpp::Overlay;
using pacman_example::LevelMap;
using pacman_example::LoadLevelMap;

namespace {

constexpr int kDX[4] = {1, 0, -1, 0};  // 方向 0 右、1 上、2 左、3 下
constexpr int kDY[4] = {0, -1, 0, 1};
constexpr double kPacmanStepTime = 0.13;  // 小精靈每 0.13 秒走一格
constexpr double kGhostStepTime = 0.2;    // 鬼每 0.2 秒走一格

enum class GameState {
    kStart,
    kPlaying,
    kWon,
    kLost,
};

GameState game_state = GameState::kStart;
int pellets_left = 0;
bool paused = false;
Maze maze;
GridObject pacman;
LevelMap level;

std::string TypeOf(GridObject object) {
    std::string type;
    object.get("type", type);
    return type;
}

// 鬼是否和小精靈在同一格。
bool Caught(GridObject ghost) { return ghost.x() == pacman.x() && ghost.y() == pacman.y(); }

void EatPellet(GameEngine, GridObject self, GridObject other) {
    if (TypeOf(other) != "pacman") return;

    self.remove();
    pellets_left--;
    if (pellets_left == 0) game_state = GameState::kWon;
}

void PacmanHit(GameEngine, GridObject, GridObject other) {
    if (TypeOf(other) == "ghost") game_state = GameState::kLost;
}

void MovePacman(GameEngine game, GridObject self) {
    if (game_state != GameState::kPlaying || paused) return;

    // 每一幀都記下最後按的方向，等那個方向走得過去時再轉彎。
    int wanted_direction = -1;
    self.get("wantedDirection", wanted_direction);
    if (game.keyDown(KEY_RIGHT)) wanted_direction = 0;
    if (game.keyDown(KEY_UP)) wanted_direction = 1;
    if (game.keyDown(KEY_LEFT)) wanted_direction = 2;
    if (game.keyDown(KEY_DOWN)) wanted_direction = 3;
    self.set("wantedDirection", wanted_direction);

    double next_move = 0.0;
    self.get("nextMove", next_move);
    if (game.time() < next_move) return;  // 還沒輪到小精靈走
    self.set("nextMove", game.time() + kPacmanStepTime);

    int direction = -1;
    self.get("direction", direction);
    if (wanted_direction >= 0 && !maze.isWall(self.x() + kDX[wanted_direction], self.y() + kDY[wanted_direction])) {
        direction = wanted_direction;
        self.set("direction", direction);
    }
    if (direction >= 0 && !maze.isWall(self.x() + kDX[direction], self.y() + kDY[direction])) {
        self.move(kDX[direction], kDY[direction]);
        self.setDirection(direction);
    }
}

void MoveGhost(GameEngine game, GridObject self) {
    if (game_state != GameState::kPlaying || paused || !pacman.exists()) return;

    double next_move = 0.0;
    self.get("nextMove", next_move);
    if (game.time() < next_move) return;  // 還沒輪到這隻鬼走
    self.set("nextMove", game.time() + kGhostStepTime);

    // 走之前先看一眼：小精靈是不是自己撞上來了？
    if (Caught(self)) {
        game_state = GameState::kLost;
        return;
    }

    int direction = 0;
    bool random_ghost = false;
    self.get("direction", direction);
    self.get("random", random_ghost);

    // 列出走得通、而且不用回頭的方向；走進死路時才回頭。
    int back = (direction + 2) % 4;
    int choices[4];
    int choice_count = 0;
    for (int d = 0; d < 4; ++d) {
        if (d == back) continue;
        if (maze.isWall(self.x() + kDX[d], self.y() + kDY[d])) continue;
        choices[choice_count] = d;
        choice_count++;
    }
    if (choice_count == 0) {
        if (maze.isWall(self.x() + kDX[back], self.y() + kDY[back])) return;  // 四面都是牆
        choices[0] = back;
        choice_count = 1;
    }

    // 隨機的鬼亂選一條路，其他鬼選離小精靈最近的那條。
    int picked = choices[0];
    if (random_ghost) {
        picked = choices[game.random(0, choice_count - 1)];
    } else {
        int best_distance = 9999;
        for (int i = 0; i < choice_count; ++i) {
            int x = self.x() + kDX[choices[i]];
            int y = self.y() + kDY[choices[i]];
            int distance = std::abs(x - pacman.x()) + std::abs(y - pacman.y());
            if (distance < best_distance) {
                best_distance = distance;
                picked = choices[i];
            }
        }
    }

    self.set("direction", picked);
    self.move(kDX[picked], kDY[picked]);

    // 走完再看一眼：是不是撲到小精靈身上了？
    if (Caught(self)) game_state = GameState::kLost;
}

void BuildLevel(GameEngine game) {
    game.clearObjects();
    pellets_left = 0;
    paused = false;

    maze = game.addMaze(level.cols, level.rows);
    maze.setWallImages("wall_iso", "wall_end", "wall_straight", "wall_corner", "wall_tee", "wall_cross");

    const Color ghost_colors[4] = {RED, PINK, SKYBLUE, ORANGE};
    int ghost_count = 0;
    int pacman_x = 0;
    int pacman_y = 0;
    for (int y = 0; y < level.rows; ++y) {
        for (int x = 0; x < level.cols; ++x) {
            const char tile = level.tiles[y][x];
            if (tile == '#') {
                maze.setWall(x, y);
            } else if (tile == '.') {
                GridObject pellet = game.addObject("pellet", nullptr, nullptr, EatPellet);
                pellet.setPosition(x, y);
                pellet.set("type", "pellet");
                pellets_left++;
            } else if (tile == 'P') {
                pacman_x = x;
                pacman_y = y;
            } else if (tile == 'G') {
                GridObject ghost = game.addObject("ghost", nullptr, MoveGhost);
                ghost.setPosition(x, y);
                ghost.setColor(ghost_colors[ghost_count % 4]);
                ghost.set("type", "ghost");
                ghost.set("nextMove", 0.0);
                ghost.set("direction", 0);
                ghost.set("random", ghost_count == 3);
                ghost_count++;
            }
        }
    }

    pacman = game.addObject("pacman", nullptr, MovePacman, PacmanHit);
    pacman.setPosition(pacman_x, pacman_y);
    pacman.set("type", "pacman");
    pacman.set("nextMove", 0.0);
    pacman.set("direction", -1);
    pacman.set("wantedDirection", -1);
}

void UpdateScore(GameEngine, Overlay self) {
    if (game_state != GameState::kPlaying) {
        self.hide();
        return;
    }
    self.setText("Pellets: " + std::to_string(pellets_left));
    self.show();
}

void UpdateStatus(GameEngine, Overlay self) {
    if (game_state == GameState::kPlaying) {
        self.hide();
        return;
    }

    if (game_state == GameState::kStart)
        self.setText("PAC-MAN");
    else if (game_state == GameState::kWon)
        self.setText("YOU WIN!");
    else
        self.setText("GAME OVER");
    self.show();
}

void StartGame(GameEngine, Overlay) { game_state = GameState::kPlaying; }

void RestartGame(GameEngine game, Overlay) {
    BuildLevel(game);
    game_state = GameState::kPlaying;
}

void TogglePause(GameEngine, Overlay) { paused = !paused; }

void UpdateStartButton(GameEngine, Overlay self) {
    if (game_state == GameState::kStart)
        self.show();
    else
        self.hide();
}

void UpdateRestartButton(GameEngine, Overlay self) {
    if (game_state == GameState::kWon || game_state == GameState::kLost)
        self.show();
    else
        self.hide();
}

void UpdatePauseButton(GameEngine, Overlay self) {
    if (game_state != GameState::kPlaying) {
        self.hide();
        return;
    }
    self.setText(paused ? "Resume" : "Pause");
    self.show();
}

}  // namespace

int main() {
    try {
        level = LoadLevelMap("map.txt");

        GameEngine game(level.cols, level.rows, 32);
        game.loadAssets("pacman.db");
        game.setBackgroundColor(BLACK);
        BuildLevel(game);
        game_state = GameState::kStart;

        constexpr int kButtonWidth = 120;
        constexpr int kButtonHeight = 40;
        const int button_x = level.cols * 32 / 2 - kButtonWidth / 2;
        const int button_y = level.rows * 32 / 2;

        game.addTextOverlay("Pellets: 0", 8, 8, 20, YELLOW, nullptr, UpdateScore);
        game.addTextOverlay("PAC-MAN", button_x, button_y - 70, 40, YELLOW, nullptr, UpdateStatus);

        Overlay start = game.addButton("Start", button_x, button_y, kButtonWidth, kButtonHeight, StartGame);
        start.setUpdateFunction(UpdateStartButton);
        Overlay restart = game.addButton("Restart", button_x, button_y, kButtonWidth, kButtonHeight, RestartGame);
        restart.setUpdateFunction(UpdateRestartButton);
        Overlay pause = game.addButton("Pause", level.cols * 32 - 88, 6, 82, 24, TogglePause);
        pause.setUpdateFunction(UpdatePauseButton);

        game.run();
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
