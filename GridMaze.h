/**
 * @file GridMaze.h
 * @brief Grid++ 迷宮擴充模組。
 *
 * 提供固定大小的牆面網格與自動拼接繪製。是一個會把整片牆畫出來的 GridObject。
 */
#ifndef GRID_PLUS_PLUS_GRID_MAZE_H_
#define GRID_PLUS_PLUS_GRID_MAZE_H_

#include <stdexcept>
#include <string>
#include <utility>

#include "GridPlusPlus.h"

namespace gridpp {

// 迷宮物件。
class GridMaze : public GridObject {
public:
    static constexpr int kMaxWidth = 64;
    static constexpr int kMaxHeight = 64;

    // 建立空白迷宮，尺寸上限為 64×64。
    GridMaze(int cols, int rows);

    // 界外座標會丟出 std::out_of_range。
    void SetWall(int x, int y, bool wall);

    // 界外座標視為牆。
    bool IsWall(int x, int y) const;

    // 所有牆使用同一素材。
    void SetWallAsset(const std::string& asset);

    // 由六種基本形狀及旋轉方向拼出所有連通組合。
    // 參數依序為孤立、端點、直線、轉角、T 形與十字。
    void SetWallTiles(const std::string& isolated, const std::string& end, const std::string& straight,
                      const std::string& corner, const std::string& tee, const std::string& cross);

    int width() const { return width_; }
    int height() const { return height_; }

    void Render(GridEngine* engine) override;

private:
    // 鄰居是否為牆。上、右、下、左分別使用 bits 0 到 3。
    int ConnectivityMask(int x, int y) const;

    // 把連通情況 mask 轉成「基本形狀索引 + 旋轉方向(0~3)」。
    static std::pair<int, int> ShapeFor(int mask);

    int width_ = 0;
    int height_ = 0;
    bool walls_[kMaxHeight][kMaxWidth] = {};

    std::string single_wall_;
    std::string wall_tiles_[6];
    bool use_tiles_ = false;
};

// Implementation details only below here.

inline GridMaze::GridMaze(int cols, int rows) : GridObject("", -1, -1), width_(cols), height_(rows) {
    if (cols < 1 || rows < 1 || cols > kMaxWidth || rows > kMaxHeight) {
        throw std::invalid_argument("GridMaze Error: dimensions must be between 1x1 and 64x64");
    }
    set_tag("maze");
}

inline void GridMaze::SetWall(int x, int y, bool wall) {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) {
        throw std::out_of_range("GridMaze Error: wall position (" + std::to_string(x) + ", " + std::to_string(y) +
                                ") is outside " + std::to_string(width_) + "x" + std::to_string(height_) + " maze");
    }
    walls_[y][x] = wall;
}

inline bool GridMaze::IsWall(int x, int y) const {
    if (x < 0 || y < 0 || x >= width_ || y >= height_) return true;
    return walls_[y][x];
}

inline void GridMaze::SetWallAsset(const std::string& asset) {
    single_wall_ = asset;
    use_tiles_ = false;
}

inline void GridMaze::SetWallTiles(const std::string& isolated, const std::string& end, const std::string& straight,
                                   const std::string& corner, const std::string& tee, const std::string& cross) {
    wall_tiles_[0] = isolated;
    wall_tiles_[1] = end;
    wall_tiles_[2] = straight;
    wall_tiles_[3] = corner;
    wall_tiles_[4] = tee;
    wall_tiles_[5] = cross;
    use_tiles_ = true;
}

inline void GridMaze::Render(GridEngine* engine) {
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            if (!walls_[y][x]) continue;
            if (use_tiles_) {
                const auto [shape, direction] = ShapeFor(ConnectivityMask(x, y));
                engine->DrawCell(wall_tiles_[shape], x, y, direction);
            } else {
                engine->DrawCell(single_wall_, x, y);
            }
        }
    }
}

inline int GridMaze::ConnectivityMask(int x, int y) const {
    int mask = 0;
    if (IsWall(x, y - 1)) mask |= 1;
    if (IsWall(x + 1, y)) mask |= 2;
    if (IsWall(x, y + 1)) mask |= 4;
    if (IsWall(x - 1, y)) mask |= 8;
    return mask;
}

inline std::pair<int, int> GridMaze::ShapeFor(int mask) {
    static constexpr int kShapes[16] = {0, 1, 1, 3, 1, 2, 3, 4, 1, 3, 2, 4, 3, 4, 4, 5};
    static constexpr int kDirections[16] = {0, 0, 3, 0, 2, 0, 3, 0, 1, 1, 1, 1, 2, 2, 3, 0};
    return {kShapes[mask], kDirections[mask]};
}

}  // namespace gridpp

#endif  // GRID_PLUS_PLUS_GRID_MAZE_H_
