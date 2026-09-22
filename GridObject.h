/** @file GridObject.h
 *  @brief 定義網格物件 handler 與實體資料。
 */
#ifndef GRID_PLUS_PLUS_GRID_OBJECT_H_
#define GRID_PLUS_PLUS_GRID_OBJECT_H_

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "GridValue.h"
#include "raylib.h"

namespace gridpp {

class GameEngine;
class GameEngineImpl;
class GridObject;

using GridObjectCallback = void (*)(GameEngine engine, GridObject self);
using CollisionCallback = void (*)(GameEngine engine, GridObject self, GridObject other);

enum class GridObjectType {
    Image,
    Square,
    Circle,
    Triangle,
    Pentagon,
    Star,
};

class GridObjectImpl {
public:
    GridObjectImpl(const GridObjectImpl&) = default;
    GridObjectImpl& operator=(const GridObjectImpl&) = default;

private:
    friend class GameEngineImpl;
    friend class GridObject;

    explicit GridObjectImpl(std::string image);
    GridObjectImpl(GridObjectType type, int x, int y, int size, Color color);

    GridObjectType type_ = GridObjectType::Image;
    std::string image_;
    int x_ = 0;
    int y_ = 0;
    int size_ = 0;
    int direction_ = 0;
    Color color_ = WHITE;
    int layer_ = 0;
    bool visible_ = true;
    bool initialized_ = false;
    bool active_ = false;
    GridObjectCallback init_ = nullptr;
    GridObjectCallback update_ = nullptr;
    CollisionCallback collide_ = nullptr;
    GridValueStore values_;
};

/** 可複製的網格物件 handler；複本仍指向同一個遊戲實體。 */
class GridObject {
public:
    GridObject() = default;

    bool exists() const;
    void remove();
    GridObject deepCopy() const;

    int x() const;
    int y() const;
    void setPosition(int x, int y);
    void move(int dx, int dy);

    std::string image() const;
    void setImage(const std::string& image);

    int direction() const;
    void setDirection(int direction);

    Color color() const;
    void setColor(Color color);

    int layer() const;
    void setLayer(int layer);

    bool visible() const;
    void show();
    void hide();

    void set(const std::string& key, int value);
    void set(const std::string& key, long long value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, const char* value);

    int get(const std::string& key, int& value) const;
    int get(const std::string& key, long long& value) const;
    int get(const std::string& key, double& value) const;
    int get(const std::string& key, bool& value) const;
    int get(const std::string& key, std::string& value) const;

    template <typename T>
    void set(const std::string& key, const std::vector<T>& value);

    template <typename T>
    int get(const std::string& key, std::vector<T>& value) const;

    void setInitFunction(GridObjectCallback function);
    void setUpdateFunction(GridObjectCallback function);
    void setCollideFunction(CollisionCallback function);

private:
    friend class GameEngineImpl;

    GridObject(std::weak_ptr<GameEngineImpl> engine, std::uint64_t id);
    std::shared_ptr<GameEngineImpl> lockEngine() const;

    std::weak_ptr<GameEngineImpl> engine_;
    std::uint64_t id_ = 0;
};

inline GridObjectImpl::GridObjectImpl(std::string image) : image_(std::move(image)) {}

inline GridObjectImpl::GridObjectImpl(GridObjectType type, int x, int y, int size, Color color)
    : type_(type), x_(x), y_(y), size_(size), color_(color) {}

}  // namespace gridpp

#endif  // GRID_PLUS_PLUS_GRID_OBJECT_H_
