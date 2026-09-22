#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include "GridPlusPlus.h"
#include "LevelMap.h"

using gridpp::Game;
using gridpp::MazeHandler;
using gridpp::ObjectHandler;
using gridpp::OverlayHandler;
using pacman_example::LevelMap;
using pacman_example::LoadLevelMap;

namespace {

constexpr int kDirectionX[4] = {1, 0, -1, 0};
constexpr int kDirectionY[4] = {0, -1, 0, 1};

enum class GameState {
    kStart,
    kPlaying,
    kWon,
    kLost,
};

GameState game_state = GameState::kStart;
int pellets_left = 0;
bool paused = false;
MazeHandler maze;
ObjectHandler player;
LevelMap level;

int ManhattanDistance(int from_x, int from_y, int to_x, int to_y) {
    return std::abs(from_x - to_x) + std::abs(from_y - to_y);
}

std::string TypeOf(ObjectHandler object) {
    std::string type;
    object.get("type", type);
    return type;
}

void EatPellet(Game, ObjectHandler self, ObjectHandler other) {
    if (TypeOf(other) != "pacman") return;

    self.remove();
    if (--pellets_left <= 0) game_state = GameState::kWon;
}

void MoveGhost(Game game, ObjectHandler self) {
    if (game_state != GameState::kPlaying || paused || !player.exists()) return;

    long long timer = 0;
    self.get("timer", timer);
    ++timer;
    self.set("timer", timer);
    if (timer < 12) return;
    self.set("timer", 0);

    long long current_direction = 0;
    bool random_ghost = false;
    self.get("direction", current_direction);
    self.get("random", random_ghost);

    int options[4];
    int option_count = 0;
    int any[4];
    int any_count = 0;
    for (int direction = 0; direction < 4; ++direction) {
        if (maze.isWall(self.x() + kDirectionX[direction], self.y() + kDirectionY[direction])) continue;
        any[any_count++] = direction;
        if (direction != (current_direction ^ 2)) options[option_count++] = direction;
    }

    int* choices = option_count > 0 ? options : any;
    const int choice_count = option_count > 0 ? option_count : any_count;
    if (choice_count == 0) return;

    int picked = choices[0];
    if (random_ghost) {
        picked = choices[game.random(0, choice_count - 1)];
    } else {
        int best_distance = 1 << 30;
        for (int i = 0; i < choice_count; ++i) {
            const int direction = choices[i];
            const int distance = ManhattanDistance(self.x() + kDirectionX[direction], self.y() + kDirectionY[direction],
                                                   player.x(), player.y());
            if (distance < best_distance) {
                best_distance = distance;
                picked = direction;
            }
        }
    }

    self.set("direction", picked);
    self.move(kDirectionX[picked], kDirectionY[picked]);
}

void MovePlayer(Game game, ObjectHandler self) {
    if (game_state != GameState::kPlaying || paused) return;

    long long wanted_direction = -1;
    long long direction = -1;
    long long timer = 0;
    self.get("wantedDirection", wanted_direction);
    self.get("direction", direction);
    self.get("timer", timer);

    if (game.keyDown(KEY_RIGHT)) wanted_direction = 0;
    if (game.keyDown(KEY_UP)) wanted_direction = 1;
    if (game.keyDown(KEY_LEFT)) wanted_direction = 2;
    if (game.keyDown(KEY_DOWN)) wanted_direction = 3;
    self.set("wantedDirection", wanted_direction);

    ++timer;
    self.set("timer", timer);
    if (timer < 8) return;
    self.set("timer", 0);

    if (wanted_direction >= 0 &&
        !maze.isWall(self.x() + kDirectionX[wanted_direction], self.y() + kDirectionY[wanted_direction])) {
        direction = wanted_direction;
        self.set("direction", direction);
    }
    if (direction >= 0 && !maze.isWall(self.x() + kDirectionX[direction], self.y() + kDirectionY[direction])) {
        self.move(kDirectionX[direction], kDirectionY[direction]);
        self.setDirection(static_cast<int>(direction));
    }
}

void HitPlayer(Game, ObjectHandler, ObjectHandler other) {
    if (TypeOf(other) == "ghost") game_state = GameState::kLost;
}

void BuildLevel(Game game) {
    game.clearObjects();
    pellets_left = 0;
    paused = false;

    maze = game.addMaze(level.cols, level.rows);
    maze.setWallImages("wall_iso", "wall_end", "wall_straight", "wall_corner", "wall_tee", "wall_cross");

    const Color ghost_colors[4] = {RED, PINK, SKYBLUE, ORANGE};
    int ghost_count = 0;
    int player_x = 0;
    int player_y = 0;
    for (int y = 0; y < level.rows; ++y) {
        for (int x = 0; x < level.cols; ++x) {
            const int tile = level.tiles[y][x];
            if (tile == 1) {
                maze.setWall(x, y);
            } else if (tile == 0) {
                ObjectHandler pellet = game.addObject("pellet", nullptr, nullptr, EatPellet);
                pellet.setPosition(x, y);
                pellet.set("type", "pellet");
                ++pellets_left;
            } else if (tile == 2) {
                player_x = x;
                player_y = y;
            } else if (tile == 3) {
                ObjectHandler ghost = game.addObject("ghost", nullptr, MoveGhost);
                ghost.setPosition(x, y);
                ghost.setColor(ghost_colors[ghost_count % 4]);
                ghost.set("type", "ghost");
                ghost.set("timer", 0);
                ghost.set("direction", 0);
                ghost.set("random", ghost_count == 3);
                ++ghost_count;
            }
        }
    }

    player = game.addObject("pacman", nullptr, MovePlayer, HitPlayer);
    player.setPosition(player_x, player_y);
    player.set("type", "pacman");
    player.set("timer", 0);
    player.set("direction", -1);
    player.set("wantedDirection", -1);
}

void UpdateScore(Game, OverlayHandler self) {
    if (game_state != GameState::kPlaying) {
        self.hide();
        return;
    }
    self.setText("Pellets: " + std::to_string(pellets_left));
    self.show();
}

void UpdateStatus(Game, OverlayHandler self) {
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

void StartGame(Game, OverlayHandler) { game_state = GameState::kPlaying; }

void RestartGame(Game game, OverlayHandler) {
    BuildLevel(game);
    game_state = GameState::kPlaying;
}

void TogglePause(Game, OverlayHandler) { paused = !paused; }

void UpdateStartButton(Game, OverlayHandler self) {
    if (game_state == GameState::kStart)
        self.show();
    else
        self.hide();
}

void UpdateRestartButton(Game, OverlayHandler self) {
    if (game_state == GameState::kWon || game_state == GameState::kLost)
        self.show();
    else
        self.hide();
}

void UpdatePauseButton(Game, OverlayHandler self) {
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

        Game game(level.cols, level.rows, 32);
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

        OverlayHandler start = game.addButton("Start", button_x, button_y, kButtonWidth, kButtonHeight, StartGame);
        start.setUpdateFunction(UpdateStartButton);
        OverlayHandler restart =
            game.addButton("Restart", button_x, button_y, kButtonWidth, kButtonHeight, RestartGame);
        restart.setUpdateFunction(UpdateRestartButton);
        OverlayHandler pause = game.addButton("Pause", level.cols * 32 - 88, 6, 82, 24, TogglePause);
        pause.setUpdateFunction(UpdatePauseButton);

        game.run();
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
