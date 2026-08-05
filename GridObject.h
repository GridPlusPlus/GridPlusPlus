/**
 * @file GridObject.h
 * @brief Grid++ 網格物件基底類別。
 *
 * 此檔為核心實作拆分；一般使用者請 include "GridPlusPlus.h"。
 */
#ifndef GRIDOBJECT_H
#define GRIDOBJECT_H

#include "raylib.h"

#include <string>

class GridEngine;

// 遊戲世界中的網格物件。
// 繼承它並覆寫 onStart / onUpdate / onCollide。詳見 docs/guide/game-objects.md。
class GridObject {
public:
    GridObject()
        : gridX(0), gridY(0), assetName("") {}
    GridObject(std::string assetName, int x, int y)
        : gridX(x), gridY(y), assetName(assetName) {}

    virtual ~GridObject() {}

    // 引擎呼叫的生命週期函式。
    virtual void onStart() {}
    virtual void onUpdate() {}
    virtual void onCollide(GridObject* other) { (void)other; }

    int  getX() const { return gridX; }
    int  getY() const { return gridY; }
    void setX(int x)  { gridX = x; }
    void setY(int y)  { gridY = y; }
    void move(int dx, int dy) { gridX += dx; gridY += dy; }

    std::string getAssetName() const { return assetName; }

    // 身分標記，碰撞時分辨對方。
    std::string getTag() const { return tag; }
    void setTag(const std::string& t) { tag = t; }

    // 方向為 0 到 3，每增加 1 逆時針旋轉 90 度。
    int  getDirection() const { return direction; }
    void setDirection(int dir) { direction = ((dir % 4) + 4) % 4; }

    // 調色。繪製時把素材整體乘上這個顏色。預設 WHITE（不改變顏色）。
    Color getTint() const { return tint; }
    void  setTint(Color c) { tint = c; }

    // 隱藏時仍會更新，但不會繪製或參與碰撞。
    bool isVisible() const { return visible; }
    void setVisible(bool value) { visible = value; }

    // 把自己畫出來；預設繪製 assetName，子類別可覆寫。
    // 因為實作需要完整的 GridEngine 定義，所以放在 GridPlusPlus.h 最後。
    virtual void render(GridEngine* engine);

    void setEngine(GridEngine* e) { engine = e; }
    GridEngine* getEngine() const { return engine; }

protected:
    int gridX, gridY;
    std::string assetName;
    std::string tag;
    int direction = 0;
    Color tint = WHITE;
    GridEngine* engine = nullptr;

private:
    bool visible = true;
};


// 用函式指標定義行為的入門物件。
class CallbackGridObject : public GridObject {
public:
    using UpdateFn  = void(*)(GridObject* self);
    using CollideFn = void(*)(GridObject* self, GridObject* other);

    CallbackGridObject(std::string asset, int x, int y,
                       UpdateFn update, CollideFn collide = nullptr)
        : GridObject(asset, x, y), updateFn(update), collideFn(collide) {}

    void onUpdate() override {
        if (updateFn) updateFn(this);
    }

    void onCollide(GridObject* other) override {
        if (collideFn) collideFn(this, other);
    }

private:
    UpdateFn updateFn;
    CollideFn collideFn;
};

#endif // GRIDOBJECT_H
