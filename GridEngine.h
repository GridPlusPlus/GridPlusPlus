/**
 * @file GridEngine.h
 * @brief Grid++ 遊戲引擎。
 *
 * 此檔為核心實作拆分；一般使用者請 include "GridPlusPlus.h"。
 */
#ifndef GRID_PLUS_PLUS_GRID_ENGINE_H_
#define GRID_PLUS_PLUS_GRID_ENGINE_H_

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include "GridAssetManager.h"
#include "GridObject.h"
#include "Overlay.h"
#include "raylib.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace gridpp {

// 建立視窗並管理主迴圈、網格物件、覆蓋層與碰撞。
class GridEngine {
public:
    static constexpr int kMaxWindowSize = 8192;

    GridEngine(int cols, int rows, int grid_size = 32);
    ~GridEngine();

    GridEngine(const GridEngine&) = delete;
    GridEngine& operator=(const GridEngine&) = delete;

    void LoadAssets(const std::filesystem::path& database_path) { assets_.Load(database_path); }

    // 背景預設為 RAYWHITE。
    void set_background_color(Color color) { background_color_ = color; }
    Color background_color() const { return background_color_; }

    // 網格線預設關閉。
    void set_show_grid(bool show) { show_grid_ = show; }
    bool show_grid() const { return show_grid_; }

    int cols() const { return cols_; }
    int rows() const { return rows_; }
    int grid_size() const { return grid_size_; }

    // 物件生命週期：
    // - Spawn 後由引擎擁有；回傳指標只能借用，不可自行 delete。
    // - 主迴圈外的 Spawn / Destroy / ClearObjects 立即生效。
    // - 主迴圈內 Spawn 的物件從下一幀開始運作。
    // - 主迴圈內 Destroy / ClearObjects 會立即停用物件，並在幀末釋放記憶體。
    GridObject* Spawn(GridObject* object);

    // 建立函式版物件；呼叫端不需要知道 CallbackGridObject。
    GridObject* Spawn(const std::string& asset_name, int x, int y, CallbackGridObject::UpdateFn update,
                      CallbackGridObject::CollideFn collide = nullptr);

    // 立即停止物件的更新、碰撞與繪製，並在這一幀結束後才釋放記憶體。
    void Destroy(GridObject* object);

    // 刪除所有遊戲物件，不影響覆蓋層與素材。
    void ClearObjects();

    // 呼叫後由引擎擁有 overlay。
    void AddOverlay(Overlay* overlay);

    void Run();

    // 在網格座標繪製素材，素材不存在時繪製紅色方塊。
    void DrawCell(const std::string& asset_name, int grid_x, int grid_y, int direction = 0, Color tint = WHITE);

private:
    void Tick();
    void InvokeOnSpawn(GridObject* object);
    bool IsPendingDestroy(GridObject* object) const;
    void FlushLifecycleChanges();
    void DrawGrid();
    void DeleteAllObjects() noexcept;
    void ClearOverlays() noexcept;

    int cols_;
    int rows_;
    int grid_size_;
    Color background_color_ = RAYWHITE;
    bool show_grid_ = false;

    GridAssetManager assets_;

    std::vector<GridObject*> objects_;
    std::vector<GridObject*> objects_to_spawn_;
    std::unordered_set<GridObject*> objects_to_destroy_;
    bool ticking_ = false;

    std::vector<Overlay*> overlays_;
};

// Implementation details only below here.

inline GridEngine::GridEngine(int cols, int rows, int grid_size) : cols_(cols), rows_(rows), grid_size_(grid_size) {
    if (cols < 1 || rows < 1 || grid_size < 1) {
        throw std::invalid_argument("GridEngine Error: cols, rows, and grid size must be greater than 0");
    }

    const std::int64_t window_width = static_cast<std::int64_t>(cols_) * grid_size_;
    const std::int64_t window_height = static_cast<std::int64_t>(rows_) * grid_size_;
    if (window_width > kMaxWindowSize || window_height > kMaxWindowSize) {
        throw std::invalid_argument("GridEngine Error: window width and height must not exceed 8192 pixels");
    }

    // 材質需要在視窗建立後才能上傳至 GPU。
    InitWindow(static_cast<int>(window_width), static_cast<int>(window_height), "Grid++ Game");
    SetTargetFPS(60);
}

