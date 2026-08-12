/**
 * @file GridObject.h
 * @brief Grid++ 網格物件基底類別。
 *
 * 此檔為核心實作拆分；一般使用者請 include "GridPlusPlus.h"。
 */
#ifndef GRID_PLUS_PLUS_GRID_OBJECT_H_
#define GRID_PLUS_PLUS_GRID_OBJECT_H_

#include <string>
#include <utility>

#include "raylib.h"

namespace gridpp {

class GridEngine;

// 遊戲世界中的網格物件。
// 繼承它並覆寫 OnSpawn / OnUpdate / OnCollide。詳見 docs/guide/game-objects.md。
class GridObject {
public:
    GridObject() = default;
    GridObject(std::string asset_name, int x, int y);
    GridObject(const GridObject& other);
    GridObject& operator=(const GridObject&) = delete;
    virtual ~GridObject() = default;

    // 引擎呼叫的生命週期函式。
    virtual void OnSpawn() {}
    virtual void OnUpdate() {}
    virtual void OnCollide(GridObject* other) { (void)other; }

    // 把自己畫出來；預設繪製 asset_name，子類別可覆寫。
    // 因為實作需要完整的 GridEngine 定義，所以放在 GridPlusPlus.h 最後。
    virtual void Render(GridEngine* engine);

    int x() const { return grid_x_; }
    int y() const { return grid_y_; }
    void set_x(int x) { grid_x_ = x; }
    void set_y(int y) { grid_y_ = y; }
    void Move(int dx, int dy);

    const std::string& asset_name() const { return asset_name_; }
    void set_asset_name(const std::string& asset_name) { asset_name_ = asset_name; }

    // 身分標記，碰撞時分辨對方。
    const std::string& tag() const { return tag_; }
    void set_tag(const std::string& tag) { tag_ = tag; }

    // 方向為 0 到 3，每增加 1 逆時針旋轉 90 度。
    int direction() const { return direction_; }
    void set_direction(int direction) { direction_ = ((direction % 4) + 4) % 4; }

    // 調色。繪製時把素材整體乘上這個顏色。預設 WHITE（不改變顏色）。
    Color tint() const { return tint_; }
    void set_tint(Color tint) { tint_ = tint; }

    // 繪製層級；數值越大越晚繪製。同層級維持 Spawn 順序。
    int z_index() const { return z_index_; }
    void set_z_index(int z_index) { z_index_ = z_index; }

    // 隱藏時仍會更新，但不會繪製或參與碰撞。
    bool visible() const { return visible_; }
    void set_visible(bool visible) { visible_ = visible; }

    GridEngine* engine() const { return engine_; }

private:
    friend class GridEngine;

    void set_engine(GridEngine* engine) { engine_ = engine; }

    int grid_x_ = 0;
    int grid_y_ = 0;

    std::string asset_name_;
    std::string tag_;

    int direction_ = 0;
    Color tint_ = WHITE;
    int z_index_ = 0;
    bool visible_ = true;

    GridEngine* engine_ = nullptr;
};

// 用函式指標定義行為的入門物件。
class CallbackGridObject : public GridObject {
public:
    using UpdateFn = void (*)(GridObject* self);
    using CollideFn = void (*)(GridObject* self, GridObject* other);

    CallbackGridObject(std::string asset_name, int x, int y, UpdateFn update, CollideFn collide = nullptr);

    void OnUpdate() override;
    void OnCollide(GridObject* other) override;

private:
    UpdateFn update_;
    CollideFn collide_;
};

// Implementation details only below here.

inline GridObject::GridObject(std::string asset_name, int x, int y)
    : grid_x_(x), grid_y_(y), asset_name_(std::move(asset_name)) {}

inline GridObject::GridObject(const GridObject& other)
    : grid_x_(other.grid_x_),
      grid_y_(other.grid_y_),
      asset_name_(other.asset_name_),
      tag_(other.tag_),
      direction_(other.direction_),
      tint_(other.tint_),
      z_index_(other.z_index_),
      visible_(other.visible_),
      engine_(nullptr) {}

inline void GridObject::Move(int dx, int dy) {
    grid_x_ += dx;
    grid_y_ += dy;
}

inline CallbackGridObject::CallbackGridObject(std::string asset_name, int x, int y, UpdateFn update, CollideFn collide)
    : GridObject(std::move(asset_name), x, y), update_(update), collide_(collide) {}

inline void CallbackGridObject::OnUpdate() {
    if (update_ != nullptr) update_(this);
}

inline void CallbackGridObject::OnCollide(GridObject* other) {
    if (collide_ != nullptr) collide_(this, other);
}

}  // namespace gridpp

#endif  // GRID_PLUS_PLUS_GRID_OBJECT_H_
