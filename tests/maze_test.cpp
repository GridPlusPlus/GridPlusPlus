#include <cassert>
#include <limits>
#include <stdexcept>

#include "GridMaze.h"

using gridpp::GridEngine;
using gridpp::GridMaze;
using gridpp::GridObject;

static bool RejectsSize(int width, int height) {
    try {
        GridMaze maze(width, height);
        (void)maze;
        return false;
    } catch (const std::invalid_argument&) {
        return true;
    }
}

int main() {
    assert(RejectsSize(0, 1));
    assert(RejectsSize(1, 0));
    assert(RejectsSize(-1, 1));
    assert(RejectsSize(GridMaze::kMaxWidth + 1, 1));
    assert(RejectsSize(1, GridMaze::kMaxHeight + 1));

    GridMaze maze(3, 2);
    assert(maze.width() == 3);
    assert(maze.height() == 2);
    assert(!maze.IsWall(1, 1));
    assert(maze.IsWall(-1, 0));
    assert(maze.IsWall(3, 0));
    maze.SetWall(1, 1, true);
    assert(maze.IsWall(1, 1));

    bool coordinate_rejected = false;
    try {
        maze.SetWall(3, 1, true);
    } catch (const std::out_of_range&) {
        coordinate_rejected = true;
    }
    assert(coordinate_rejected);

    const int windows_before = test_init_window_calls;
    try {
        GridEngine invalid_game(0, 1);
        (void)invalid_game;
        assert(false);
    } catch (const std::invalid_argument&) {
    }
    assert(test_init_window_calls == windows_before);

    try {
        GridEngine oversized_game(65, 1, 128);
        (void)oversized_game;
        assert(false);
    } catch (const std::invalid_argument&) {
    }
    assert(test_init_window_calls == windows_before);

    try {
        GridEngine overflowing_game(std::numeric_limits<int>::max(), 1, std::numeric_limits<int>::max());
        (void)overflowing_game;
        assert(false);
    } catch (const std::invalid_argument&) {
    }
    assert(test_init_window_calls == windows_before);

    {
        GridEngine maximum_size_game(64, 1, 128);
        assert(test_init_window_calls == windows_before + 1);
    }

    GridEngine game(2, 2);
    game.LoadAssets("examples/pacman/pacman.db");
    GridObject pellet("pellet", 0, 0);
    pellet.Render(&game);
    const unsigned int pellet_texture = test_last_texture_id;
    GridObject cross("wall_cross", 0, 0);
    cross.Render(&game);
    const unsigned int cross_texture = test_last_texture_id;
    assert(pellet_texture != cross_texture);

    GridMaze render_maze(1, 1);
    render_maze.SetWall(0, 0, true);
    render_maze.SetWallTiles("wall_iso", "wall_end", "wall_straight", "wall_corner", "wall_tee", "wall_cross");
    render_maze.SetWallAsset("pellet");
    render_maze.Render(&game);
    assert(test_last_texture_id == pellet_texture);

    render_maze.SetWallTiles("wall_iso", "wall_end", "wall_straight", "wall_corner", "wall_tee", "wall_cross");
    render_maze.Render(&game);
    assert(test_last_texture_id == cross_texture);
}
