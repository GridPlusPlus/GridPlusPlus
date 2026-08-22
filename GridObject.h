/** @file GridObject.h
 *  @brief 定義網格物件與函式版網格物件。
 */
#ifndef GRID_PLUS_PLUS_GRID_OBJECT_H_
#define GRID_PLUS_PLUS_GRID_OBJECT_H_

#include <string>
#include <utility>

#include "raylib.h"

namespace gridpp {

class GridEngine;

/** 遊戲世界中會更新、碰撞與繪製的物件。 */
class GridObject {
public:
    GridObject() = default;
    GridObject(std::string asset_name, int x, int y);
    /** 複本不屬於任何 GridEngine。 */
    GridObject(const GridObject& other);
    GridObject& operator=(const GridObject& other) = delete;
    virtual ~GridObject() = default;

    /** 物件加入引擎後呼叫一次。 */
    virtual void OnSpawn();

    /** 每幀更新時呼叫。 */
    virtual void OnUpdate();

    /**
     * 與另一個可見物件位於同一個有效網格座標時呼叫。
     * @param other 與此物件碰撞的借用指標。
     */
    virtual void OnCollide(GridObject* other);

    /**
     * 繪製物件。預設使用物件的素材、方向與顏色繪製所在格。
     * @param engine 所屬引擎的借用指標。
     */
    virtual void Render(GridEngine* engine);

    int x() const;
    int y() const;
    void set_x(int x);
    void set_y(int y);
    void Move(int dx, int dy);

    const std::string& asset_name() const;
    void set_asset_name(const std::string& asset_name);

    /** 可用於碰撞時辨識物件。 */
    const std::string& tag() const;
    void set_tag(const std::string& tag);

    int direction() const;
    /** 每增加 1 逆時針旋轉 90 度；輸入會正規化至 0 到 3。 */
    void set_direction(int direction);

    Color tint() const;
    void set_tint(Color tint);

    int z_index() const;
    /** 數值越大越晚繪製；相同數值保持 Spawn 順序。 */
    void set_z_index(int z_index);

    bool visible() const;
    /** 隱藏的物件仍會更新，但不會繪製或參與碰撞。 */
    void set_visible(bool visible);

    /** @return 所屬引擎的借用指標；尚未 Spawn 時為 nullptr。 */
    GridEngine* engine() const;

private:
    friend class GridEngine;

    void set_engine(GridEngine* engine);

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

/** 使用函式指標提供更新與碰撞行為的網格物件。 */
class CallbackGridObject : public GridObject {
public:
    using UpdateFn = void (*)(GridObject* self);
    using CollideFn = void (*)(GridObject* self, GridObject* other);

    /** update 與 collide 可以是 nullptr。 */
    CallbackGridObject(std::string asset_name, int x, int y, UpdateFn update, CollideFn collide = nullptr);

    void OnUpdate() override;
    void OnCollide(GridObject* other) override;

private:
    UpdateFn update_;
    CollideFn collide_;
};

// Inline definitions

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

inline void GridObject::OnSpawn() {}

inline void GridObject::OnUpdate() {}

inline void GridObject::OnCollide(GridObject* other) { (void)other; }

inline int GridObject::x() const { return grid_x_; }

inline int GridObject::y() const { return grid_y_; }

inline void GridObject::set_x(int x) { grid_x_ = x; }

inline void GridObject::set_y(int y) { grid_y_ = y; }

inline void GridObject::Move(int dx, int dy) {
    grid_x_ += dx;
    grid_y_ += dy;
}

inline const std::string& GridObject::asset_name() const { return asset_name_; }

inline void GridObject::set_asset_name(const std::string& asset_name) { asset_name_ = asset_name; }

inline const std::string& GridObject::tag() const { return tag_; }

inline void GridObject::set_tag(const std::string& tag) { tag_ = tag; }

inline int GridObject::direction() const { return direction_; }

inline void GridObject::set_direction(int direction) { direction_ = ((direction % 4) + 4) % 4; }

inline Color GridObject::tint() const { return tint_; }

inline void GridObject::set_tint(Color tint) { tint_ = tint; }

inline int GridObject::z_index() const { return z_index_; }

inline void GridObject::set_z_index(int z_index) { z_index_ = z_index; }

inline bool GridObject::visible() const { return visible_; }

inline void GridObject::set_visible(bool visible) { visible_ = visible; }

inline GridEngine* GridObject::engine() const { return engine_; }

inline void GridObject::set_engine(GridEngine* engine) { engine_ = engine; }

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
