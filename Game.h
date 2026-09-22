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
#include "GridMaze.h"
#include "GridShapes.h"

namespace gridpp {

class Game;
class ObjectHandler;
class OverlayHandler;
class MazeHandler;

using ObjectFunction = void (*)(Game game, ObjectHandler self);
using CollisionFunction = void (*)(Game game, ObjectHandler self, ObjectHandler other);
using OverlayFunction = void (*)(Game game, OverlayHandler self);
using MazeFunction = void (*)(Game game, MazeHandler self);

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

    int get(const std::string& key, int& value) const;
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

/** Engine 內一個畫面覆蓋元素的存取憑證。 */
class OverlayHandler {
public:
    OverlayHandler() = default;

    bool exists() const;
    void remove();
    OverlayHandler deepCopy() const;

    int x() const;
    int y() const;
    void setPosition(int x, int y);
    void move(int dx, int dy);

    bool visible() const;
    void show();
    void hide();

    std::string image() const;
    void setImage(const std::string& image);
    std::string text() const;
    void setText(const std::string& text);
    void setColor(Color color);

    void set(const std::string& key, int value);
    void set(const std::string& key, long long value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, const char* value);

    int get(const std::string& key, int& value) const;
    int get(const std::string& key, long long& value) const;
    int get(const std::string& key, double& value) const;
    int get(const std::string& key, bool& value) const;
    int get(const std::string& key, std::string& value) const;

    void setInitFunction(OverlayFunction function);
    void setUpdateFunction(OverlayFunction function);
    void setClickFunction(OverlayFunction function);

private:
    friend class Game;
    friend class detail::GameState;

    OverlayHandler(std::weak_ptr<detail::GameState> state, std::uint64_t id);
    std::shared_ptr<detail::GameState> lockState() const;

    std::weak_ptr<detail::GameState> state_;
    std::uint64_t id_ = 0;
};

/** Engine 內一座迷宮的存取憑證。 */
class MazeHandler {
public:
    MazeHandler() = default;

    bool exists() const;
    void remove();
    MazeHandler deepCopy() const;

    void setWall(int x, int y, bool wall = true);
    bool isWall(int x, int y) const;
    void setWallImage(const std::string& image);
    void setWallImages(const std::string& isolated, const std::string& end, const std::string& straight,
                       const std::string& corner, const std::string& tee, const std::string& cross);

    int width() const;
    int height() const;

    void setInitFunction(MazeFunction function);
    void setUpdateFunction(MazeFunction function);

private:
    friend class detail::GameState;

    MazeHandler(std::weak_ptr<detail::GameState> state, std::uint64_t id);
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

    ObjectHandler addObject(const std::string& image, ObjectFunction init = nullptr, ObjectFunction update = nullptr,
                            CollisionFunction collide = nullptr);
    ObjectHandler addSquare(int x, int y, int size, Color color = BLACK, ObjectFunction init = nullptr,
                            ObjectFunction update = nullptr, CollisionFunction collide = nullptr);
    ObjectHandler addCircle(int x, int y, int size, Color color = BLACK, ObjectFunction init = nullptr,
                            ObjectFunction update = nullptr, CollisionFunction collide = nullptr);
    ObjectHandler addTriangle(int x, int y, int size, Color color = BLACK, ObjectFunction init = nullptr,
                              ObjectFunction update = nullptr, CollisionFunction collide = nullptr);
    ObjectHandler addPentagon(int x, int y, int size, Color color = BLACK, ObjectFunction init = nullptr,
                              ObjectFunction update = nullptr, CollisionFunction collide = nullptr);
    ObjectHandler addStar(int x, int y, int size, Color color = BLACK, ObjectFunction init = nullptr,
                          ObjectFunction update = nullptr, CollisionFunction collide = nullptr);
    MazeHandler addMaze(int cols, int rows, MazeFunction init = nullptr, MazeFunction update = nullptr);
    void clearObjects();

    OverlayHandler addOverlay(const std::string& image, OverlayFunction init = nullptr,
                              OverlayFunction update = nullptr);
    OverlayHandler addTextOverlay(const std::string& text, int x, int y, int font_size = 20, Color color = BLACK,
                                  OverlayFunction init = nullptr, OverlayFunction update = nullptr);
    OverlayHandler addButton(const std::string& text, int x, int y, int width, int height,
                             OverlayFunction click = nullptr);
    void clearOverlays();

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

template <typename ShapeType>
class HandlerShape : public ShapeType {
public:
    HandlerShape(GameState* state, std::uint64_t id, int x, int y, int size, Color color);
    HandlerShape(GameState* state, std::uint64_t id, const ShapeType& other);

    void OnUpdate() override;
    void OnCollide(GridObject* other) override;

private:
    GameState* state_;
    std::uint64_t id_;
};

class HandlerOverlay : public Overlay {
public:
    HandlerOverlay(GameState* state, std::uint64_t id);

