#include "GridPlusPlus.h"

#include <cassert>

static int callbackUpdates = 0;
static int destroyedUpdates = 0;
static int destroyedCollisions = 0;
static int destroyedDraws = 0;
static int destructors = 0;
static int runtimeSpawnUpdates = 0;
static int hiddenUpdates = 0;
static int hiddenCollisions = 0;
static int hiddenDraws = 0;
static int hideOnCollisionCalls = 0;
static int clearedDestructors = 0;
static int replacementUpdates = 0;

static void updateCallback(GridObject*) {
    callbackUpdates++;
}

static void updateRuntimeSpawn(GridObject*) {
    runtimeSpawnUpdates++;
}

static void updateReplacement(GridObject*) {
    replacementUpdates++;
}

class SelfDestroyingObject : public GridObject {
public:
    SelfDestroyingObject() : GridObject("", 0, 0) {}
    ~SelfDestroyingObject() override { destructors++; }

    void onUpdate() override {
        destroyedUpdates++;
        getEngine()->destroy(this);
    }

    void onCollide(GridObject*) override { destroyedCollisions++; }
    void render(GridEngine*) override { destroyedDraws++; }
};

class RuntimeSpawner : public GridObject {
public:
    RuntimeSpawner() : GridObject("", 1, 0) {}

    void onUpdate() override {
        if (spawned) return;
        getEngine()->spawn("", 1, 1, updateRuntimeSpawn);
        spawned = true;
    }

private:
    bool spawned = false;
};

class HiddenObject : public GridObject {
public:
    HiddenObject() : GridObject("", 0, 0) {}
    void onUpdate() override { hiddenUpdates++; }
    void onCollide(GridObject*) override { hiddenCollisions++; }
    void render(GridEngine*) override { hiddenDraws++; }
};

class HideOnCollision : public GridObject {
public:
    HideOnCollision() : GridObject("", 5, 5) {}

    void onCollide(GridObject*) override {
        hideOnCollisionCalls++;
        setVisible(false);
    }
};

class ClearAndRespawn : public GridObject {
public:
    ~ClearAndRespawn() override { clearedDestructors++; }

    void onUpdate() override {
        getEngine()->clearObjects();
        getEngine()->spawn("", 0, 0, updateReplacement);
    }
};

int main() {
    GridEngine game(2, 2);

    GridObject* callbackObject = game.spawn("", 0, 0, updateCallback);
    assert(callbackObject != nullptr);
    GridObject* existingObject = new SelfDestroyingObject();
    assert(game.spawn(existingObject) == existingObject);
    game.spawn(new RuntimeSpawner());
    HiddenObject* hidden = new HiddenObject();
    hidden->setVisible(false);
    game.spawn(hidden);
    game.spawn(new HideOnCollision());
    game.spawn(new GridObject("", 5, 5));
    game.spawn(new GridObject("", 5, 5));

    game.run();

    assert(callbackUpdates == 2);
    assert(destroyedUpdates == 1);
    assert(destroyedCollisions == 0);
    assert(destroyedDraws == 0);
    assert(destructors == 1);
    assert(runtimeSpawnUpdates == 1);
    assert(hiddenUpdates == 2);
    assert(hiddenCollisions == 0);
    assert(hiddenDraws == 0);
    assert(hideOnCollisionCalls == 1);

    game.clearObjects();

    GridEngine restartedGame(2, 2);
    restartedGame.spawn(new ClearAndRespawn());
    restartedGame.run();

    assert(clearedDestructors == 1);
    assert(replacementUpdates == 1);
    restartedGame.clearObjects();
}
