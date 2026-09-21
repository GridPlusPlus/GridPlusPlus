#include <cassert>
#include <stdexcept>

#include "GridPlusPlus.h"

using gridpp::Game;
using gridpp::ObjectHandler;

namespace {

int circle_updates = 0;

void UpdateCircle(Game, ObjectHandler) { ++circle_updates; }

bool SameColor(Color left, Color right) {
    return left.r == right.r && left.g == right.g && left.b == right.b && left.a == right.a;
}

}  // namespace

int main() {
    Game game(4, 4, 40);

    ObjectHandler square = game.addSquare(1, 2, 20, RED);
    ObjectHandler circle = game.addCircle(1, 2, 24, BLUE, nullptr, UpdateCircle);
    ObjectHandler triangle = game.addTriangle(1, 2, 30, GREEN);
    ObjectHandler pentagon = game.addPentagon(1, 2, 32, ORANGE);
    ObjectHandler star = game.addStar(1, 2, 36, YELLOW);
    ObjectHandler star_copy = star.deepCopy();
    star_copy.move(1, 0);
    star_copy.setColor(PINK);

    assert(square.x() == 1);
    assert(square.y() == 2);
    assert(star.x() == 1);
    assert(star_copy.x() == 2);
    assert(SameColor(star.color(), YELLOW));
    assert(SameColor(star_copy.color(), PINK));

    bool image_rejected = false;
    try {
        square.setImage("not-used");
    } catch (const std::runtime_error&) {
        image_rejected = true;
    }
    assert(image_rejected);

    game.run();

    assert(circle_updates == 2);
    assert(test_draw_rectangle_calls == 2);
    assert(test_draw_circle_calls == 2);
    assert(test_draw_poly_calls == 4);
    assert(test_draw_triangle_fan_calls == 4);

    game.clearObjects();
    assert(!square.exists());
    assert(!circle.exists());
    assert(!triangle.exists());
    assert(!pentagon.exists());
    assert(!star.exists());
    assert(!star_copy.exists());
}
