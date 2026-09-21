#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

#include "GridPlusPlus.h"

using gridpp::Game;
using gridpp::ObjectHandler;

namespace {

std::vector<long long> init_order;
int child_updates = 0;
bool child_spawned = false;

void RecordInit(Game, ObjectHandler self) {
    long long order = 0;
    assert(self.get("order", order) == 1);
    init_order.push_back(order);
}

void UpdateChild(Game, ObjectHandler) { ++child_updates; }

void InitChild(Game, ObjectHandler self) { self.set("ready", true); }

void SpawnChild(Game game, ObjectHandler) {
    if (child_spawned) return;
    child_spawned = true;
    game.addObject("", InitChild, UpdateChild);
}

}  // namespace

int main() {
    Game game(3, 3);

    ObjectHandler first = game.addObject("", RecordInit);
    first.set("order", 1);
    first.set("name", "first");
    first.setPosition(1, 1);

    ObjectHandler second = game.addObject("", RecordInit);
    second.set("order", 2LL);

    ObjectHandler alias = first;
    alias.move(1, 0);
    assert(first.x() == 2);

    ObjectHandler copy = first.deepCopy();
    copy.setPosition(0, 0);
    assert(first.x() == 2);
    assert(copy.x() == 0);

    std::string copied_name;
    assert(copy.get("name", copied_name) == 1);
    assert(copied_name == "first");

    long long missing = 42;
    assert(first.get("missing", missing) == 0);
    assert(missing == 0);

    bool wrong_type_failed = false;
    try {
        bool wrong = false;
        first.get("order", wrong);
    } catch (const std::runtime_error&) {
        wrong_type_failed = true;
    }
    assert(wrong_type_failed);

    game.addObject("", nullptr, SpawnChild);
    assert(init_order.empty());

    game.run();

    assert((init_order == std::vector<long long>{1, 2, 1}));
    assert(child_updates == 1);

    first.remove();
    assert(!first.exists());
    assert(alias.exists() == false);
    assert(copy.exists());

    bool removed_object_failed = false;
    try {
        alias.move(1, 0);
    } catch (const std::runtime_error&) {
        removed_object_failed = true;
    }
    assert(removed_object_failed);

    return 0;
}
