#include <cassert>
#include <stdexcept>

#include "GridPlusPlus.h"

using gridpp::GridEngine;
using gridpp::GridObject;
using gridpp::Overlay;

static int callback_updates = 0;
static int destroyed_updates = 0;
static int destroyed_collisions = 0;
static int destroyed_draws = 0;
static int destructors = 0;
static int runtime_spawn_updates = 0;
static int hidden_updates = 0;
static int hidden_collisions = 0;
static int hidden_draws = 0;
static int hide_on_collision_calls = 0;
static int cleared_destructors = 0;
static int replacement_updates = 0;
static int engine_cleanup_destructors = 0;
static int overlay_destructors = 0;
static int runtime_overlay_updates = 0;

static void UpdateCallback(GridObject*) { ++callback_updates; }

static void UpdateRuntimeSpawn(GridObject*) { ++runtime_spawn_updates; }

static void UpdateReplacement(GridObject*) { ++replacement_updates; }

class SelfDestroyingObject : public GridObject {
public:
    SelfDestroyingObject() : GridObject("", 0, 0) {}
    ~SelfDestroyingObject() override { ++destructors; }

    void OnUpdate() override {
        ++destroyed_updates;
        engine()->Destroy(this);
    }

    void OnCollide(GridObject*) override { ++destroyed_collisions; }
    void Render(GridEngine*) override { ++destroyed_draws; }
};

class RuntimeSpawner : public GridObject {
public:
    RuntimeSpawner() : GridObject("", 1, 0) {}

    void OnUpdate() override {
        if (spawned_) return;
        engine()->Spawn("", 1, 1, UpdateRuntimeSpawn);
        spawned_ = true;
    }

private:
    bool spawned_ = false;
};

class HiddenObject : public GridObject {
public:
    HiddenObject() : GridObject("", 0, 0) {}
    void OnUpdate() override { ++hidden_updates; }
    void OnCollide(GridObject*) override { ++hidden_collisions; }
    void Render(GridEngine*) override { ++hidden_draws; }
};

class HideOnCollision : public GridObject {
public:
    HideOnCollision() : GridObject("", 5, 5) {}

    void OnCollide(GridObject*) override {
        ++hide_on_collision_calls;
        set_visible(false);
    }
};

class ClearAndRespawn : public GridObject {
public:
    ~ClearAndRespawn() override { ++cleared_destructors; }

    void OnUpdate() override {
        engine()->ClearObjects();
        engine()->Spawn("", 0, 0, UpdateReplacement);
    }
};

class EngineCleanupObject : public GridObject {
public:
    ~EngineCleanupObject() override { ++engine_cleanup_destructors; }
};

class RuntimeChildOverlay : public Overlay {
public:
    ~RuntimeChildOverlay() override { ++overlay_destructors; }
    void OnUpdate() override { ++runtime_overlay_updates; }
};

class RuntimeAddingOverlay : public Overlay {
public:
    explicit RuntimeAddingOverlay(GridEngine* engine) : engine_(engine) {}
    ~RuntimeAddingOverlay() override { ++overlay_destructors; }

    void OnUpdate() override {
        if (added_) return;
        engine_->AddOverlay(new RuntimeChildOverlay());
        added_ = true;
    }

private:
    GridEngine* engine_;
    bool added_ = false;
};

int main() {
    {
        GridEngine game(2, 2);

        bool null_spawn_rejected = false;
        try {
            game.Spawn(nullptr);
        } catch (const std::invalid_argument&) {
            null_spawn_rejected = true;
        }
        assert(null_spawn_rejected);

        GridObject* callback_object = game.Spawn("", 0, 0, UpdateCallback);
        assert(callback_object != nullptr);
        GridObject* existing_object = new SelfDestroyingObject();
        assert(game.Spawn(existing_object) == existing_object);
        game.Spawn(new RuntimeSpawner());
        HiddenObject* hidden = new HiddenObject();
        hidden->set_visible(false);
        game.Spawn(hidden);
        game.Spawn(new HideOnCollision());
        game.Spawn(new GridObject("", 5, 5));
        game.Spawn(new GridObject("", 5, 5));

        EngineCleanupObject* owned_object = new EngineCleanupObject();
        game.Spawn(owned_object);
        bool duplicate_spawn_rejected = false;
        try {
            game.Spawn(owned_object);
        } catch (const std::logic_error&) {
            duplicate_spawn_rejected = true;
        }
        assert(duplicate_spawn_rejected);

        bool null_overlay_rejected = false;
        try {
            game.AddOverlay(nullptr);
        } catch (const std::invalid_argument&) {
            null_overlay_rejected = true;
        }
        assert(null_overlay_rejected);

        RuntimeAddingOverlay* overlay = new RuntimeAddingOverlay(&game);
        game.AddOverlay(overlay);
        bool duplicate_overlay_rejected = false;
        try {
            game.AddOverlay(overlay);
        } catch (const std::logic_error&) {
            duplicate_overlay_rejected = true;
        }
        assert(duplicate_overlay_rejected);

        game.Run();

        assert(callback_updates == 2);
        assert(destroyed_updates == 1);
        assert(destroyed_collisions == 0);
        assert(destroyed_draws == 0);
        assert(destructors == 1);
        assert(runtime_spawn_updates == 1);
        assert(hidden_updates == 2);
        assert(hidden_collisions == 0);
        assert(hidden_draws == 0);
        assert(hide_on_collision_calls == 1);
        assert(runtime_overlay_updates == 1);
    }
    assert(engine_cleanup_destructors == 1);
    assert(overlay_destructors == 2);

    {
        GridEngine restarted_game(2, 2);
        restarted_game.Spawn(new ClearAndRespawn());
        restarted_game.Run();

        assert(cleared_destructors == 1);
        assert(replacement_updates == 1);
    }

    const int loads_before = test_texture_loads;
    const int unloads_before = test_texture_unloads;
    {
        GridEngine asset_game(2, 2);
        asset_game.LoadAssets("examples/pacman/pacman.db");
        const int textures_per_pack = test_texture_loads - loads_before;
        assert(textures_per_pack > 0);

        asset_game.LoadAssets("examples/pacman/pacman.db");
        assert(test_texture_unloads - unloads_before == textures_per_pack);
    }
    assert(test_texture_loads - loads_before == test_texture_unloads - unloads_before);
    assert(test_texture_unloads_after_close == 0);
}
