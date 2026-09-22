/** @file Overlay.h
 *  @brief 定義畫面覆蓋元素 handler 與實體資料。
 */
#ifndef GRID_PLUS_PLUS_OVERLAY_H_
#define GRID_PLUS_PLUS_OVERLAY_H_

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "GridValue.h"
#include "raylib.h"

namespace gridpp {

class GameEngine;
class GameEngineImpl;
class Overlay;

using OverlayCallback = void (*)(GameEngine engine, Overlay self);

enum class OverlayType {
    Image,
    Text,
    Button,
};

class OverlayImpl {
public:
    OverlayImpl(const OverlayImpl&) = default;
    OverlayImpl& operator=(const OverlayImpl&) = default;

private:
    friend class GameEngineImpl;
    friend class Overlay;

    explicit OverlayImpl(OverlayType type);

    OverlayType type_;
    std::string content_;
    int x_ = 0;
    int y_ = 0;
    int width_ = 0;
    int height_ = 0;
    int font_size_ = 20;
    Color color_ = BLACK;
    bool visible_ = true;
    bool hover_ = false;
    bool initialized_ = false;
    bool active_ = false;
    OverlayCallback init_ = nullptr;
    OverlayCallback update_ = nullptr;
    OverlayCallback click_ = nullptr;
    GridValueStore values_;
};

/** 可複製的畫面覆蓋元素 handler。 */
class Overlay {
public:
    Overlay() = default;

    bool exists() const;
    void remove();
    Overlay deepCopy() const;

    int x() const;
    int y() const;
    void setPosition(int x, int y);
    void move(int dx, int dy);

    bool visible() const;
    void show();
    void hide();

    std::string image() const;
    void setImage(const std::string& image);
    std::string text() const;
    void setText(const std::string& text);
    void setColor(Color color);

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

    void setInitFunction(OverlayCallback function);
    void setUpdateFunction(OverlayCallback function);
    void setClickFunction(OverlayCallback function);

private:
    friend class GameEngineImpl;

    Overlay(std::weak_ptr<GameEngineImpl> engine, std::uint64_t id);
    std::shared_ptr<GameEngineImpl> lockEngine() const;

    std::weak_ptr<GameEngineImpl> engine_;
    std::uint64_t id_ = 0;
};

inline OverlayImpl::OverlayImpl(OverlayType type) : type_(type) {}

}  // namespace gridpp

#endif  // GRID_PLUS_PLUS_OVERLAY_H_