inline GridEngine::~GridEngine() {
    DeleteAllObjects();
    ClearOverlays();
    // raylib 的 Texture 必須在 OpenGL context 關閉前釋放。
    assets_.Clear();
    CloseWindow();
}

// spawn for user-created object
inline GridObject* GridEngine::Spawn(GridObject* object) {
    if (object == nullptr) throw std::invalid_argument("GridEngine Error: Cannot Spawn nullptr GridObject");
    if (object->engine() != nullptr)
        throw std::logic_error("GridEngine Error: Same GridObject cannot be Spawned twice");
    object->set_engine(this);

    try {
        if (ticking_) {
            objects_to_spawn_.push_back(object);
        } else {
            objects_.push_back(object);
        }
    } catch (...) {
        delete object;
        throw;
    }

    if (!ticking_) InvokeOnSpawn(object);
    return object;
}

// spawn for callback object; user doesn't need to know about CallbackGridObject
inline GridObject* GridEngine::Spawn(const std::string& asset_name, int x, int y, CallbackGridObject::UpdateFn update,
                                     CallbackGridObject::CollideFn collide) {
    return Spawn(new CallbackGridObject(asset_name, x, y, update, collide));
}

inline void GridEngine::Destroy(GridObject* object) {
    if (object == nullptr) return;
    if (ticking_) {
        objects_to_destroy_.insert(object);
        return;
    }

    const auto it = std::find(objects_.begin(), objects_.end(), object);
    if (it != objects_.end()) {
        delete *it;
        objects_.erase(it);
    }
}

inline void GridEngine::ClearObjects() {
    if (ticking_) {
        objects_to_destroy_.insert(objects_.begin(), objects_.end());
        objects_to_destroy_.insert(objects_to_spawn_.begin(), objects_to_spawn_.end());
        return;
    }
    DeleteAllObjects();
}

inline void GridEngine::AddOverlay(Overlay* overlay) {
    if (overlay == nullptr) throw std::invalid_argument("Grid++ 錯誤：不能加入 nullptr Overlay");
    if (overlay->engine_ != nullptr) throw std::logic_error("Grid++ 錯誤：同一個 Overlay 不能加入兩次");
    overlays_.push_back(overlay);
    overlay->engine_ = this;
}

inline void GridEngine::Run() {
#ifdef __EMSCRIPTEN__
    // WebAssembly 主迴圈由瀏覽器排程。
    emscripten_set_main_loop_arg([](void* self) { static_cast<GridEngine*>(self)->Tick(); }, this, 0, 1);
#else
    while (!WindowShouldClose()) Tick();
#endif
}

inline void GridEngine::DrawCell(const std::string& asset_name, int grid_x, int grid_y, int direction, Color tint) {
    const int pixel_x = grid_x * grid_size_;
    const int pixel_y = grid_y * grid_size_;
    if (!asset_name.empty() && assets_.Has(asset_name)) {
        const Texture2D texture = assets_.Get(asset_name);
        const Rectangle source = {0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)};
        // 以格子中心為旋轉軸。
        const Rectangle destination = {static_cast<float>(pixel_x) + grid_size_ / 2.0f,
                                       static_cast<float>(pixel_y) + grid_size_ / 2.0f, static_cast<float>(grid_size_),
                                       static_cast<float>(grid_size_)};
        const Vector2 origin = {grid_size_ / 2.0f, grid_size_ / 2.0f};
        DrawTexturePro(texture, source, destination, origin, -90.0f * direction, tint);
    } else {
        DrawRectangle(pixel_x, pixel_y, grid_size_, grid_size_, RED);
    }
}

