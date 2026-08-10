/**
 * @file GridShapes.h
 * @brief 不需素材包的基本網格圖形。
 */
#ifndef GRID_PLUS_PLUS_GRID_SHAPES_H_
#define GRID_PLUS_PLUS_GRID_SHAPES_H_

#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>

#include "GridPlusPlus.h"

namespace gridpp::shapes {

// 所有基本圖形的共用基底；size 使用像素，圖形置中於所在格。
class Shape : public GridObject {
public:
    int size() const { return size_; }
    void set_size(int size);

protected:
    Shape(int x, int y, int size, Color color);
    Vector2 Center(GridEngine* engine) const;

private:
    int size_;
};

class Square : public Shape {
public:
    Square(int x, int y, int size, Color color = BLACK) : Shape(x, y, size, color) {}
    void Render(GridEngine* engine) override;
};

class Circle : public Shape {
public:
    Circle(int x, int y, int size, Color color = BLACK) : Shape(x, y, size, color) {}
    void Render(GridEngine* engine) override;
};

class Triangle : public Shape {
public:
    Triangle(int x, int y, int size, Color color = BLACK) : Shape(x, y, size, color) {}
    void Render(GridEngine* engine) override;
};

class Pentagon : public Shape {
public:
    Pentagon(int x, int y, int size, Color color = BLACK) : Shape(x, y, size, color) {}
    void Render(GridEngine* engine) override;
};

class Star : public Shape {
public:
    Star(int x, int y, int size, Color color = BLACK) : Shape(x, y, size, color) {}
    void Render(GridEngine* engine) override;
};

// Implementation details only below here.

inline Shape::Shape(int x, int y, int size, Color color) : GridObject("", x, y), size_(size) {
    set_size(size);
    set_tint(color);
}

inline void Shape::set_size(int size) {
    if (size < 1) throw std::invalid_argument("GridShapes Error: size must be greater than 0");
    size_ = size;
}

inline Vector2 Shape::Center(GridEngine* engine) const {
    const float grid_size = static_cast<float>(engine->grid_size());
    return {(static_cast<float>(x()) + 0.5f) * grid_size, (static_cast<float>(y()) + 0.5f) * grid_size};
}

inline void Square::Render(GridEngine* engine) {
    const int grid_size = engine->grid_size();
    const int pixel_x = x() * grid_size + (grid_size - size()) / 2;
    const int pixel_y = y() * grid_size + (grid_size - size()) / 2;
    DrawRectangle(pixel_x, pixel_y, size(), size(), tint());
}

inline void Circle::Render(GridEngine* engine) {
    const Vector2 center = Center(engine);
    DrawCircle(static_cast<int>(center.x), static_cast<int>(center.y), size() / 2.0f, tint());
}

inline void Triangle::Render(GridEngine* engine) { DrawPoly(Center(engine), 3, size() / 2.0f, -90.0f, tint()); }

inline void Pentagon::Render(GridEngine* engine) { DrawPoly(Center(engine), 5, size() / 2.0f, -90.0f, tint()); }

inline void Star::Render(GridEngine* engine) {
    constexpr float kPi = 3.14159265358979323846f;
    constexpr float kInnerRadiusRatio = 0.45f;
    const Vector2 center = Center(engine);
    const float outer_radius = size() / 2.0f;
    std::array<Vector2, 12> points = {};
    points[0] = center;
    for (std::size_t i = 0; i <= 10; ++i) {
        const std::size_t vertex = i % 10;
        const float radius = vertex % 2 == 0 ? outer_radius : outer_radius * kInnerRadiusRatio;
        const float angle = (-90.0f + static_cast<float>(vertex) * 36.0f) * kPi / 180.0f;
        points[i + 1] = {center.x + std::cos(angle) * radius, center.y + std::sin(angle) * radius};
    }
    DrawTriangleFan(points.data(), static_cast<int>(points.size()), tint());
}

}  // namespace gridpp::shapes

#endif  // GRID_PLUS_PLUS_GRID_SHAPES_H_
