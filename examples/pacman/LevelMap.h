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

// 地圖字元和教學版相同：'#' 牆、'.' 豆子、'P' 小精靈起點、'G' 鬼的起點。
struct LevelMap {
    int rows = 0;
    int cols = 0;
    char tiles[kMaxMapHeight][kMaxMapWidth] = {};
};

inline LevelMap LoadLevelMap(std::istream& input) {
    LevelMap level;
    int pacman_count = 0;
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;

        const int width = static_cast<int>(line.size());
        if (level.rows == 0) level.cols = width;
        if (level.rows == kMaxMapHeight || level.cols > kMaxMapWidth) {
            throw std::runtime_error("Map Error: map must be at most 64x64");
        }
        if (width != level.cols) {
            throw std::runtime_error("Map Error: row " + std::to_string(level.rows) + " has " +
                                     std::to_string(width) + " characters, expected " + std::to_string(level.cols));
        }

        for (int x = 0; x < width; ++x) {
            const char tile = line[x];
            if (tile != '#' && tile != '.' && tile != 'P' && tile != 'G') {
                throw std::runtime_error("Map Error: unknown character '" + std::string(1, tile) + "' at (" +
                                         std::to_string(x) + ", " + std::to_string(level.rows) + ")");
            }
            if (tile == 'P') ++pacman_count;
            level.tiles[level.rows][x] = tile;
        }
        ++level.rows;
    }

    if (level.rows == 0) throw std::runtime_error("Map Error: map is empty");
    if (pacman_count != 1) throw std::runtime_error("Map Error: map must contain exactly one 'P'");
    return level;
}

inline LevelMap LoadLevelMap(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file) throw std::runtime_error("Map Error: cannot open '" + path.string() + "'");
    return LoadLevelMap(file);
}

}  // namespace pacman_example

#endif  // GRID_PLUS_PLUS_EXAMPLES_PACMAN_LEVEL_MAP_H_
