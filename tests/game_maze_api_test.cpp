#include <cassert>

#include "GridPlusPlus.h"

using gridpp::Game;
using gridpp::MazeHandler;

namespace {

int maze_updates = 0;

void InitMaze(Game, MazeHandler self) {
    self.setWall(0, 0);
    self.setWallImage("wall");
}

void UpdateMaze(Game, MazeHandler) { ++maze_updates; }

}  // namespace

int main() {
    Game game(3, 2);
    MazeHandler maze = game.addMaze(3, 2, InitMaze, UpdateMaze);

    assert(maze.width() == 3);
    assert(maze.height() == 2);
    assert(!maze.isWall(0, 0));
    assert(maze.isWall(-1, 0));

    game.run();

    assert(maze.isWall(0, 0));
    assert(maze_updates == 2);

    MazeHandler copy = maze.deepCopy();
    copy.setWall(1, 1);
    assert(!maze.isWall(1, 1));
    assert(copy.isWall(1, 1));

    maze.remove();
    assert(!maze.exists());
    assert(copy.exists());

    game.clearObjects();
    assert(!copy.exists());
}
