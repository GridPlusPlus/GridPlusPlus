/**
 * @file Overlay.h
 * @brief Grid++ 畫面覆蓋層基底類別。
 *
 * 此檔為核心實作拆分；一般使用者請 include "GridPlusPlus.h"。
 */
#ifndef GRID_PLUS_PLUS_OVERLAY_H_
#define GRID_PLUS_PLUS_OVERLAY_H_

#include <string>
#include <utility>

#include "raylib.h"

namespace gridpp {

class GridEngine;

// 使用像素座標、繪製在網格世界上方的畫面內容。
class Overlay {
public:
    virtual ~Overlay() = default;
    virtual void OnUpdate() {}
    virtual void Draw() {}

private:
    friend class GridEngine;

    GridEngine* engine_ = nullptr;
};

// 文字標籤：在 (x, y) 畫一行文字。
class Label : public Overlay {
public:
    Label(std::string text, int x, int y, int font_size = 20, Color color = BLACK);

    void set_text(const std::string& text) { text_ = text; }
    void Draw() override;

private:
    std::string text_;
    int x_;
    int y_;
    int font_size_;
    Color color_;
};

// 可點擊的文字按鈕。
class Button : public Overlay {
public:
    Button(std::string text, int x, int y, int width, int height);

    virtual void OnClick() {}
    void OnUpdate() override;
    void Draw() override;

private:
    std::string text_;
    int x_;
    int y_;
    int width_;
    int height_;
    bool hover_ = false;
};

// Implementation details only below here.

inline Label::Label(std::string text, int x, int y, int font_size, Color color)
    : text_(std::move(text)), x_(x), y_(y), font_size_(font_size), color_(color) {}

inline void Label::Draw() { DrawText(text_.c_str(), x_, y_, font_size_, color_); }

inline Button::Button(std::string text, int x, int y, int width, int height)
    : text_(std::move(text)), x_(x), y_(y), width_(width), height_(height) {}

inline void Button::OnUpdate() {
    const Vector2 mouse = GetMousePosition();
    hover_ = mouse.x >= x_ && mouse.x <= x_ + width_ && mouse.y >= y_ && mouse.y <= y_ + height_;
    if (hover_ && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) OnClick();
}

inline void Button::Draw() {
    DrawRectangle(x_, y_, width_, height_, hover_ ? SKYBLUE : LIGHTGRAY);
    DrawRectangleLines(x_, y_, width_, height_, DARKGRAY);
    constexpr int kFontSize = 20;
    const int text_width = MeasureText(text_.c_str(), kFontSize);
    DrawText(text_.c_str(), x_ + (width_ - text_width) / 2, y_ + (height_ - kFontSize) / 2, kFontSize, BLACK);
}

}  // namespace gridpp

#endif  // GRID_PLUS_PLUS_OVERLAY_H_
