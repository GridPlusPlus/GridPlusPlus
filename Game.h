/** @file Game.h
 *  @brief 定義不需要指標與繼承的 Grid++ 遊戲 API。
 */
#ifndef GRID_PLUS_PLUS_GAME_H_
#define GRID_PLUS_PLUS_GAME_H_

#include <algorithm>
#include <cstdint>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

#include "GridEngine.h"

namespace gridpp {

class Game;
class ObjectHandler;
class OverlayHandler;
class MazeHandler;

using ObjectFunction = void (*)(Game game, ObjectHandler self);
using CollisionFunction = void (*)(Game game, ObjectHandler self, ObjectHandler other);

namespace detail {
class GameState;
}

/** Engine 內一個網格物件的存取憑證。 */
class ObjectHandler {
public:
    ObjectHandler() = default;

    bool exists() const;
    void remove();
    ObjectHandler deepCopy() const;

    int x() const;
    int y() const;
    void setPosition(int x, int y);
    void move(int dx, int dy);

    std::string image() const;
    void setImage(const std::string& image);

    int direction() const;
    void setDirection(int direction);

    Color color() const;
    void setColor(Color color);

    int layer() const;
    void setLayer(int layer);

    bool visible() const;
    void show();
    void hide();

    void set(const std::string& key, int value);
    void set(const std::string& key, long long value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, const char* value);

    int get(const std::string& key, long long& value) const;
    int get(const std::string& key, double& value) const;
    int get(const std::string& key, bool& value) const;
    int get(const std::string& key, std::string& value) const;

    void setInitFunction(ObjectFunction function);
    void setUpdateFunction(ObjectFunction function);
    void setCollideFunction(CollisionFunction function);

private:
    friend class Game;
    friend class detail::GameState;

    ObjectHandler(std::weak_ptr<detail::GameState> state, std::uint64_t id);
    std::shared_ptr<detail::GameState> lockState() const;

    std::weak_ptr<detail::GameState> state_;
    std::uint64_t id_ = 0;
};

/** 遊戲的 value-like 公開入口；複本會操作同一個內部 Engine。 */
class Game {
public:
    Game(int cols, int rows, int grid_size = 32);

    void loadAssets(const std::string& database_path);
    void setBackgroundColor(Color color);
    void showGrid(bool show);

    int cols() const;
    int rows() const;
    int gridSize() const;

    ObjectHandler addObject(const std::string& image, ObjectFunction init = nullptr,
                            ObjectFunction update = nullptr, CollisionFunction collide = nullptr);
    void clearObjects();

    bool keyPressed(int key) const;
    bool keyDown(int key) const;
    bool mousePressed(int button) const;
    int mouseX() const;
    int mouseY() const;
    double time() const;
    int random(int min, int max) const;

    void run();

private:
    friend class ObjectHandler;
    friend class detail::GameState;

    explicit Game(std::shared_ptr<detail::GameState> state);

    std::shared_ptr<detail::GameState> state_;
};

namespace detail {

using StoredValue = std::variant<long long, double, bool, std::string>;

enum class ElementType {
    kObject,
    kOverlay,
    kMaze,
};

struct ElementId {
    ElementType type;
    std::uint64_t id;
};

class HandlerGridObject : public GridObject {
public:
    HandlerGridObject(GameState* state, std::uint64_t id, std::string image);
    HandlerGridObject(GameState* state, std::uint64_t id, const GridObject& other);

    void OnUpdate() override;
    void OnCollide(GridObject* other) override;

private:
    GameState* state_;
    std::uint64_t id_;
};

struct ObjectRecord {
    GridObject* object = nullptr;
    ObjectFunction init = nullptr;
    ObjectFunction update = nullptr;
    CollisionFunction collide = nullptr;
    std::unordered_map<std::string, StoredValue> values;
    bool initialized = false;
};

class GameState : public std::enable_shared_from_this<GameState> {
public:
    GameState(int cols, int rows, int grid_size);

    ObjectHandler AddObject(const std::string& image, ObjectFunction init, ObjectFunction update,
                            CollisionFunction collide);
    ObjectHandler CloneObject(std::uint64_t id);
    void RemoveObject(std::uint64_t id);
    void ClearObjects();

    bool HasObject(std::uint64_t id) const;
    ObjectRecord& RequireObject(std::uint64_t id);
    const ObjectRecord& RequireObject(std::uint64_t id) const;

    void UpdateObject(std::uint64_t id);
    void CollideObject(std::uint64_t id, GridObject* other);
    void InitializePendingElements();

