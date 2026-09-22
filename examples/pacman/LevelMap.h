#ifndef GRID_PLUS_PLUS_EXAMPLES_PACMAN_LEVEL_MAP_H_
#define GRID_PLUS_PLUS_EXAMPLES_PACMAN_LEVEL_MAP_H_

#include <filesystem>
#include <fstream>
#include <istream>
#include <stdexcept>
#include <string>

namespace pacman_example {

constexpr int kMaxMapWidth = 64;
constexpr int kMaxMapHeight = 64;

// Pacman map tiles: 0=pellet, 1=wall, 2=player, 3=ghost.
struct LevelMap {
    int rows = 0;
    int cols = 0;
    int tiles[kMaxMapHeight][kMaxMapWidth] = {};
};

inline LevelMap LoadLevelMap(std::istream& input) {
    LevelMap level;
    if (!(input >> level.rows >> level.cols)) {
        throw std::runtime_error("Map Error: first line must contain rows and columns");
    }
    if (level.rows < 1 || level.cols < 1 || level.rows > kMaxMapHeight || level.cols > kMaxMapWidth) {
        throw std::runtime_error("Map Error: dimensions must be between 1x1 and 64x64");
    }

    int player_count = 0;
    for (int y = 0; y < level.rows; ++y) {
        for (int x = 0; x < level.cols; ++x) {
            int& tile = level.tiles[y][x];
            if (!(input >> tile)) {
                throw std::runtime_error("Map Error: missing or invalid tile at (" + std::to_string(x) + ", " +
                                         std::to_string(y) + ")");
            }
            if (tile < 0 || tile > 3) {
                throw std::runtime_error("Map Error: tile at (" + std::to_string(x) + ", " + std::to_string(y) +
                                         ") must be 0, 1, 2, or 3");
            }
            if (tile == 2) ++player_count;
        }
    }

    input >> std::ws;
    if (!input.eof()) throw std::runtime_error("Map Error: extra data after the declared grid");
    if (player_count != 1) throw std::runtime_error("Map Error: map must contain exactly one player tile (2)");
    return level;
}

inline LevelMap LoadLevelMap(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Map Error: cannot open '" + path.string() + "'");
    return LoadLevelMap(file);
}

}  // namespace pacman_example

#endif  // GRID_PLUS_PLUS_EXAMPLES_PACMAN_LEVEL_MAP_H_
