/**
 * @file GridMaze.h
 * @brief Grid++ 迷宮擴充模組。
 *
 * 提供固定大小的牆面網格與自動拼接繪製。是一個會把整片牆畫出來的 GridObject。
 */
#ifndef GRIDMAZE_H
#define GRIDMAZE_H

#include "GridPlusPlus.h"

// 迷宮物件
class GridMaze : public GridObject {
public:
    // 建立空白迷宮，尺寸上限為 64×64。
    GridMaze(int cols, int rows) {
        gridX = -1; gridY = -1;  // 放到畫面外，避開引擎的同格碰撞
        tag = "maze";
        width  = (cols <= MAX_W) ? cols : MAX_W;   // 夾在陣列容量內，避免超出
        height = (rows <= MAX_H) ? rows : MAX_H;
        for (int y = 0; y < height; y++)           // 一開始全部設為空地
            for (int x = 0; x < width; x++)
                walls[y][x] = false;
    }

    // 界外座標不處理。
    void setWall(int x, int y, bool wall) {
        if (x >= 0 && y >= 0 && x < width && y < height) walls[y][x] = wall;
    }

    // 所有牆使用同一素材。
    void setWallAsset(const std::string& asset) { singleWall = asset; }

    // 由六種基本形狀及旋轉方向拼出所有連通組合。
    // 參數依序為孤立、端點、直線、轉角、T 形與十字。
    void setWallTiles(const std::string& iso, const std::string& end,
                      const std::string& straight, const std::string& corner,
                      const std::string& tee, const std::string& cross) {
        wallTiles[0] = iso;    wallTiles[1] = end;  wallTiles[2] = straight;
        wallTiles[3] = corner; wallTiles[4] = tee;  wallTiles[5] = cross;
        useTiles = true;
    }

    // 界外座標視為牆。
    bool isWall(int x, int y) const {
        if (x < 0 || y < 0 || x >= width || y >= height) return true;
        return walls[y][x];
    }

    int getWidth()  const { return width; }
    int getHeight() const { return height; }

    void render(GridEngine* engine) override {
        for (int y = 0; y < height; y++)
            for (int x = 0; x < width; x++) {
                if (!walls[y][x]) continue;
                if (useTiles) {
                    // 自動拼接：選基本形狀 + 旋轉
                    int shape, dir;
                    shapeFor(connectivityMask(x, y), shape, dir);
                    engine->drawCell(wallTiles[shape], x, y, dir);
                } else {
                    engine->drawCell(singleWall, x, y);
                }
            }
    }

private:
    // 鄰居是否為牆。上、右、下、左分別使用 bits 0 到 3。
    int connectivityMask(int x, int y) const {
        int m = 0;
        if (isWall(x, y - 1)) m |= 1;
        if (isWall(x + 1, y)) m |= 2;
        if (isWall(x, y + 1)) m |= 4;
        if (isWall(x - 1, y)) m |= 8;
        return m;
    }

    // 把連通情況 mask 轉成「基本形狀索引 + 旋轉方向(0~3)」。
    // 形狀：0=孤立 1=端點 2=直線 3=轉角 4=T形 5=十字。
    // 例如各種「端點」都用同一張端點素材，只是旋轉到不同方向。
    static void shapeFor(int mask, int& shape, int& dir) {
        static const int SHAPE[16] = { 0,1,1,3,1,2,3,4,1,3,2,4,3,4,4,5 };
        static const int DIR[16]   = { 0,0,3,0,2,0,3,0,1,1,1,1,2,2,3,0 };
        shape = SHAPE[mask];
        dir   = DIR[mask];
    }

    static const int MAX_W = 64, MAX_H = 64; // 地圖上限（32px 下遠超螢幕容量）
    bool walls[MAX_H][MAX_W];                // walls[y][x]，true 表示牆壁
    int  width = 0, height = 0;
    std::string singleWall;
    std::string wallTiles[6];
    bool useTiles = false;
};

#endif // GRIDMAZE_H
