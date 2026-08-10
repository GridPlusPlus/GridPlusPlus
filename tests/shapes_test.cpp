#include <cassert>
#include <stdexcept>

#include "GridShapes.h"

using gridpp::GridEngine;
using gridpp::shapes::Circle;
using gridpp::shapes::Pentagon;
using gridpp::shapes::Square;
using gridpp::shapes::Star;
using gridpp::shapes::Triangle;

static bool SameColor(Color left, Color right) {
    return left.r == right.r && left.g == right.g && left.b == right.b && left.a == right.a;
}

int main() {
    GridEngine game(4, 4, 40);

    Square square(1, 2, 20, RED);
    square.Render(&game);
    assert(test_draw_rectangle_calls == 1);
    assert(test_last_rectangle.x == 50);
    assert(test_last_rectangle.y == 90);
    assert(test_last_rectangle.width == 20);
    assert(test_last_rectangle.height == 20);
    assert(SameColor(test_last_shape_color, RED));

    Circle circle(1, 2, 24, BLUE);
    circle.Render(&game);
    assert(test_draw_circle_calls == 1);
    assert(test_last_circle_center.x == 60);
    assert(test_last_circle_center.y == 100);
    assert(test_last_circle_radius == 12);
    assert(SameColor(test_last_shape_color, BLUE));

    Triangle triangle(1, 2, 30, GREEN);
    triangle.Render(&game);
    assert(test_draw_poly_calls == 1);
    assert(test_last_poly_sides == 3);
    assert(test_last_poly_radius == 15);
    assert(test_last_poly_rotation == -90);

    Pentagon pentagon(1, 2, 32, ORANGE);
    pentagon.Render(&game);
    assert(test_draw_poly_calls == 2);
    assert(test_last_poly_sides == 5);
    assert(test_last_poly_radius == 16);

    Star star(1, 2, 36, YELLOW);
    star.Render(&game);
    assert(test_draw_triangle_fan_calls == 1);
    assert(test_last_triangle_fan_center.x == 60);
    assert(test_last_triangle_fan_center.y == 100);
    assert(test_last_triangle_fan_points == 12);

    circle.set_size(50);
    assert(circle.size() == 50);
    circle.set_tint(PINK);
    circle.Render(&game);
    assert(test_last_circle_radius == 25);
    assert(SameColor(test_last_shape_color, PINK));

    bool size_rejected = false;
    try {
        Square invalid(0, 0, 0);
        (void)invalid;
    } catch (const std::invalid_argument&) {
        size_rejected = true;
    }
    assert(size_rejected);
}
