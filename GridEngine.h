/**
 * @file GridEngine.h
 * @brief Grid++ 遊戲引擎。
 *
 * 此檔為核心實作拆分；一般使用者請 include "GridPlusPlus.h"。
 */
#ifndef GRIDENGINE_H
#define GRIDENGINE_H

#include "GridAssetManager.h"
#include "GridObject.h"
#include "Overlay.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include <string>
#include <vector>

// 建立視窗並管理主迴圈、網格物件、覆蓋層與碰撞。
class GridEngine {
public:
    GridEngine(int cols, int rows, int gridSize = 32)
        : cols(cols), rows(rows), gridSize(gridSize) {
        // 材質需要在視窗建立後才能上傳至 GPU。
        InitWindow(cols * gridSize, rows * gridSize, "Grid++ Game");
        SetTargetFPS(60);
    }

    ~GridEngine() { CloseWindow(); }

    void loadAssets(const std::string& dbPath) { assets.load(dbPath); }

    // 背景預設為 RAYWHITE。
    void  setBackgroundColor(Color c) { bgColor = c; }
    Color getBackgroundColor() const  { return bgColor; }

    // 網格線預設關閉。
    void setShowGrid(bool show) { showGrid = show; }
    bool getShowGrid() const    { return showGrid; }

    // 加入物件並呼叫 onStart。
    void spawn(GridObject* obj) {
        obj->setEngine(this);
        objects.push_back(obj);
        obj->onStart();
    }

    void addOverlay(Overlay* overlay) { overlays.push_back(overlay); }

    // 刪除所有遊戲物件，不影響覆蓋層與素材。
    void clearObjects() {
        for (GridObject* o : objects) delete o;
        objects.clear();
    }

    void run() {
#ifdef __EMSCRIPTEN__
        // WebAssembly 主迴圈由瀏覽器排程。
        emscripten_set_main_loop_arg(
            [](void* self) { static_cast<GridEngine*>(self)->tick(); },
            this, 0, 1);
#else
        while (!WindowShouldClose()) tick();
#endif
    }

    int getCols() const { return cols; }
    int getRows() const { return rows; }
    int getGridSize() const { return gridSize; }

    // 在網格座標繪製素材，素材不存在時繪製紅色方塊。
    void drawCell(const std::string& asset, int gx, int gy,
                  int direction = 0, Color tint = WHITE) {
        int px = gx * gridSize, py = gy * gridSize;
        if (!asset.empty() && assets.has(asset)) {
            Texture2D tex = assets.get(asset);
            Rectangle src = { 0, 0, (float)tex.width, (float)tex.height };
            // 以格子中心為旋轉軸。
            Rectangle dst = { (float)px + gridSize / 2.0f, (float)py + gridSize / 2.0f,
                              (float)gridSize, (float)gridSize };
            Vector2 origin = { gridSize / 2.0f, gridSize / 2.0f };
            DrawTexturePro(tex, src, dst, origin, -90.0f * direction, tint);
        } else {
            DrawRectangle(px, py, gridSize, gridSize, RED);
        }
    }

private:
    void tick() {
        // 更新。
        for (GridObject* object : objects) object->onUpdate();
        for (Overlay* overlay : overlays) overlay->onUpdate();

        // 碰撞。
        for (size_t i = 0; i < objects.size(); i++)
            for (size_t j = i + 1; j < objects.size(); j++)
                if (objects[i]->getX() == objects[j]->getX() &&
                    objects[i]->getY() == objects[j]->getY()) {
                    objects[i]->onCollide(objects[j]);
                    objects[j]->onCollide(objects[i]);
                }

        // 繪製。
        BeginDrawing();
        ClearBackground(bgColor);
        if (showGrid) drawGrid();
        for (GridObject* object : objects) object->render(this);
        for (Overlay* overlay : overlays) overlay->draw();
        EndDrawing();
    }

    void drawGrid() {
        for (int x = 0; x <= cols; x++)
            DrawLine(x * gridSize, 0, x * gridSize, rows * gridSize, LIGHTGRAY);
        for (int y = 0; y <= rows; y++)
            DrawLine(0, y * gridSize, cols * gridSize, y * gridSize, LIGHTGRAY);
    }

    int cols, rows, gridSize;
    Color bgColor = RAYWHITE;
    bool showGrid = false;
    GridAssetManager assets;
    std::vector<GridObject*> objects;
    std::vector<Overlay*> overlays;
};

#endif // GRIDENGINE_H
