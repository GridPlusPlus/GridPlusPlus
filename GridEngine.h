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

#include <algorithm>
#include <string>
#include <unordered_set>
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

    // 物件生命週期：
    // - 主迴圈外的 spawn / destroy / clearObjects 立即生效。
    // - 主迴圈內 spawn 的物件從下一幀開始運作。
    // - 主迴圈內 destroy / clearObjects 會立即停用物件，並在幀末釋放記憶體。
    GridObject* spawn(GridObject* obj) {
        obj->setEngine(this);
        if (ticking) {
            objectsToSpawn.push_back(obj);
        } else {
            objects.push_back(obj);
            obj->onStart();
        }
        return obj;
    }

    // 建立函式版物件；呼叫端不需要知道 CallbackGridObject。
    GridObject* spawn(const std::string& asset, int x, int y,
                      void (*update)(GridObject*),
                      void (*collide)(GridObject*, GridObject*) = nullptr) {
        return spawn(new CallbackGridObject(asset, x, y, update, collide));
    }

    // 立即停止物件的更新、碰撞與繪製，並在這一幀結束後才釋放記憶體。
    void destroy(GridObject* object) {
        if (!object) return;
        if (ticking) {
            objectsToDestroy.insert(object);
            return;
        }

        std::vector<GridObject*>::iterator it =
            std::find(objects.begin(), objects.end(), object);
        if (it != objects.end()) {
            delete *it;
            objects.erase(it);
        }
    }

    void addOverlay(Overlay* overlay) { overlays.push_back(overlay); }

    // 刪除所有遊戲物件，不影響覆蓋層與素材。
    void clearObjects() {
        if (ticking) {
            objectsToDestroy.insert(objects.begin(), objects.end());
            objectsToDestroy.insert(objectsToSpawn.begin(), objectsToSpawn.end());
            return;
        }
        for (GridObject* o : objects) delete o;
        for (GridObject* o : objectsToSpawn) delete o;
        objects.clear();
        objectsToSpawn.clear();
        objectsToDestroy.clear();
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
        ticking = true;

        // 更新
        for (GridObject* object : objects)
            if (!isPendingDestroy(object)) object->onUpdate();
        for (Overlay* overlay : overlays) overlay->onUpdate();

        // 碰撞
        for (size_t i = 0; i < objects.size(); i++) {
            if (isPendingDestroy(objects[i]) || !objects[i]->isVisible()) continue;
            for (size_t j = i + 1; j < objects.size(); j++) {
                if (isPendingDestroy(objects[j]) || !objects[j]->isVisible()) continue;
                if (objects[i]->getX() == objects[j]->getX() &&
                    objects[i]->getY() == objects[j]->getY()) {
                    objects[i]->onCollide(objects[j]);
                    if (isPendingDestroy(objects[i]) || !objects[i]->isVisible()) break;
                    if (isPendingDestroy(objects[j]) || !objects[j]->isVisible()) continue;
                    objects[j]->onCollide(objects[i]);
                    if (isPendingDestroy(objects[i]) || !objects[i]->isVisible()) break;
                }
            }
        }

        // 繪製
        BeginDrawing();
        ClearBackground(bgColor);
        if (showGrid) drawGrid();
        for (GridObject* object : objects)
            if (!isPendingDestroy(object) && object->isVisible()) object->render(this);
        for (Overlay* overlay : overlays) overlay->draw();
        EndDrawing();

        ticking = false;
        flushLifecycleChanges();
    }

    bool isPendingDestroy(GridObject* object) const {
        return objectsToDestroy.count(object) != 0;
    }

    void flushLifecycleChanges() {
        objects.erase(
            std::remove_if(objects.begin(), objects.end(),
                [&](GridObject* object) {
                    if (!isPendingDestroy(object)) return false;
                    delete object;
                    return true;
                }),
            objects.end());

        std::vector<GridObject*> spawned;
        spawned.swap(objectsToSpawn);
        for (GridObject* object : spawned) {
            if (isPendingDestroy(object)) {
                delete object;
            } else {
                objects.push_back(object);
                object->onStart();
            }
        }
        objectsToDestroy.clear();
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
    std::vector<GridObject*> objectsToSpawn;
    std::vector<Overlay*> overlays;
    std::unordered_set<GridObject*> objectsToDestroy;
    bool ticking = false;
};

#endif // GRIDENGINE_H
