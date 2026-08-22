/** @file GridMaze.h
 *  @brief 定義固定大小的迷宮物件。
 */
#ifndef GRID_PLUS_PLUS_GRID_MAZE_H_
#define GRID_PLUS_PLUS_GRID_MAZE_H_

#include <stdexcept>
#include <string>
#include <utility>

#include "GridPlusPlus.h"

namespace gridpp {

/** 儲存牆面網格並繪製整座迷宮。 */
class GridMaze : public GridObject {
public:
    static constexpr int kMaxWidth = 64;
    static constexpr int kMaxHeight = 64;

    /**
     * 建立空白迷宮。
     * @param cols 迷宮欄數，範圍為 1 到 kMaxWidth。
     * @param rows 迷宮列數，範圍為 1 到 kMaxHeight。
     * @throws std::invalid_argument 若尺寸超出範圍。
     */
    GridMaze(int cols, int rows);

    /**
     * 設定一格是否為牆。
     * @param x 網格 x 座標。
     * @param y 網格 y 座標。
     * @param wall true 表示牆，false 表示通道。
     * @throws std::out_of_range 若座標位於迷宮外。
     */
    void SetWall(int x, int y, bool wall);

    /**
     * @param x 網格 x 座標。
     * @param y 網格 y 座標。
     * @return 指定位置是否為牆；迷宮外一律回傳 true。
     */
    bool IsWall(int x, int y) const;

    /** 所有牆使用同一素材；最後呼叫此函式會切換至單一素材模式。 */
    void SetWallAsset(const std::string& asset);

    /**
     * 設定自動拼接牆面使用的六種素材，並切換至自動拼接模式。
     * @param isolated 孤立牆素材。
     * @param end 端點素材。
     * @param straight 直線素材。
     * @param corner 轉角素材。
     * @param tee T 形素材。
     * @param cross 十字素材。
     */
    void SetWallTiles(const std::string& isolated, const std::string& end, const std::string& straight,
                      const std::string& corner, const std::string& tee, const std::string& cross);

    int width() const;
    int height() const;

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

// Inline definitions

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

inline int GridMaze::width() const { return width_; }

inline int GridMaze::height() const { return height_; }

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
