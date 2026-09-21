/** @file GridEngine.h
 *  @brief 定義 Grid++ 遊戲引擎。
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

namespace detail {
class GameState;
}

/** 建立視窗並管理遊戲物件、碰撞、繪製與主迴圈。 */
class GridEngine {
public:
    static constexpr int kMaxWindowSize = 8192;

    /**
     * 建立遊戲引擎與視窗。
     * @param cols 網格欄數。
     * @param rows 網格列數。
     * @param grid_size 每格的像素寬度與高度。
     * @throws std::invalid_argument 若任一參數小於 1，或視窗寬、高超過 kMaxWindowSize。
     */
    GridEngine(int cols, int rows, int grid_size = 32);
    ~GridEngine();

    GridEngine(const GridEngine& other) = delete;
    GridEngine& operator=(const GridEngine& other) = delete;

    /**
     * 載入素材包並取代目前素材。
     * @param database_path 素材包路徑。
     * @throws std::runtime_error 若檔案無法讀取或素材包格式無效。
     */
    void LoadAssets(const std::filesystem::path& database_path);

    void set_background_color(Color color);
    Color background_color() const;

    void set_show_grid(bool show);
    bool show_grid() const;

    int cols() const;
    int rows() const;
    int grid_size() const;

    /**
     * 將物件加入遊戲並接管其所有權。
     *
     * 主迴圈外會立即呼叫 GridObject::OnSpawn()。主迴圈內加入的物件會在幀末呼叫 OnSpawn()，並從
     * 下一幀開始更新、碰撞與繪製。
     *
     * @param object 使用 `new` 建立且尚未屬於任何引擎的物件。
     * @return 指向已加入物件的借用指標；呼叫端不可 delete。
     * @throws std::invalid_argument 若 object 是 nullptr。
     * @throws std::logic_error 若 object 已屬於一個引擎。
     */
    GridObject* Spawn(GridObject* object);

    /**
     * 建立並加入函式版物件。
     * @param asset_name 繪製時使用的素材名稱。
     * @param x 初始網格 x 座標。
     * @param y 初始網格 y 座標。
     * @param update 每幀呼叫的函式；可以是 nullptr。
     * @param collide 發生碰撞時呼叫的函式；可以是 nullptr。
     * @return 指向已加入物件的借用指標；呼叫端不可 delete。
     */
    GridObject* Spawn(const std::string& asset_name, int x, int y, CallbackGridObject::UpdateFn update,
                      CallbackGridObject::CollideFn collide = nullptr);

    /**
     * 刪除引擎擁有的物件。
     *
     * 主迴圈內呼叫時，物件會立即停止更新、碰撞與繪製，並在幀末釋放。
     * @param object 要刪除的借用指標；nullptr 或不屬於此引擎的指標不會產生效果。
     */
    void Destroy(GridObject* object);

    /** 刪除所有遊戲物件；不影響 Overlay 與已載入的素材。 */
    void ClearObjects();

    /**
     * 加入畫面覆蓋層並接管其所有權。
     *
     * 主迴圈外會立即呼叫 Overlay::OnSpawn()。主迴圈內加入的 Overlay 會在幀末呼叫
     * OnSpawn()，並從下一幀開始更新與繪製。
     *
     * @param overlay 使用 `new` 建立且尚未屬於任何引擎的 Overlay。
     * @throws std::invalid_argument 若 overlay 是 nullptr。
     * @throws std::logic_error 若 overlay 已屬於一個引擎。
     */
    void AddOverlay(Overlay* overlay);

    /** 刪除引擎擁有的 Overlay；主迴圈內呼叫時延後至幀末釋放。 */
    void DestroyOverlay(Overlay* overlay);

    /** 刪除所有 Overlay；不影響遊戲物件與已載入的素材。 */
    void ClearOverlays();

    /** 執行遊戲主迴圈，直到視窗關閉。 */
    void Run();

    /**
     * 在指定網格座標繪製素材；素材不存在時繪製紅色方塊。
     * @param asset_name 素材名稱。
     * @param grid_x 網格 x 座標。
     * @param grid_y 網格 y 座標。
     * @param direction 逆時針旋轉的 90 度倍數。
     * @param tint 繪製時套用的顏色。
     */
    void DrawCell(const std::string& asset_name, int grid_x, int grid_y, int direction = 0, Color tint = WHITE);

private:
    friend class detail::GameState;

    using AfterTickFunction = void (*)(void* context);

