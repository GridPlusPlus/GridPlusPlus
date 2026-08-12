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
static int runtime_spawn_calls = 0;
static int hidden_updates = 0;
static int hidden_collisions = 0;
static int hidden_draws = 0;
static int hide_on_collision_calls = 0;
static int cleared_destructors = 0;
static int replacement_updates = 0;
static int engine_cleanup_destructors = 0;
static int overlay_destructors = 0;
static int runtime_overlay_updates = 0;
static int runtime_overlay_draws = 0;
static int draw_added_overlay_updates = 0;
static int draw_added_overlay_draws = 0;
static int out_of_bounds_collisions = 0;
static int failed_spawn_destructors = 0;
static int pending_spawn_calls = 0;
static int pending_spawn_destructors = 0;

static void UpdateCallback(GridObject*) { ++callback_updates; }

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

class RuntimeSpawnedObject : public GridObject {
public:
    void OnSpawn() override { ++runtime_spawn_calls; }
    void OnUpdate() override { ++runtime_spawn_updates; }
};

class RuntimeSpawner : public GridObject {
public:
    RuntimeSpawner() : GridObject("", 1, 0) {}

    void OnUpdate() override {
        if (spawned_) return;
        engine()->Spawn(new RuntimeSpawnedObject());
        spawned_ = true;
    }

private:
    bool spawned_ = false;
};

class ThrowingOnSpawn : public GridObject {
public:
    ~ThrowingOnSpawn() override { ++failed_spawn_destructors; }
    void OnSpawn() override { throw std::runtime_error("spawn failed"); }
};

class PendingAfterSpawnFailure : public GridObject {
public:
    ~PendingAfterSpawnFailure() override { ++pending_spawn_destructors; }
    void OnSpawn() override { ++pending_spawn_calls; }
};

class RuntimeSpawnFailure : public GridObject {
public:
    void OnUpdate() override {
        if (spawned_) return;
        engine()->Spawn(new ThrowingOnSpawn());
        engine()->Spawn(new PendingAfterSpawnFailure());
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
    HideOnCollision() : GridObject("", 0, 1) {}

    void OnCollide(GridObject*) override {
        ++hide_on_collision_calls;
        set_visible(false);
    }
};

class OutOfBoundsCollisionCounter : public GridObject {
public:
    OutOfBoundsCollisionCounter(int x, int y) : GridObject("", x, y) {}
    void OnCollide(GridObject*) override { ++out_of_bounds_collisions; }
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
    void Draw() override { ++runtime_overlay_draws; }
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

class DrawAddedOverlay : public Overlay {
public:
    ~DrawAddedOverlay() override { ++overlay_destructors; }
    void OnUpdate() override { ++draw_added_overlay_updates; }
    void Draw() override { ++draw_added_overlay_draws; }
};

class RuntimeDrawingOverlay : public Overlay {
public:
    explicit RuntimeDrawingOverlay(GridEngine* engine) : engine_(engine) {}
    ~RuntimeDrawingOverlay() override { ++overlay_destructors; }

    void Draw() override {
        if (added_) return;
        engine_->AddOverlay(new DrawAddedOverlay());
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
        callback_object->set_asset_name("changed");
        assert(callback_object->asset_name() == "changed");
        GridObject copied_object(*callback_object);
        assert(copied_object.engine() == nullptr);
        GridObject* existing_object = new SelfDestroyingObject();
        assert(game.Spawn(existing_object) == existing_object);
        game.Spawn(new RuntimeSpawner());
        HiddenObject* hidden = new HiddenObject();
        hidden->set_visible(false);
        game.Spawn(hidden);
        game.Spawn(new HideOnCollision());
        game.Spawn(new GridObject("", 0, 1));
        game.Spawn(new GridObject("", 0, 1));
        for (const auto [x, y] :
             {std::pair{-1, -1}, std::pair{-1, 0}, std::pair{0, -1}, std::pair{2, 0}, std::pair{0, 2}}) {
            game.Spawn(new OutOfBoundsCollisionCounter(x, y));
            game.Spawn(new GridObject("", x, y));
        }

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
        game.AddOverlay(new RuntimeDrawingOverlay(&game));
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
        assert(runtime_spawn_calls == 1);
        assert(hidden_updates == 2);
        assert(hidden_collisions == 0);
        assert(hidden_draws == 0);
        assert(hide_on_collision_calls == 1);
        assert(out_of_bounds_collisions == 0);
        assert(runtime_overlay_updates == 1);
        assert(runtime_overlay_draws == 1);
        assert(draw_added_overlay_updates == 1);
        assert(draw_added_overlay_draws == 1);
    }
    assert(engine_cleanup_destructors == 1);
    assert(overlay_destructors == 4);

    {
        GridEngine game(1, 1);
        bool spawn_failure_caught = false;
        try {
            game.Spawn(new ThrowingOnSpawn());
        } catch (const std::runtime_error&) {
            spawn_failure_caught = true;
        }
        assert(spawn_failure_caught);
        assert(failed_spawn_destructors == 1);
    }

    {
        GridEngine game(1, 1);
        game.Spawn(new RuntimeSpawnFailure());
        bool spawn_failure_caught = false;
        try {
            game.Run();
        } catch (const std::runtime_error&) {
            spawn_failure_caught = true;
        }
        assert(spawn_failure_caught);
        assert(failed_spawn_destructors == 2);
        assert(pending_spawn_calls == 0);

        game.Run();
        assert(pending_spawn_calls == 1);
    }
    assert(pending_spawn_destructors == 1);

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