inline void GridEngine::Tick() {
    ticking_ = true;
    // 執行期間加入的 Overlay 從下一幀開始更新與繪製。
    const std::size_t overlay_count = overlays_.size();

    // 更新
    for (GridObject* object : objects_) {
        if (!IsPendingDestroy(object)) object->OnUpdate();
    }
    for (std::size_t i = 0; i < overlay_count; ++i) overlays_[i]->OnUpdate();

    // 碰撞
    const auto can_collide = [&](GridObject* object) {
        return !IsPendingDestroy(object) && object->visible() && object->x() >= 0 && object->x() < cols_ &&
               object->y() >= 0 && object->y() < rows_;
    };
    for (std::size_t i = 0; i < objects_.size(); ++i) {
        if (!can_collide(objects_[i])) continue;
        for (std::size_t j = i + 1; j < objects_.size(); ++j) {
            if (!can_collide(objects_[j])) continue;
            if (objects_[i]->x() == objects_[j]->x() && objects_[i]->y() == objects_[j]->y()) {
                objects_[i]->OnCollide(objects_[j]);
                if (!can_collide(objects_[i])) break;
                if (!can_collide(objects_[j])) continue;
                objects_[j]->OnCollide(objects_[i]);
                if (!can_collide(objects_[i])) break;
            }
        }
    }

    // 繪製
    BeginDrawing();
    ClearBackground(background_color_);
    if (show_grid_) DrawGrid();
    std::vector<GridObject*> draw_order = objects_;
    std::stable_sort(draw_order.begin(), draw_order.end(), [](const GridObject* left, const GridObject* right) {
        return left->z_index() < right->z_index();
    });
    for (GridObject* object : draw_order) {
        if (!IsPendingDestroy(object) && object->visible()) object->Render(this);
    }
    for (std::size_t i = 0; i < overlay_count; ++i) overlays_[i]->Draw();
    EndDrawing();

    ticking_ = false;
    FlushLifecycleChanges();
}

inline bool GridEngine::IsPendingDestroy(GridObject* object) const { return objects_to_destroy_.count(object) != 0; }

inline void GridEngine::InvokeOnSpawn(GridObject* object) {
    try {
        object->OnSpawn();
    } catch (...) {
        const auto it = std::find(objects_.begin(), objects_.end(), object);
        if (it != objects_.end()) {
            delete *it;
            objects_.erase(it);
        }
        throw;
    }
}

inline void GridEngine::FlushLifecycleChanges() {
    objects_.erase(std::remove_if(objects_.begin(), objects_.end(),
                                  [&](GridObject* object) {
                                      if (!IsPendingDestroy(object)) return false;
                                      delete object;
                                      return true;
                                  }),
                   objects_.end());

    objects_to_spawn_.erase(std::remove_if(objects_to_spawn_.begin(), objects_to_spawn_.end(),
                                           [&](GridObject* object) {
                                               if (!IsPendingDestroy(object)) return false;
                                               delete object;
                                               return true;
                                           }),
                            objects_to_spawn_.end());
    objects_to_destroy_.clear();

    while (!objects_to_spawn_.empty()) {
        GridObject* object = objects_to_spawn_.front();
        objects_.push_back(object);
        objects_to_spawn_.erase(objects_to_spawn_.begin());
        InvokeOnSpawn(object);
    }
}

inline void GridEngine::DrawGrid() {
    for (int x = 0; x <= cols_; ++x) DrawLine(x * grid_size_, 0, x * grid_size_, rows_ * grid_size_, LIGHTGRAY);
    for (int y = 0; y <= rows_; ++y) DrawLine(0, y * grid_size_, cols_ * grid_size_, y * grid_size_, LIGHTGRAY);
}

inline void GridEngine::DeleteAllObjects() noexcept {
    for (GridObject* object : objects_) delete object;
    for (GridObject* object : objects_to_spawn_) delete object;
    objects_.clear();
    objects_to_spawn_.clear();
    objects_to_destroy_.clear();
}

inline void GridEngine::ClearOverlays() noexcept {
    for (Overlay* overlay : overlays_) delete overlay;
    overlays_.clear();
}

}  // namespace gridpp

#endif  // GRID_PLUS_PLUS_GRID_ENGINE_H_