    void OnUpdate() override;
    void Draw() override;

private:
    GameState* state_;
    std::uint64_t id_;
};

class HandlerMaze : public GridMaze {
public:
    HandlerMaze(GameState* state, std::uint64_t id, int cols, int rows);
    HandlerMaze(GameState* state, std::uint64_t id, const GridMaze& other);

    void OnUpdate() override;

private:
    GameState* state_;
    std::uint64_t id_;
};

enum class ObjectType {
    kImage,
    kSquare,
    kCircle,
    kTriangle,
    kPentagon,
    kStar,
};

struct ObjectRecord {
    GridObject* object = nullptr;
    ObjectType type = ObjectType::kImage;
    ObjectFunction init = nullptr;
    ObjectFunction update = nullptr;
    CollisionFunction collide = nullptr;
    std::unordered_map<std::string, StoredValue> values;
    bool initialized = false;
};

enum class OverlayType {
    kImage,
    kText,
    kButton,
};

struct OverlayRecord {
    Overlay* overlay = nullptr;
    OverlayType type = OverlayType::kImage;
    OverlayFunction init = nullptr;
    OverlayFunction update = nullptr;
    OverlayFunction click = nullptr;
    std::unordered_map<std::string, StoredValue> values;
    std::string content;
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int font_size = 20;
    Color color = BLACK;
    bool visible = true;
    bool hover = false;
    bool initialized = false;
};

struct MazeRecord {
    GridMaze* maze = nullptr;
    MazeFunction init = nullptr;
    MazeFunction update = nullptr;
    bool initialized = false;
};

class GameState : public std::enable_shared_from_this<GameState> {
public:
    GameState(int cols, int rows, int grid_size);

    ObjectHandler AddObject(const std::string& image, ObjectFunction init, ObjectFunction update,
                            CollisionFunction collide);
    ObjectHandler AddShape(ObjectType type, int x, int y, int size, Color color, ObjectFunction init,
                           ObjectFunction update, CollisionFunction collide);
    MazeHandler AddMaze(int cols, int rows, MazeFunction init, MazeFunction update);
    ObjectHandler CloneObject(std::uint64_t id);
    MazeHandler CloneMaze(std::uint64_t id);
    void RemoveObject(std::uint64_t id);
    void RemoveMaze(std::uint64_t id);
    void ClearObjects();

    OverlayHandler AddImageOverlay(const std::string& image, OverlayFunction init, OverlayFunction update);
    OverlayHandler AddTextOverlay(const std::string& text, int x, int y, int font_size, Color color,
                                  OverlayFunction init, OverlayFunction update);
    OverlayHandler AddButton(const std::string& text, int x, int y, int width, int height, OverlayFunction click);
    OverlayHandler CloneOverlay(std::uint64_t id);
    void RemoveOverlay(std::uint64_t id);
    void ClearOverlays();

    bool HasObject(std::uint64_t id) const;
    ObjectRecord& RequireObject(std::uint64_t id);
    const ObjectRecord& RequireObject(std::uint64_t id) const;
    bool HasMaze(std::uint64_t id) const;
    MazeRecord& RequireMaze(std::uint64_t id);
    const MazeRecord& RequireMaze(std::uint64_t id) const;
    bool HasOverlay(std::uint64_t id) const;
    OverlayRecord& RequireOverlay(std::uint64_t id);
    const OverlayRecord& RequireOverlay(std::uint64_t id) const;

    void UpdateObject(std::uint64_t id);
    void UpdateMaze(std::uint64_t id);
    void CollideObject(std::uint64_t id, GridObject* other);
    void UpdateOverlay(std::uint64_t id);
    void DrawOverlay(std::uint64_t id);
    void InitializePendingElements();

    Game PublicGame();
    ObjectHandler PublicObject(std::uint64_t id);
    MazeHandler PublicMaze(std::uint64_t id);
    OverlayHandler PublicOverlay(std::uint64_t id);

    GridEngine engine;

private:
    static void AfterTick(void* context);

    std::uint64_t AllocateId();
    ObjectHandler RegisterObject(std::uint64_t id, GridObject* object, ObjectFunction init, ObjectFunction update,
                                 CollisionFunction collide, bool initialized, ObjectType type = ObjectType::kImage,
                                 std::unordered_map<std::string, StoredValue> values = {});
    MazeHandler RegisterMaze(std::uint64_t id, GridMaze* maze, MazeFunction init, MazeFunction update,
                             bool initialized);
    GridObject* CreateShape(ObjectType type, std::uint64_t id, int x, int y, int size, Color color);
    GridObject* CloneShape(ObjectType type, std::uint64_t id, const GridObject& source);
    OverlayHandler RegisterOverlay(std::uint64_t id, Overlay* overlay, OverlayRecord record);
    void InitializeObject(std::uint64_t id);
    void InitializeMaze(std::uint64_t id);
    void InitializeOverlay(std::uint64_t id);
    void RemovePending(ElementType type, std::uint64_t id);