    void SetAfterTickFunction(AfterTickFunction function, void* context);
    void Tick();
    bool IsPendingDestroy(GridObject* object) const;
    bool IsPendingDestroy(Overlay* overlay) const;
    void InvokeOnSpawn(GridObject* object);
    void InvokeOnSpawn(Overlay* overlay);
    void FlushLifecycleChanges();
    void DrawGrid();
    void DeleteAllObjects() noexcept;
    void DeleteAllOverlays() noexcept;

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
    std::vector<Overlay*> overlays_to_add_;
    std::unordered_set<Overlay*> overlays_to_destroy_;

    AfterTickFunction after_tick_function_ = nullptr;
    void* after_tick_context_ = nullptr;
};

// Inline definitions

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
    DeleteAllOverlays();
    // raylib 的 Texture 必須在 OpenGL context 關閉前釋放。
    assets_.Clear();
    CloseWindow();
}

inline void GridEngine::LoadAssets(const std::filesystem::path& database_path) { assets_.Load(database_path); }

inline void GridEngine::set_background_color(Color color) { background_color_ = color; }

inline Color GridEngine::background_color() const { return background_color_; }

inline void GridEngine::set_show_grid(bool show) { show_grid_ = show; }

inline bool GridEngine::show_grid() const { return show_grid_; }

inline int GridEngine::cols() const { return cols_; }

inline int GridEngine::rows() const { return rows_; }

inline int GridEngine::grid_size() const { return grid_size_; }

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
    overlay->engine_ = this;

    try {
        if (ticking_)
            overlays_to_add_.push_back(overlay);
        else
            overlays_.push_back(overlay);
    } catch (...) {
        delete overlay;
        throw;
    }

    if (!ticking_) InvokeOnSpawn(overlay);
}

inline void GridEngine::DestroyOverlay(Overlay* overlay) {
    if (overlay == nullptr) return;
    if (ticking_) {
        overlays_to_destroy_.insert(overlay);
        return;
    }

    const auto it = std::find(overlays_.begin(), overlays_.end(), overlay);
    if (it != overlays_.end()) {
        delete *it;
        overlays_.erase(it);
    }
}

inline void GridEngine::ClearOverlays() {
    if (ticking_) {
        overlays_to_destroy_.insert(overlays_.begin(), overlays_.end());
        overlays_to_destroy_.insert(overlays_to_add_.begin(), overlays_to_add_.end());
        return;
    }
    DeleteAllOverlays();
}

inline void GridEngine::SetAfterTickFunction(AfterTickFunction function, void* context) {
    after_tick_function_ = function;
    after_tick_context_ = context;
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
    for (std::size_t i = 0; i < overlay_count; ++i) {
        if (!IsPendingDestroy(overlays_[i])) overlays_[i]->OnUpdate();
    }

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
    for (std::size_t i = 0; i < overlay_count; ++i) {
        if (!IsPendingDestroy(overlays_[i])) overlays_[i]->Draw();
    }
    EndDrawing();

    ticking_ = false;
    FlushLifecycleChanges();
    if (after_tick_function_ != nullptr) after_tick_function_(after_tick_context_);
}

inline bool GridEngine::IsPendingDestroy(GridObject* object) const { return objects_to_destroy_.count(object) != 0; }

inline bool GridEngine::IsPendingDestroy(Overlay* overlay) const { return overlays_to_destroy_.count(overlay) != 0; }

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

inline void GridEngine::InvokeOnSpawn(Overlay* overlay) {
    try {
        overlay->OnSpawn();
    } catch (...) {
        const auto it = std::find(overlays_.begin(), overlays_.end(), overlay);
        if (it != overlays_.end()) {
            delete *it;
            overlays_.erase(it);
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

    overlays_.erase(std::remove_if(overlays_.begin(), overlays_.end(),
                                   [&](Overlay* overlay) {
                                       if (!IsPendingDestroy(overlay)) return false;
                                       delete overlay;
                                       return true;
                                   }),
                    overlays_.end());

    overlays_to_add_.erase(std::remove_if(overlays_to_add_.begin(), overlays_to_add_.end(),
                                          [&](Overlay* overlay) {
                                              if (!IsPendingDestroy(overlay)) return false;
                                              delete overlay;
                                              return true;
                                          }),
                           overlays_to_add_.end());
    overlays_to_destroy_.clear();

    while (!overlays_to_add_.empty()) {
        Overlay* overlay = overlays_to_add_.front();
        overlays_.push_back(overlay);
        overlays_to_add_.erase(overlays_to_add_.begin());
        InvokeOnSpawn(overlay);
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

inline void GridEngine::DeleteAllOverlays() noexcept {
    for (Overlay* overlay : overlays_) delete overlay;
    for (Overlay* overlay : overlays_to_add_) delete overlay;
    overlays_.clear();
    overlays_to_add_.clear();
    overlays_to_destroy_.clear();
}

}  // namespace gridpp

#endif  // GRID_PLUS_PLUS_GRID_ENGINE_H_