    Game PublicGame();
    ObjectHandler PublicObject(std::uint64_t id);

    GridEngine engine;

private:
    static void AfterTick(void* context);

    std::uint64_t AllocateId();
    ObjectHandler RegisterObject(std::uint64_t id, GridObject* object, ObjectFunction init, ObjectFunction update,
                                 CollisionFunction collide, bool initialized,
                                 std::unordered_map<std::string, StoredValue> values = {});
    void InitializeObject(std::uint64_t id);
    void RemovePending(ElementType type, std::uint64_t id);

    std::uint64_t next_id_ = 1;
    std::unordered_map<std::uint64_t, ObjectRecord> objects_;
    std::unordered_map<GridObject*, std::uint64_t> object_ids_;
    std::vector<ElementId> pending_init_;
    bool initializing_ = false;
};

template <typename T>
inline void SetStoredValue(ObjectRecord& record, const std::string& key, T value) {
    const auto it = record.values.find(key);
    if (it != record.values.end() && !std::holds_alternative<T>(it->second)) {
        throw std::runtime_error("Grid++ Error: Value '" + key + "' was already stored with a different type");
    }
    record.values[key] = std::move(value);
}

template <typename T>
inline int GetStoredValue(const ObjectRecord& record, const std::string& key, T& value) {
    const auto it = record.values.find(key);
    if (it == record.values.end()) {
        value = T{};
        return 0;
    }

    const T* stored = std::get_if<T>(&it->second);
    if (stored == nullptr) {
        throw std::runtime_error("Grid++ Error: Value '" + key + "' was requested with the wrong type");
    }
    value = *stored;
    return 1;
}

// HandlerGridObject

inline HandlerGridObject::HandlerGridObject(GameState* state, std::uint64_t id, std::string image)
    : GridObject(std::move(image), 0, 0), state_(state), id_(id) {}

inline HandlerGridObject::HandlerGridObject(GameState* state, std::uint64_t id, const GridObject& other)
    : GridObject(other), state_(state), id_(id) {}

inline void HandlerGridObject::OnUpdate() { state_->UpdateObject(id_); }

inline void HandlerGridObject::OnCollide(GridObject* other) { state_->CollideObject(id_, other); }

// GameState

inline GameState::GameState(int cols, int rows, int grid_size) : engine(cols, rows, grid_size) {
    engine.SetAfterTickFunction(AfterTick, this);
}

inline ObjectHandler GameState::AddObject(const std::string& image, ObjectFunction init, ObjectFunction update,
                                          CollisionFunction collide) {
    const std::uint64_t id = AllocateId();
    return RegisterObject(id, new HandlerGridObject(this, id, image), init, update, collide, false);
}

inline ObjectHandler GameState::CloneObject(std::uint64_t id) {
    const ObjectRecord& source = RequireObject(id);
    const std::uint64_t copy_id = AllocateId();
    return RegisterObject(copy_id, new HandlerGridObject(this, copy_id, *source.object), source.init, source.update,
                          source.collide, source.initialized, source.values);
}

inline void GameState::RemoveObject(std::uint64_t id) {
    const auto it = objects_.find(id);
    if (it == objects_.end()) return;

    GridObject* object = it->second.object;
    object_ids_.erase(object);
    objects_.erase(it);
    engine.Destroy(object);
}

inline void GameState::ClearObjects() {
    objects_.clear();
    object_ids_.clear();
    engine.ClearObjects();
}

inline bool GameState::HasObject(std::uint64_t id) const { return objects_.count(id) != 0; }

inline ObjectRecord& GameState::RequireObject(std::uint64_t id) {
    const auto it = objects_.find(id);
    if (it == objects_.end()) throw std::runtime_error("Grid++ Error: ObjectHandler no longer refers to an object");
    return it->second;
}

inline const ObjectRecord& GameState::RequireObject(std::uint64_t id) const {
    const auto it = objects_.find(id);
    if (it == objects_.end()) throw std::runtime_error("Grid++ Error: ObjectHandler no longer refers to an object");
    return it->second;
}

inline void GameState::UpdateObject(std::uint64_t id) {
    const auto it = objects_.find(id);
    if (it == objects_.end() || it->second.update == nullptr) return;
    it->second.update(PublicGame(), PublicObject(id));
}

inline void GameState::CollideObject(std::uint64_t id, GridObject* other) {
    const auto source = objects_.find(id);
    const auto target_id = object_ids_.find(other);
    if (source == objects_.end() || source->second.collide == nullptr || target_id == object_ids_.end()) return;
    source->second.collide(PublicGame(), PublicObject(id), PublicObject(target_id->second));
}

inline void GameState::InitializePendingElements() {
    if (initializing_) return;
    initializing_ = true;
    std::size_t index = 0;

    try {
        while (index < pending_init_.size()) {
            const ElementId element = pending_init_[index];
            ++index;
            if (element.type == ElementType::kObject) InitializeObject(element.id);
        }
        pending_init_.clear();
        initializing_ = false;
    } catch (...) {
        pending_init_.erase(pending_init_.begin(), pending_init_.begin() + static_cast<std::ptrdiff_t>(index));
        initializing_ = false;
        throw;
    }
}

inline Game GameState::PublicGame() { return Game(shared_from_this()); }

inline ObjectHandler GameState::PublicObject(std::uint64_t id) { return ObjectHandler(shared_from_this(), id); }

inline void GameState::AfterTick(void* context) { static_cast<GameState*>(context)->InitializePendingElements(); }

inline std::uint64_t GameState::AllocateId() {
    if (next_id_ == std::numeric_limits<std::uint64_t>::max()) {
        throw std::overflow_error("Grid++ Error: Element ID limit reached");
    }
    return next_id_++;
}

inline ObjectHandler GameState::RegisterObject(std::uint64_t id, GridObject* object, ObjectFunction init,
                                               ObjectFunction update,
                                               CollisionFunction collide, bool initialized,
                                               std::unordered_map<std::string, StoredValue> values) {
    try {
        engine.Spawn(object);
    } catch (...) {
        throw;
    }

    try {
        objects_.emplace(id, ObjectRecord{object, init, update, collide, std::move(values), initialized});
        object_ids_.emplace(object, id);
        if (!initialized) pending_init_.push_back({ElementType::kObject, id});
    } catch (...) {
        object_ids_.erase(object);
        objects_.erase(id);
        RemovePending(ElementType::kObject, id);
        engine.Destroy(object);
        throw;
    }

    return PublicObject(id);
}

inline void GameState::InitializeObject(std::uint64_t id) {
    const auto it = objects_.find(id);
    if (it == objects_.end() || it->second.initialized) return;

    it->second.initialized = true;
    if (it->second.init == nullptr) return;

    try {
        it->second.init(PublicGame(), PublicObject(id));
    } catch (...) {
        RemoveObject(id);
        throw;
    }
}

inline void GameState::RemovePending(ElementType type, std::uint64_t id) {
    pending_init_.erase(std::remove_if(pending_init_.begin(), pending_init_.end(),
                                      [&](const ElementId& element) {
                                          return element.type == type && element.id == id;
                                      }),
                       pending_init_.end());
}

}  // namespace detail

// ObjectHandler

inline ObjectHandler::ObjectHandler(std::weak_ptr<detail::GameState> state, std::uint64_t id)
    : state_(std::move(state)), id_(id) {}

inline std::shared_ptr<detail::GameState> ObjectHandler::lockState() const {
    std::shared_ptr<detail::GameState> state = state_.lock();
    if (state == nullptr) throw std::runtime_error("Grid++ Error: ObjectHandler's Game no longer exists");
    return state;
}

inline bool ObjectHandler::exists() const {
    const std::shared_ptr<detail::GameState> state = state_.lock();
    return state != nullptr && state->HasObject(id_);
}

inline void ObjectHandler::remove() {
    const std::shared_ptr<detail::GameState> state = lockState();
    state->RequireObject(id_);
    state->RemoveObject(id_);
}

inline ObjectHandler ObjectHandler::deepCopy() const { return lockState()->CloneObject(id_); }

inline int ObjectHandler::x() const { return lockState()->RequireObject(id_).object->x(); }

inline int ObjectHandler::y() const { return lockState()->RequireObject(id_).object->y(); }

inline void ObjectHandler::setPosition(int x, int y) {
    const std::shared_ptr<detail::GameState> state = lockState();
    GridObject* object = state->RequireObject(id_).object;
    object->set_x(x);
    object->set_y(y);
}

inline void ObjectHandler::move(int dx, int dy) { lockState()->RequireObject(id_).object->Move(dx, dy); }

inline std::string ObjectHandler::image() const { return lockState()->RequireObject(id_).object->asset_name(); }

inline void ObjectHandler::setImage(const std::string& image) {
    lockState()->RequireObject(id_).object->set_asset_name(image);
}

inline int ObjectHandler::direction() const { return lockState()->RequireObject(id_).object->direction(); }

inline void ObjectHandler::setDirection(int direction) {
    lockState()->RequireObject(id_).object->set_direction(direction);
}

inline Color ObjectHandler::color() const { return lockState()->RequireObject(id_).object->tint(); }

inline void ObjectHandler::setColor(Color color) { lockState()->RequireObject(id_).object->set_tint(color); }

inline int ObjectHandler::layer() const { return lockState()->RequireObject(id_).object->z_index(); }

inline void ObjectHandler::setLayer(int layer) { lockState()->RequireObject(id_).object->set_z_index(layer); }

inline bool ObjectHandler::visible() const { return lockState()->RequireObject(id_).object->visible(); }

inline void ObjectHandler::show() { lockState()->RequireObject(id_).object->set_visible(true); }

inline void ObjectHandler::hide() { lockState()->RequireObject(id_).object->set_visible(false); }

inline void ObjectHandler::set(const std::string& key, int value) { set(key, static_cast<long long>(value)); }

inline void ObjectHandler::set(const std::string& key, long long value) {
    detail::SetStoredValue(lockState()->RequireObject(id_), key, value);
}

inline void ObjectHandler::set(const std::string& key, double value) {
    detail::SetStoredValue(lockState()->RequireObject(id_), key, value);
}

inline void ObjectHandler::set(const std::string& key, bool value) {
    detail::SetStoredValue(lockState()->RequireObject(id_), key, value);
}

inline void ObjectHandler::set(const std::string& key, const std::string& value) {
    detail::SetStoredValue(lockState()->RequireObject(id_), key, value);
}

inline void ObjectHandler::set(const std::string& key, const char* value) { set(key, std::string(value)); }

inline int ObjectHandler::get(const std::string& key, long long& value) const {
    return detail::GetStoredValue(lockState()->RequireObject(id_), key, value);
}

inline int ObjectHandler::get(const std::string& key, double& value) const {
    return detail::GetStoredValue(lockState()->RequireObject(id_), key, value);
}

inline int ObjectHandler::get(const std::string& key, bool& value) const {
    return detail::GetStoredValue(lockState()->RequireObject(id_), key, value);
}

inline int ObjectHandler::get(const std::string& key, std::string& value) const {
    return detail::GetStoredValue(lockState()->RequireObject(id_), key, value);
}

inline void ObjectHandler::setInitFunction(ObjectFunction function) {
    lockState()->RequireObject(id_).init = function;
}

inline void ObjectHandler::setUpdateFunction(ObjectFunction function) {
    lockState()->RequireObject(id_).update = function;
}

inline void ObjectHandler::setCollideFunction(CollisionFunction function) {
    lockState()->RequireObject(id_).collide = function;
}

// Game

inline Game::Game(int cols, int rows, int grid_size)
    : state_(std::make_shared<detail::GameState>(cols, rows, grid_size)) {}

inline Game::Game(std::shared_ptr<detail::GameState> state) : state_(std::move(state)) {}

inline void Game::loadAssets(const std::string& database_path) { state_->engine.LoadAssets(database_path); }

inline void Game::setBackgroundColor(Color color) { state_->engine.set_background_color(color); }

inline void Game::showGrid(bool show) { state_->engine.set_show_grid(show); }

inline int Game::cols() const { return state_->engine.cols(); }

inline int Game::rows() const { return state_->engine.rows(); }

inline int Game::gridSize() const { return state_->engine.grid_size(); }

inline ObjectHandler Game::addObject(const std::string& image, ObjectFunction init, ObjectFunction update,
                                     CollisionFunction collide) {
    return state_->AddObject(image, init, update, collide);
}

inline void Game::clearObjects() { state_->ClearObjects(); }

inline bool Game::keyPressed(int key) const { return IsKeyPressed(key); }

inline bool Game::keyDown(int key) const { return IsKeyDown(key); }

inline bool Game::mousePressed(int button) const { return IsMouseButtonPressed(button); }

inline int Game::mouseX() const { return static_cast<int>(GetMousePosition().x); }

inline int Game::mouseY() const { return static_cast<int>(GetMousePosition().y); }

inline double Game::time() const { return GetTime(); }

inline int Game::random(int min, int max) const { return GetRandomValue(min, max); }

inline void Game::run() {
    state_->InitializePendingElements();
    state_->engine.Run();
}

}  // namespace gridpp

#endif  // GRID_PLUS_PLUS_GAME_H_
