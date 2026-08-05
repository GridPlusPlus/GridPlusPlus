/**
 * @file Overlay.h
 * @brief Grid++ 畫面覆蓋層基底類別。
 */
#ifndef OVERLAY_H
#define OVERLAY_H

#include "raylib.h"

#include <string>

// 使用像素座標、繪製在網格世界上方的畫面內容。
class Overlay {
public:
    virtual ~Overlay() {}
    virtual void onUpdate() {}
    virtual void draw() {}
};

// 文字標籤：在 (x, y) 畫一行文字。
class Label : public Overlay {
public:
    Label(const std::string& text, int x, int y, int fontSize = 20, Color color = BLACK)
        : text(text), x(x), y(y), fontSize(fontSize), color(color) {}

    void setText(const std::string& t) { text = t; }

    void draw() override {
        DrawText(text.c_str(), x, y, fontSize, color);
    }

protected:
    std::string text;
    int x, y, fontSize;
    Color color;
};

// 可點擊的文字按鈕。
class Button : public Overlay {
public:
    Button(const std::string& text, int x, int y, int w, int h)
        : text(text), x(x), y(y), w(w), h(h) {}

    virtual void onClick() {}

    void onUpdate() override {
        Vector2 m = GetMousePosition();
        hover = (m.x >= x && m.x <= x + w && m.y >= y && m.y <= y + h);
        if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) onClick();
    }

    void draw() override {
        DrawRectangle(x, y, w, h, hover ? SKYBLUE : LIGHTGRAY);
        DrawRectangleLines(x, y, w, h, DARKGRAY);
        int fs = 20, tw = MeasureText(text.c_str(), fs);
        DrawText(text.c_str(), x + (w - tw) / 2, y + (h - fs) / 2, fs, BLACK);
    }

protected:
    std::string text;
    int x, y, w, h;
    bool hover = false;
};

#endif // OVERLAY_H
