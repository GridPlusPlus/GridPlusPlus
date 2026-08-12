#include <cassert>
#include <vector>

#include "GridPlusPlus.h"

using gridpp::GridEngine;
using gridpp::GridObject;
using gridpp::Overlay;

static std::vector<int> update_order;
static std::vector<int> collision_order;
static std::vector<int> draw_order;

class TrackedObject : public GridObject {
public:
    TrackedObject(int id, int x, int y, int z_index) : GridObject("", x, y), id_(id) { set_z_index(z_index); }

    void OnUpdate() override { update_order.push_back(id_); }
    void OnCollide(GridObject*) override { collision_order.push_back(id_); }
    void Render(GridEngine*) override { draw_order.push_back(id_); }

private:
    int id_;
};

class ChangingZObject : public TrackedObject {
public:
    ChangingZObject() : TrackedObject(4, 1, 1, -20) {}

    void OnUpdate() override {
        TrackedObject::OnUpdate();
        if (++updates_ == 2) set_z_index(20);
    }

private:
    int updates_ = 0;
};

class TrackedOverlay : public Overlay {
public:
    void Draw() override { draw_order.push_back(9); }
};

int main() {
    GridObject object;
    assert(object.z_index() == 0);
    object.set_z_index(-7);
    assert(object.z_index() == -7);
    GridObject copy(object);
    assert(copy.z_index() == -7);

    GridEngine game(2, 2);
    game.Spawn(new TrackedObject(1, 0, 0, 10));
    game.Spawn(new TrackedObject(2, 0, 0, -10));
    game.Spawn(new TrackedObject(3, 0, 0, 10));
    game.Spawn(new ChangingZObject());
    game.AddOverlay(new TrackedOverlay());
    game.Run();

    assert((update_order == std::vector<int>{1, 2, 3, 4, 1, 2, 3, 4}));
    assert((collision_order == std::vector<int>{1, 2, 1, 3, 2, 3, 1, 2, 1, 3, 2, 3}));
    assert((draw_order == std::vector<int>{4, 2, 1, 3, 9, 2, 1, 3, 4, 9}));
}