    std::uint64_t next_id_ = 1;
    std::unordered_map<std::uint64_t, ObjectRecord> objects_;
    std::unordered_map<GridObject*, std::uint64_t> object_ids_;
    std::unordered_map<std::uint64_t, MazeRecord> mazes_;
    std::unordered_map<std::uint64_t, OverlayRecord> overlays_;
    std::vector<ElementId> pending_init_;
    bool initializing_ = false;
};

template <typename Record, typename T>
inline void SetStoredValue(Record& record, const std::string& key, T value) {
    const auto it = record.values.find(key);
    if (it != record.values.end() && !std::holds_alternative<T>(it->second)) {
        throw std::runtime_error("Grid++ Error: Value '" + key + "' was already stored with a different type");
    }
    record.values[key] = std::move(value);
}

template <typename Record, typename T>
inline int GetStoredValue(const Record& record, const std::string& key, T& value) {
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

// HandlerShape

template <typename ShapeType>
inline HandlerShape<ShapeType>::HandlerShape(GameState* state, std::uint64_t id, int x, int y, int size, Color color)
    : ShapeType(x, y, size, color), state_(state), id_(id) {}

template <typename ShapeType>
inline HandlerShape<ShapeType>::HandlerShape(GameState* state, std::uint64_t id, const ShapeType& other)
    : ShapeType(other), state_(state), id_(id) {}

template <typename ShapeType>
inline void HandlerShape<ShapeType>::OnUpdate() {
    state_->UpdateObject(id_);
}

template <typename ShapeType>
inline void HandlerShape<ShapeType>::OnCollide(GridObject* other) {
    state_->CollideObject(id_, other);
}

// HandlerOverlay

inline HandlerOverlay::HandlerOverlay(GameState* state, std::uint64_t id) : state_(state), id_(id) {}

inline void HandlerOverlay::OnUpdate() { state_->UpdateOverlay(id_); }

inline void HandlerOverlay::Draw() { state_->DrawOverlay(id_); }

// HandlerMaze

inline HandlerMaze::HandlerMaze(GameState* state, std::uint64_t id, int cols, int rows)
    : GridMaze(cols, rows), state_(state), id_(id) {}

inline HandlerMaze::HandlerMaze(GameState* state, std::uint64_t id, const GridMaze& other)
    : GridMaze(other), state_(state), id_(id) {}

inline void HandlerMaze::OnUpdate() { state_->UpdateMaze(id_); }

// GameState

inline GameState::GameState(int cols, int rows, int grid_size) : engine(cols, rows, grid_size) {
    engine.SetAfterTickFunction(AfterTick, this);
}

inline ObjectHandler GameState::AddObject(const std::string& image, ObjectFunction init, ObjectFunction update,
                                          CollisionFunction collide) {
    const std::uint64_t id = AllocateId();
    return RegisterObject(id, new HandlerGridObject(this, id, image), init, update, collide, false);
}

inline ObjectHandler GameState::AddShape(ObjectType type, int x, int y, int size, Color color, ObjectFunction init,
                                         ObjectFunction update, CollisionFunction collide) {
    const std::uint64_t id = AllocateId();
    return RegisterObject(id, CreateShape(type, id, x, y, size, color), init, update, collide, false, type);
}

inline MazeHandler GameState::AddMaze(int cols, int rows, MazeFunction init, MazeFunction update) {
    const std::uint64_t id = AllocateId();
    return RegisterMaze(id, new HandlerMaze(this, id, cols, rows), init, update, false);
}

inline ObjectHandler GameState::CloneObject(std::uint64_t id) {
    const ObjectRecord& source = RequireObject(id);
    const std::uint64_t copy_id = AllocateId();
    GridObject* copy = source.type == ObjectType::kImage
                           ? static_cast<GridObject*>(new HandlerGridObject(this, copy_id, *source.object))
                           : CloneShape(source.type, copy_id, *source.object);
    return RegisterObject(copy_id, copy, source.init, source.update, source.collide, source.initialized, source.type,
                          source.values);
}

inline MazeHandler GameState::CloneMaze(std::uint64_t id) {
    const MazeRecord& source = RequireMaze(id);
    const std::uint64_t copy_id = AllocateId();
    return RegisterMaze(copy_id, new HandlerMaze(this, copy_id, *source.maze), source.init, source.update,
                        source.initialized);
}

inline void GameState::RemoveObject(std::uint64_t id) {
    const auto it = objects_.find(id);
    if (it == objects_.end()) return;

    GridObject* object = it->second.object;
    object_ids_.erase(object);
    objects_.erase(it);
    engine.Destroy(object);
}

inline void GameState::RemoveMaze(std::uint64_t id) {
    const auto it = mazes_.find(id);
    if (it == mazes_.end()) return;

    GridMaze* maze = it->second.maze;
    mazes_.erase(it);
    engine.Destroy(maze);
}

inline void GameState::ClearObjects() {
    objects_.clear();
    object_ids_.clear();
    mazes_.clear();
    engine.ClearObjects();
}

inline OverlayHandler GameState::AddImageOverlay(const std::string& image, OverlayFunction init,
                                                 OverlayFunction update) {
    const std::uint64_t id = AllocateId();
    OverlayRecord record;
    record.type = OverlayType::kImage;
    record.init = init;
    record.update = update;
    record.content = image;
    record.color = WHITE;
    return RegisterOverlay(id, new HandlerOverlay(this, id), std::move(record));
}

inline OverlayHandler GameState::AddTextOverlay(const std::string& text, int x, int y, int font_size, Color color,
                                                OverlayFunction init, OverlayFunction update) {
    if (font_size < 1) throw std::invalid_argument("Grid++ Error: Text size must be greater than 0");

    const std::uint64_t id = AllocateId();
    OverlayRecord record;
    record.type = OverlayType::kText;
    record.init = init;
    record.update = update;
    record.content = text;
    record.x = x;
    record.y = y;
    record.font_size = font_size;
    record.color = color;
    return RegisterOverlay(id, new HandlerOverlay(this, id), std::move(record));
}

inline OverlayHandler GameState::AddButton(const std::string& text, int x, int y, int width, int height,
                                           OverlayFunction click) {
    if (width < 1 || height < 1) {
        throw std::invalid_argument("Grid++ Error: Button width and height must be greater than 0");
    }

    const std::uint64_t id = AllocateId();
    OverlayRecord record;
    record.type = OverlayType::kButton;
    record.click = click;
    record.content = text;
    record.x = x;
    record.y = y;
    record.width = width;
    record.height = height;
    return RegisterOverlay(id, new HandlerOverlay(this, id), std::move(record));
}

inline OverlayHandler GameState::CloneOverlay(std::uint64_t id) {
    const OverlayRecord& source = RequireOverlay(id);
    const std::uint64_t copy_id = AllocateId();
    OverlayRecord copy = source;
    copy.overlay = nullptr;
    return RegisterOverlay(copy_id, new HandlerOverlay(this, copy_id), std::move(copy));
}

inline void GameState::RemoveOverlay(std::uint64_t id) {
    const auto it = overlays_.find(id);
    if (it == overlays_.end()) return;

    Overlay* overlay = it->second.overlay;
    overlays_.erase(it);
    engine.DestroyOverlay(overlay);
}

inline void GameState::ClearOverlays() {
    overlays_.clear();
    engine.ClearOverlays();
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

inline bool GameState::HasMaze(std::uint64_t id) const { return mazes_.count(id) != 0; }

inline MazeRecord& GameState::RequireMaze(std::uint64_t id) {
    const auto it = mazes_.find(id);
    if (it == mazes_.end()) throw std::runtime_error("Grid++ Error: MazeHandler no longer refers to a maze");
    return it->second;
}

inline const MazeRecord& GameState::RequireMaze(std::uint64_t id) const {
    const auto it = mazes_.find(id);
    if (it == mazes_.end()) throw std::runtime_error("Grid++ Error: MazeHandler no longer refers to a maze");
    return it->second;
}

inline bool GameState::HasOverlay(std::uint64_t id) const { return overlays_.count(id) != 0; }

inline OverlayRecord& GameState::RequireOverlay(std::uint64_t id) {
    const auto it = overlays_.find(id);
    if (it == overlays_.end()) throw std::runtime_error("Grid++ Error: OverlayHandler no longer refers to an overlay");
    return it->second;
}

inline const OverlayRecord& GameState::RequireOverlay(std::uint64_t id) const {
    const auto it = overlays_.find(id);
    if (it == overlays_.end()) throw std::runtime_error("Grid++ Error: OverlayHandler no longer refers to an overlay");
    return it->second;
}

inline void GameState::UpdateObject(std::uint64_t id) {
    const auto it = objects_.find(id);
    if (it == objects_.end() || it->second.update == nullptr) return;
    it->second.update(PublicGame(), PublicObject(id));
}

inline void GameState::UpdateMaze(std::uint64_t id) {
    const auto it = mazes_.find(id);
    if (it == mazes_.end() || it->second.update == nullptr) return;
    it->second.update(PublicGame(), PublicMaze(id));
}

inline void GameState::CollideObject(std::uint64_t id, GridObject* other) {
    const auto source = objects_.find(id);
    const auto target_id = object_ids_.find(other);
    if (source == objects_.end() || source->second.collide == nullptr || target_id == object_ids_.end()) return;
    source->second.collide(PublicGame(), PublicObject(id), PublicObject(target_id->second));
}

inline void GameState::UpdateOverlay(std::uint64_t id) {
    const auto it = overlays_.find(id);
    if (it == overlays_.end()) return;

    if (it->second.update != nullptr) it->second.update(PublicGame(), PublicOverlay(id));

    const auto current = overlays_.find(id);
    if (current == overlays_.end() || current->second.type != OverlayType::kButton || !current->second.visible) return;

    OverlayRecord& button = current->second;
    const Vector2 mouse = GetMousePosition();
    button.hover = mouse.x >= button.x && mouse.x <= button.x + button.width && mouse.y >= button.y &&
                   mouse.y <= button.y + button.height;
    if (button.hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && button.click != nullptr) {
        button.click(PublicGame(), PublicOverlay(id));
    }
}

inline void GameState::DrawOverlay(std::uint64_t id) {
    const auto it = overlays_.find(id);
    if (it == overlays_.end() || !it->second.visible) return;

    const OverlayRecord& overlay = it->second;
    if (overlay.type == OverlayType::kImage) {
        engine.DrawOverlayAsset(overlay.content, overlay.x, overlay.y, overlay.color);
        return;
    }
    if (overlay.type == OverlayType::kText) {
        DrawText(overlay.content.c_str(), overlay.x, overlay.y, overlay.font_size, overlay.color);
        return;
    }

    DrawRectangle(overlay.x, overlay.y, overlay.width, overlay.height, overlay.hover ? SKYBLUE : LIGHTGRAY);
    DrawRectangleLines(overlay.x, overlay.y, overlay.width, overlay.height, DARKGRAY);
    const int text_width = MeasureText(overlay.content.c_str(), overlay.font_size);
    DrawText(overlay.content.c_str(), overlay.x + (overlay.width - text_width) / 2,
             overlay.y + (overlay.height - overlay.font_size) / 2, overlay.font_size, overlay.color);
}

inline void GameState::InitializePendingElements() {
    if (initializing_) return;
    initializing_ = true;
    std::size_t index = 0;

    try {
        while (index < pending_init_.size()) {
            const ElementId element = pending_init_[index];
            ++index;
            if (element.type == ElementType::kObject)
                InitializeObject(element.id);
            else if (element.type == ElementType::kOverlay)
                InitializeOverlay(element.id);
            else if (element.type == ElementType::kMaze)
                InitializeMaze(element.id);
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

inline MazeHandler GameState::PublicMaze(std::uint64_t id) { return MazeHandler(shared_from_this(), id); }

inline OverlayHandler GameState::PublicOverlay(std::uint64_t id) { return OverlayHandler(shared_from_this(), id); }

inline void GameState::AfterTick(void* context) { static_cast<GameState*>(context)->InitializePendingElements(); }

inline std::uint64_t GameState::AllocateId() {
    if (next_id_ == std::numeric_limits<std::uint64_t>::max()) {
        throw std::overflow_error("Grid++ Error: Element ID limit reached");
    }
    return next_id_++;
}

inline GridObject* GameState::CreateShape(ObjectType type, std::uint64_t id, int x, int y, int size, Color color) {
    switch (type) {
        case ObjectType::kSquare:
            return new HandlerShape<shapes::Square>(this, id, x, y, size, color);
        case ObjectType::kCircle:
            return new HandlerShape<shapes::Circle>(this, id, x, y, size, color);
        case ObjectType::kTriangle:
            return new HandlerShape<shapes::Triangle>(this, id, x, y, size, color);
        case ObjectType::kPentagon:
            return new HandlerShape<shapes::Pentagon>(this, id, x, y, size, color);
        case ObjectType::kStar:
            return new HandlerShape<shapes::Star>(this, id, x, y, size, color);
        case ObjectType::kImage:
            throw std::logic_error("Grid++ Error: Image objects are not shapes");
    }
    throw std::logic_error("Grid++ Error: Unknown shape type");
}

inline GridObject* GameState::CloneShape(ObjectType type, std::uint64_t id, const GridObject& source) {
    switch (type) {
        case ObjectType::kSquare:
            return new HandlerShape<shapes::Square>(this, id, static_cast<const shapes::Square&>(source));
        case ObjectType::kCircle:
            return new HandlerShape<shapes::Circle>(this, id, static_cast<const shapes::Circle&>(source));
        case ObjectType::kTriangle:
            return new HandlerShape<shapes::Triangle>(this, id, static_cast<const shapes::Triangle&>(source));
        case ObjectType::kPentagon:
            return new HandlerShape<shapes::Pentagon>(this, id, static_cast<const shapes::Pentagon&>(source));
        case ObjectType::kStar:
            return new HandlerShape<shapes::Star>(this, id, static_cast<const shapes::Star&>(source));
        case ObjectType::kImage:
            throw std::logic_error("Grid++ Error: Image objects are not shapes");
    }
    throw std::logic_error("Grid++ Error: Unknown shape type");
}

inline ObjectHandler GameState::RegisterObject(std::uint64_t id, GridObject* object, ObjectFunction init,
                                               ObjectFunction update, CollisionFunction collide, bool initialized,
                                               ObjectType type, std::unordered_map<std::string, StoredValue> values) {
    engine.Spawn(object);

    try {
        objects_.emplace(id, ObjectRecord{object, type, init, update, collide, std::move(values), initialized});
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

inline MazeHandler GameState::RegisterMaze(std::uint64_t id, GridMaze* maze, MazeFunction init, MazeFunction update,
                                           bool initialized) {
    engine.Spawn(maze);

    try {
        mazes_.emplace(id, MazeRecord{maze, init, update, initialized});
        if (!initialized) pending_init_.push_back({ElementType::kMaze, id});
    } catch (...) {
        mazes_.erase(id);
        RemovePending(ElementType::kMaze, id);
        engine.Destroy(maze);
        throw;
    }

    return PublicMaze(id);
}

inline OverlayHandler GameState::RegisterOverlay(std::uint64_t id, Overlay* overlay, OverlayRecord record) {
    engine.AddOverlay(overlay);

    record.overlay = overlay;
    const bool initialized = record.initialized;
    try {
        overlays_.emplace(id, std::move(record));
        if (!initialized) pending_init_.push_back({ElementType::kOverlay, id});
    } catch (...) {
        overlays_.erase(id);
        RemovePending(ElementType::kOverlay, id);
        engine.DestroyOverlay(overlay);
        throw;
    }

    return PublicOverlay(id);
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

inline void GameState::InitializeMaze(std::uint64_t id) {
    const auto it = mazes_.find(id);
    if (it == mazes_.end() || it->second.initialized) return;

    it->second.initialized = true;
    if (it->second.init == nullptr) return;

    try {
        it->second.init(PublicGame(), PublicMaze(id));
    } catch (...) {
        RemoveMaze(id);
        throw;
    }
}

inline void GameState::InitializeOverlay(std::uint64_t id) {
    const auto it = overlays_.find(id);
    if (it == overlays_.end() || it->second.initialized) return;

    it->second.initialized = true;
    if (it->second.init == nullptr) return;

    try {
        it->second.init(PublicGame(), PublicOverlay(id));
    } catch (...) {
        RemoveOverlay(id);
        throw;
    }
}

inline void GameState::RemovePending(ElementType type, std::uint64_t id) {
    pending_init_.erase(
        std::remove_if(pending_init_.begin(), pending_init_.end(),
                       [&](const ElementId& element) { return element.type == type && element.id == id; }),
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

inline std::string ObjectHandler::image() const {
    const detail::ObjectRecord& record = lockState()->RequireObject(id_);
    if (record.type != detail::ObjectType::kImage) {
        throw std::runtime_error("Grid++ Error: image() requires an image object");
    }
    return record.object->asset_name();
}

inline void ObjectHandler::setImage(const std::string& image) {
    detail::ObjectRecord& record = lockState()->RequireObject(id_);
    if (record.type != detail::ObjectType::kImage) {
        throw std::runtime_error("Grid++ Error: setImage() requires an image object");
    }
    record.object->set_asset_name(image);
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

inline int ObjectHandler::get(const std::string& key, int& value) const {
    long long stored = 0;
    const int result = get(key, stored);
    if (stored < std::numeric_limits<int>::min() || stored > std::numeric_limits<int>::max()) {
        throw std::runtime_error("Grid++ Error: Value '" + key + "' does not fit in int");
    }
    value = static_cast<int>(stored);
    return result;
}

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

inline void ObjectHandler::setInitFunction(ObjectFunction function) { lockState()->RequireObject(id_).init = function; }

inline void ObjectHandler::setUpdateFunction(ObjectFunction function) {
    lockState()->RequireObject(id_).update = function;
}

inline void ObjectHandler::setCollideFunction(CollisionFunction function) {
    lockState()->RequireObject(id_).collide = function;
}

// OverlayHandler

inline OverlayHandler::OverlayHandler(std::weak_ptr<detail::GameState> state, std::uint64_t id)
    : state_(std::move(state)), id_(id) {}

inline std::shared_ptr<detail::GameState> OverlayHandler::lockState() const {
    std::shared_ptr<detail::GameState> state = state_.lock();
    if (state == nullptr) throw std::runtime_error("Grid++ Error: OverlayHandler's Game no longer exists");
    return state;
}

inline bool OverlayHandler::exists() const {
    const std::shared_ptr<detail::GameState> state = state_.lock();
    return state != nullptr && state->HasOverlay(id_);
}

inline void OverlayHandler::remove() {
    const std::shared_ptr<detail::GameState> state = lockState();
    state->RequireOverlay(id_);
    state->RemoveOverlay(id_);
}

inline OverlayHandler OverlayHandler::deepCopy() const { return lockState()->CloneOverlay(id_); }

inline int OverlayHandler::x() const { return lockState()->RequireOverlay(id_).x; }

inline int OverlayHandler::y() const { return lockState()->RequireOverlay(id_).y; }

inline void OverlayHandler::setPosition(int x, int y) {
    detail::OverlayRecord& overlay = lockState()->RequireOverlay(id_);
    overlay.x = x;
    overlay.y = y;
}

inline void OverlayHandler::move(int dx, int dy) {
    detail::OverlayRecord& overlay = lockState()->RequireOverlay(id_);
    overlay.x += dx;
    overlay.y += dy;
}

inline bool OverlayHandler::visible() const { return lockState()->RequireOverlay(id_).visible; }

inline void OverlayHandler::show() { lockState()->RequireOverlay(id_).visible = true; }

inline void OverlayHandler::hide() { lockState()->RequireOverlay(id_).visible = false; }

inline std::string OverlayHandler::image() const {
    const detail::OverlayRecord& overlay = lockState()->RequireOverlay(id_);
    if (overlay.type != detail::OverlayType::kImage) {
        throw std::runtime_error("Grid++ Error: image() requires an image overlay");
    }
    return overlay.content;
}

inline void OverlayHandler::setImage(const std::string& image) {
    detail::OverlayRecord& overlay = lockState()->RequireOverlay(id_);
    if (overlay.type != detail::OverlayType::kImage) {
        throw std::runtime_error("Grid++ Error: setImage() requires an image overlay");
    }
    overlay.content = image;
}

inline std::string OverlayHandler::text() const {
    const detail::OverlayRecord& overlay = lockState()->RequireOverlay(id_);
    if (overlay.type == detail::OverlayType::kImage) {
        throw std::runtime_error("Grid++ Error: text() requires a text overlay or button");
    }
    return overlay.content;
}

inline void OverlayHandler::setText(const std::string& text) {
    detail::OverlayRecord& overlay = lockState()->RequireOverlay(id_);
    if (overlay.type == detail::OverlayType::kImage) {
        throw std::runtime_error("Grid++ Error: setText() requires a text overlay or button");
    }
    overlay.content = text;
}

inline void OverlayHandler::setColor(Color color) { lockState()->RequireOverlay(id_).color = color; }

inline void OverlayHandler::set(const std::string& key, int value) { set(key, static_cast<long long>(value)); }

inline void OverlayHandler::set(const std::string& key, long long value) {
    detail::SetStoredValue(lockState()->RequireOverlay(id_), key, value);
}

inline void OverlayHandler::set(const std::string& key, double value) {
    detail::SetStoredValue(lockState()->RequireOverlay(id_), key, value);
}

inline void OverlayHandler::set(const std::string& key, bool value) {
    detail::SetStoredValue(lockState()->RequireOverlay(id_), key, value);
}

inline void OverlayHandler::set(const std::string& key, const std::string& value) {
    detail::SetStoredValue(lockState()->RequireOverlay(id_), key, value);
}

inline void OverlayHandler::set(const std::string& key, const char* value) { set(key, std::string(value)); }

inline int OverlayHandler::get(const std::string& key, int& value) const {
    long long stored = 0;
    const int result = get(key, stored);
    if (stored < std::numeric_limits<int>::min() || stored > std::numeric_limits<int>::max()) {
        throw std::runtime_error("Grid++ Error: Value '" + key + "' does not fit in int");
    }
    value = static_cast<int>(stored);
    return result;
}

inline int OverlayHandler::get(const std::string& key, long long& value) const {
    return detail::GetStoredValue(lockState()->RequireOverlay(id_), key, value);
}

inline int OverlayHandler::get(const std::string& key, double& value) const {
    return detail::GetStoredValue(lockState()->RequireOverlay(id_), key, value);
}

inline int OverlayHandler::get(const std::string& key, bool& value) const {
    return detail::GetStoredValue(lockState()->RequireOverlay(id_), key, value);
}

inline int OverlayHandler::get(const std::string& key, std::string& value) const {
    return detail::GetStoredValue(lockState()->RequireOverlay(id_), key, value);
}

inline void OverlayHandler::setInitFunction(OverlayFunction function) {
    lockState()->RequireOverlay(id_).init = function;
}

inline void OverlayHandler::setUpdateFunction(OverlayFunction function) {
    lockState()->RequireOverlay(id_).update = function;
}

inline void OverlayHandler::setClickFunction(OverlayFunction function) {
    detail::OverlayRecord& overlay = lockState()->RequireOverlay(id_);
    if (overlay.type != detail::OverlayType::kButton) {
        throw std::runtime_error("Grid++ Error: setClickFunction() requires a button");
    }
    overlay.click = function;
}

// MazeHandler

inline MazeHandler::MazeHandler(std::weak_ptr<detail::GameState> state, std::uint64_t id)
    : state_(std::move(state)), id_(id) {}

inline std::shared_ptr<detail::GameState> MazeHandler::lockState() const {
    std::shared_ptr<detail::GameState> state = state_.lock();
    if (state == nullptr) throw std::runtime_error("Grid++ Error: MazeHandler's Game no longer exists");
    return state;
}

inline bool MazeHandler::exists() const {
    const std::shared_ptr<detail::GameState> state = state_.lock();
    return state != nullptr && state->HasMaze(id_);
}

inline void MazeHandler::remove() {
    const std::shared_ptr<detail::GameState> state = lockState();
    state->RequireMaze(id_);
    state->RemoveMaze(id_);
}

inline MazeHandler MazeHandler::deepCopy() const { return lockState()->CloneMaze(id_); }

inline void MazeHandler::setWall(int x, int y, bool wall) { lockState()->RequireMaze(id_).maze->SetWall(x, y, wall); }

inline bool MazeHandler::isWall(int x, int y) const { return lockState()->RequireMaze(id_).maze->IsWall(x, y); }

inline void MazeHandler::setWallImage(const std::string& image) {
    lockState()->RequireMaze(id_).maze->SetWallAsset(image);
}

inline void MazeHandler::setWallImages(const std::string& isolated, const std::string& end, const std::string& straight,
                                       const std::string& corner, const std::string& tee, const std::string& cross) {
    lockState()->RequireMaze(id_).maze->SetWallTiles(isolated, end, straight, corner, tee, cross);
}

inline int MazeHandler::width() const { return lockState()->RequireMaze(id_).maze->width(); }

inline int MazeHandler::height() const { return lockState()->RequireMaze(id_).maze->height(); }

inline void MazeHandler::setInitFunction(MazeFunction function) { lockState()->RequireMaze(id_).init = function; }

inline void MazeHandler::setUpdateFunction(MazeFunction function) { lockState()->RequireMaze(id_).update = function; }

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

inline ObjectHandler Game::addSquare(int x, int y, int size, Color color, ObjectFunction init, ObjectFunction update,
                                     CollisionFunction collide) {
    return state_->AddShape(detail::ObjectType::kSquare, x, y, size, color, init, update, collide);
}

inline ObjectHandler Game::addCircle(int x, int y, int size, Color color, ObjectFunction init, ObjectFunction update,
                                     CollisionFunction collide) {
    return state_->AddShape(detail::ObjectType::kCircle, x, y, size, color, init, update, collide);
}

inline ObjectHandler Game::addTriangle(int x, int y, int size, Color color, ObjectFunction init, ObjectFunction update,
                                       CollisionFunction collide) {
    return state_->AddShape(detail::ObjectType::kTriangle, x, y, size, color, init, update, collide);
}

inline ObjectHandler Game::addPentagon(int x, int y, int size, Color color, ObjectFunction init, ObjectFunction update,
                                       CollisionFunction collide) {
    return state_->AddShape(detail::ObjectType::kPentagon, x, y, size, color, init, update, collide);
}

inline ObjectHandler Game::addStar(int x, int y, int size, Color color, ObjectFunction init, ObjectFunction update,
                                   CollisionFunction collide) {
    return state_->AddShape(detail::ObjectType::kStar, x, y, size, color, init, update, collide);
}

inline MazeHandler Game::addMaze(int cols, int rows, MazeFunction init, MazeFunction update) {
    return state_->AddMaze(cols, rows, init, update);
}

inline void Game::clearObjects() { state_->ClearObjects(); }

inline OverlayHandler Game::addOverlay(const std::string& image, OverlayFunction init, OverlayFunction update) {
    return state_->AddImageOverlay(image, init, update);
}

inline OverlayHandler Game::addTextOverlay(const std::string& text, int x, int y, int font_size, Color color,
                                           OverlayFunction init, OverlayFunction update) {
    return state_->AddTextOverlay(text, x, y, font_size, color, init, update);
}

inline OverlayHandler Game::addButton(const std::string& text, int x, int y, int width, int height,
                                      OverlayFunction click) {
    return state_->AddButton(text, x, y, width, height, click);
}

inline void Game::clearOverlays() { state_->ClearOverlays(); }

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
