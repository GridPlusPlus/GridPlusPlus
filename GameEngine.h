/** @file GameEngine.h
 *  @brief 定義 Grid++ 的公開 GameEngine handler 與實際引擎。
 */
#ifndef GRID_PLUS_PLUS_GAME_ENGINE_H_
#define GRID_PLUS_PLUS_GAME_ENGINE_H_

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "GridAssetManager.h"
#include "GridMaze.h"
#include "GridObject.h"
#include "Overlay.h"
#include "raylib.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

namespace gridpp {

/** @cond */
class GameEngineImpl;
/** @endcond */

/** 學生使用的遊戲入口；複本仍操作同一個實際引擎。 */
class GameEngine {
public:
    GameEngine(int cols, int rows, int grid_size = 32);

    void loadAssets(const std::string& database_path);
    void setBackgroundColor(Color color);
    void showGrid(bool show);

    int cols() const;
    int rows() const;
    int gridSize() const;

    GridObject addObject(const std::string& image, GridObjectCallback init = nullptr,
                         GridObjectCallback update = nullptr, CollisionCallback collide = nullptr);
    GridObject addSquare(int x, int y, int size, Color color = BLACK, GridObjectCallback init = nullptr,
                         GridObjectCallback update = nullptr, CollisionCallback collide = nullptr);
    GridObject addCircle(int x, int y, int size, Color color = BLACK, GridObjectCallback init = nullptr,
                         GridObjectCallback update = nullptr, CollisionCallback collide = nullptr);
    GridObject addTriangle(int x, int y, int size, Color color = BLACK, GridObjectCallback init = nullptr,
                           GridObjectCallback update = nullptr, CollisionCallback collide = nullptr);
    GridObject addPentagon(int x, int y, int size, Color color = BLACK, GridObjectCallback init = nullptr,
                           GridObjectCallback update = nullptr, CollisionCallback collide = nullptr);
    GridObject addStar(int x, int y, int size, Color color = BLACK, GridObjectCallback init = nullptr,
                       GridObjectCallback update = nullptr, CollisionCallback collide = nullptr);
    Maze addMaze(int cols, int rows, MazeCallback init = nullptr, MazeCallback update = nullptr);
    void clearObjects();

    Overlay addOverlay(const std::string& image, OverlayCallback init = nullptr, OverlayCallback update = nullptr);
    Overlay addTextOverlay(const std::string& text, int x, int y, int font_size = 20, Color color = BLACK,
                           OverlayCallback init = nullptr, OverlayCallback update = nullptr);
    Overlay addButton(const std::string& text, int x, int y, int width, int height, OverlayCallback click = nullptr);
    void clearOverlays();

    bool keyPressed(int key) const;
    bool keyDown(int key) const;
    bool mousePressed(int button) const;
    int mouseX() const;
    int mouseY() const;
    double time() const;
    int random(int min, int max) const;

    void run();

    /** @cond */
private:
    friend class GameEngineImpl;

    explicit GameEngine(std::shared_ptr<GameEngineImpl> impl);
    std::shared_ptr<GameEngineImpl> impl_;
    /** @endcond */
};

/** @cond */
class GameEngineImpl : public std::enable_shared_from_this<GameEngineImpl> {
public:
    static constexpr int kMaxWindowSize = 8192;
    ~GameEngineImpl();

private:
    friend class GameEngine;
    friend class GridObject;
    friend class Overlay;
    friend class Maze;

    enum class WorldType { Object, Maze };

    struct WorldId {
        WorldType type;
        std::uint64_t id;
    };

    GameEngineImpl(int cols, int rows, int grid_size);

    std::uint64_t allocateId();
    GameEngine publicEngine();
    GridObject publicObject(std::uint64_t id);
    Overlay publicOverlay(std::uint64_t id);
    Maze publicMaze(std::uint64_t id);

    bool hasObject(std::uint64_t id) const;
    bool hasOverlay(std::uint64_t id) const;
    bool hasMaze(std::uint64_t id) const;
    GridObjectImpl& requireObject(std::uint64_t id);
    const GridObjectImpl& requireObject(std::uint64_t id) const;
    OverlayImpl& requireOverlay(std::uint64_t id);
    const OverlayImpl& requireOverlay(std::uint64_t id) const;
    MazeImpl& requireMaze(std::uint64_t id);
    const MazeImpl& requireMaze(std::uint64_t id) const;

    GridObject addObject(const std::string& image, GridObjectCallback init, GridObjectCallback update,
                         CollisionCallback collide);
    GridObject addShape(GridObjectType type, int x, int y, int size, Color color, GridObjectCallback init,
                        GridObjectCallback update, CollisionCallback collide);
    Overlay addImageOverlay(const std::string& image, OverlayCallback init, OverlayCallback update);
    Overlay addTextOverlay(const std::string& text, int x, int y, int font_size, Color color, OverlayCallback init,
                           OverlayCallback update);
    Overlay addButton(const std::string& text, int x, int y, int width, int height, OverlayCallback click);
    Maze addMaze(int cols, int rows, MazeCallback init, MazeCallback update);

    GridObject cloneObject(std::uint64_t id);
    Overlay cloneOverlay(std::uint64_t id);
    Maze cloneMaze(std::uint64_t id);
    void removeObject(std::uint64_t id);
    void removeOverlay(std::uint64_t id);
    void removeMaze(std::uint64_t id);
    void clearObjects();
    void clearOverlays();

    void initializePending();
    void initializeWorld(WorldId item);
    void initializeOverlay(std::uint64_t id);
    void updateWorld(const std::vector<WorldId>& frame_world);
    void collideObjects(const std::vector<WorldId>& frame_world);
    void updateOverlays(const std::vector<std::uint64_t>& frame_overlays);
    void draw(const std::vector<WorldId>& frame_world, const std::vector<std::uint64_t>& frame_overlays);
    void drawObject(const GridObjectImpl& object);
    void drawMaze(const MazeImpl& maze);
    void drawOverlay(const OverlayImpl& overlay);
    void drawCell(const std::string& image, int grid_x, int grid_y, int direction = 0, Color tint = WHITE);
    void drawOverlayImage(const std::string& image, int pixel_x, int pixel_y, Color tint);
    void drawGrid();
    void tick();
    void run();

    static void eraseWorld(std::vector<WorldId>& source, WorldType type, std::uint64_t id);
    static void eraseId(std::vector<std::uint64_t>& source, std::uint64_t id);
    int worldLayer(const WorldId& item) const;
    bool worldActive(const WorldId& item) const;

    int cols_;
    int rows_;
    int grid_size_;
    Color background_color_ = RAYWHITE;
    bool show_grid_ = false;
    GridAssetManager assets_;
    std::uint64_t next_id_ = 1;
    std::unordered_map<std::uint64_t, GridObjectImpl> objects_;
    std::unordered_map<std::uint64_t, OverlayImpl> overlays_;
    std::unordered_map<std::uint64_t, MazeImpl> mazes_;
    std::vector<WorldId> world_order_;
    std::vector<std::uint64_t> overlay_order_;
    std::vector<WorldId> pending_world_;
    std::vector<std::uint64_t> pending_overlays_;
};

// GameEngineImpl construction and lookup

inline GameEngineImpl::GameEngineImpl(int cols, int rows, int grid_size)
    : cols_(cols), rows_(rows), grid_size_(grid_size) {
    if (cols < 1 || rows < 1 || grid_size < 1) {
        throw std::invalid_argument("Grid++ Error: cols, rows, and grid size must be greater than 0");
    }
    const std::int64_t width = static_cast<std::int64_t>(cols) * grid_size;
    const std::int64_t height = static_cast<std::int64_t>(rows) * grid_size;
    if (width > kMaxWindowSize || height > kMaxWindowSize) {
        throw std::invalid_argument("Grid++ Error: window width and height must not exceed 8192 pixels");
    }
    InitWindow(static_cast<int>(width), static_cast<int>(height), "Grid++ Game");
    SetTargetFPS(60);
}

inline GameEngineImpl::~GameEngineImpl() {
    assets_.Clear();
    if (IsWindowReady()) CloseWindow();
}

inline std::uint64_t GameEngineImpl::allocateId() {
    if (next_id_ == std::numeric_limits<std::uint64_t>::max()) {
        throw std::overflow_error("Grid++ Error: no more element IDs are available");
    }
    return next_id_++;
}

inline GameEngine GameEngineImpl::publicEngine() { return GameEngine(shared_from_this()); }
inline GridObject GameEngineImpl::publicObject(std::uint64_t id) { return GridObject(shared_from_this(), id); }
inline Overlay GameEngineImpl::publicOverlay(std::uint64_t id) { return Overlay(shared_from_this(), id); }
inline Maze GameEngineImpl::publicMaze(std::uint64_t id) { return Maze(shared_from_this(), id); }

inline bool GameEngineImpl::hasObject(std::uint64_t id) const { return objects_.count(id) != 0; }
inline bool GameEngineImpl::hasOverlay(std::uint64_t id) const { return overlays_.count(id) != 0; }
inline bool GameEngineImpl::hasMaze(std::uint64_t id) const { return mazes_.count(id) != 0; }

inline GridObjectImpl& GameEngineImpl::requireObject(std::uint64_t id) {
    const auto found = objects_.find(id);
    if (found == objects_.end()) throw std::runtime_error("Grid++ Error: GridObject no longer exists");
    return found->second;
}

inline const GridObjectImpl& GameEngineImpl::requireObject(std::uint64_t id) const {
    const auto found = objects_.find(id);
    if (found == objects_.end()) throw std::runtime_error("Grid++ Error: GridObject no longer exists");
    return found->second;
}

inline OverlayImpl& GameEngineImpl::requireOverlay(std::uint64_t id) {
    const auto found = overlays_.find(id);
    if (found == overlays_.end()) throw std::runtime_error("Grid++ Error: Overlay no longer exists");
    return found->second;
}

inline const OverlayImpl& GameEngineImpl::requireOverlay(std::uint64_t id) const {
    const auto found = overlays_.find(id);
    if (found == overlays_.end()) throw std::runtime_error("Grid++ Error: Overlay no longer exists");
    return found->second;
}

inline MazeImpl& GameEngineImpl::requireMaze(std::uint64_t id) {
    const auto found = mazes_.find(id);
    if (found == mazes_.end()) throw std::runtime_error("Grid++ Error: Maze no longer exists");
    return found->second;
}

inline const MazeImpl& GameEngineImpl::requireMaze(std::uint64_t id) const {
    const auto found = mazes_.find(id);
    if (found == mazes_.end()) throw std::runtime_error("Grid++ Error: Maze no longer exists");
    return found->second;
}

// Creation and ownership

inline GridObject GameEngineImpl::addObject(const std::string& image, GridObjectCallback init,
                                            GridObjectCallback update, CollisionCallback collide) {
    const std::uint64_t id = allocateId();
    GridObjectImpl object(image);
    object.init_ = init;
    object.update_ = update;
    object.collide_ = collide;
    objects_.emplace(id, std::move(object));
    world_order_.push_back({WorldType::Object, id});
    pending_world_.push_back({WorldType::Object, id});
    return publicObject(id);
}

inline GridObject GameEngineImpl::addShape(GridObjectType type, int x, int y, int size, Color color,
                                           GridObjectCallback init, GridObjectCallback update,
                                           CollisionCallback collide) {
    if (size < 1) throw std::invalid_argument("Grid++ Error: shape size must be greater than 0");
    const std::uint64_t id = allocateId();
    GridObjectImpl object(type, x, y, size, color);
    object.init_ = init;
    object.update_ = update;
    object.collide_ = collide;
    objects_.emplace(id, std::move(object));
    world_order_.push_back({WorldType::Object, id});
    pending_world_.push_back({WorldType::Object, id});
    return publicObject(id);
}

inline Overlay GameEngineImpl::addImageOverlay(const std::string& image, OverlayCallback init, OverlayCallback update) {
    const std::uint64_t id = allocateId();
    OverlayImpl overlay(OverlayType::Image);
    overlay.content_ = image;
    overlay.color_ = WHITE;
    overlay.init_ = init;
    overlay.update_ = update;
    overlays_.emplace(id, std::move(overlay));
    overlay_order_.push_back(id);
    pending_overlays_.push_back(id);
    return publicOverlay(id);
}

inline Overlay GameEngineImpl::addTextOverlay(const std::string& text, int x, int y, int font_size, Color color,
                                              OverlayCallback init, OverlayCallback update) {
    if (font_size < 1) throw std::invalid_argument("Grid++ Error: text size must be greater than 0");
    const std::uint64_t id = allocateId();
    OverlayImpl overlay(OverlayType::Text);
    overlay.content_ = text;
    overlay.x_ = x;
    overlay.y_ = y;
    overlay.font_size_ = font_size;
    overlay.color_ = color;
    overlay.init_ = init;
    overlay.update_ = update;
    overlays_.emplace(id, std::move(overlay));
    overlay_order_.push_back(id);
    pending_overlays_.push_back(id);
    return publicOverlay(id);
}

inline Overlay GameEngineImpl::addButton(const std::string& text, int x, int y, int width, int height,
                                         OverlayCallback click) {
    if (width < 1 || height < 1) {
        throw std::invalid_argument("Grid++ Error: button width and height must be greater than 0");
    }
    const std::uint64_t id = allocateId();
    OverlayImpl overlay(OverlayType::Button);
    overlay.content_ = text;
    overlay.x_ = x;
    overlay.y_ = y;
    overlay.width_ = width;
    overlay.height_ = height;
    overlay.click_ = click;
    overlays_.emplace(id, std::move(overlay));
    overlay_order_.push_back(id);
    pending_overlays_.push_back(id);
    return publicOverlay(id);
}

inline Maze GameEngineImpl::addMaze(int cols, int rows, MazeCallback init, MazeCallback update) {
    const std::uint64_t id = allocateId();
    MazeImpl maze(cols, rows);
    maze.init_ = init;
    maze.update_ = update;
    mazes_.emplace(id, std::move(maze));
    world_order_.push_back({WorldType::Maze, id});
    pending_world_.push_back({WorldType::Maze, id});
    return publicMaze(id);
}

inline GridObject GameEngineImpl::cloneObject(std::uint64_t id) {
    GridObjectImpl copy = requireObject(id);
    copy.active_ = false;
    const std::uint64_t copy_id = allocateId();
    objects_.emplace(copy_id, std::move(copy));
    world_order_.push_back({WorldType::Object, copy_id});
    pending_world_.push_back({WorldType::Object, copy_id});
    return publicObject(copy_id);
}

inline Overlay GameEngineImpl::cloneOverlay(std::uint64_t id) {
    OverlayImpl copy = requireOverlay(id);
    copy.active_ = false;
    const std::uint64_t copy_id = allocateId();
    overlays_.emplace(copy_id, std::move(copy));
    overlay_order_.push_back(copy_id);
    pending_overlays_.push_back(copy_id);
    return publicOverlay(copy_id);
}

inline Maze GameEngineImpl::cloneMaze(std::uint64_t id) {
    MazeImpl copy = requireMaze(id);
    copy.active_ = false;
    const std::uint64_t copy_id = allocateId();
    mazes_.emplace(copy_id, std::move(copy));
    world_order_.push_back({WorldType::Maze, copy_id});
    pending_world_.push_back({WorldType::Maze, copy_id});
    return publicMaze(copy_id);
}

inline void GameEngineImpl::eraseWorld(std::vector<WorldId>& source, WorldType type, std::uint64_t id) {
    source.erase(std::remove_if(source.begin(), source.end(),
                                [&](const WorldId& item) { return item.type == type && item.id == id; }),
                 source.end());
}

inline void GameEngineImpl::eraseId(std::vector<std::uint64_t>& source, std::uint64_t id) {
    source.erase(std::remove(source.begin(), source.end(), id), source.end());
}

inline void GameEngineImpl::removeObject(std::uint64_t id) {
    if (objects_.erase(id) == 0) return;
    eraseWorld(world_order_, WorldType::Object, id);
    eraseWorld(pending_world_, WorldType::Object, id);
}

inline void GameEngineImpl::removeOverlay(std::uint64_t id) {
    if (overlays_.erase(id) == 0) return;
    eraseId(overlay_order_, id);
    eraseId(pending_overlays_, id);
}

inline void GameEngineImpl::removeMaze(std::uint64_t id) {
    if (mazes_.erase(id) == 0) return;
    eraseWorld(world_order_, WorldType::Maze, id);
    eraseWorld(pending_world_, WorldType::Maze, id);
}

inline void GameEngineImpl::clearObjects() {
    objects_.clear();
    mazes_.clear();
    world_order_.clear();
    pending_world_.clear();
}

inline void GameEngineImpl::clearOverlays() {
    overlays_.clear();
    overlay_order_.clear();
    pending_overlays_.clear();
}

// Lifecycle

inline void GameEngineImpl::initializeWorld(WorldId item) {
    if (item.type == WorldType::Object) {
        const auto found = objects_.find(item.id);
        if (found == objects_.end()) return;
        if (!found->second.initialized_) {
            found->second.initialized_ = true;
            const GridObjectCallback callback = found->second.init_;
            if (callback != nullptr) callback(publicEngine(), publicObject(item.id));
        }
        const auto current = objects_.find(item.id);
        if (current != objects_.end()) current->second.active_ = true;
        return;
    }

    const auto found = mazes_.find(item.id);
    if (found == mazes_.end()) return;
    if (!found->second.initialized_) {
        found->second.initialized_ = true;
        const MazeCallback callback = found->second.init_;
        if (callback != nullptr) callback(publicEngine(), publicMaze(item.id));
    }
    const auto current = mazes_.find(item.id);
    if (current != mazes_.end()) current->second.active_ = true;
}

inline void GameEngineImpl::initializeOverlay(std::uint64_t id) {
    const auto found = overlays_.find(id);
    if (found == overlays_.end()) return;
    if (!found->second.initialized_) {
        found->second.initialized_ = true;
        const OverlayCallback callback = found->second.init_;
        if (callback != nullptr) callback(publicEngine(), publicOverlay(id));
    }
    const auto current = overlays_.find(id);
    if (current != overlays_.end()) current->second.active_ = true;
}

inline void GameEngineImpl::initializePending() {
    while (!pending_world_.empty() || !pending_overlays_.empty()) {
        std::vector<WorldId> world = std::move(pending_world_);
        std::vector<std::uint64_t> overlays = std::move(pending_overlays_);
        pending_world_.clear();
        pending_overlays_.clear();
        for (const WorldId item : world) initializeWorld(item);
        for (const std::uint64_t id : overlays) initializeOverlay(id);
    }
}

inline void GameEngineImpl::updateWorld(const std::vector<WorldId>& frame_world) {
    for (const WorldId item : frame_world) {
        if (item.type == WorldType::Object) {
            const auto found = objects_.find(item.id);
            if (found == objects_.end() || !found->second.active_) continue;
            const GridObjectCallback callback = found->second.update_;
            if (callback != nullptr) callback(publicEngine(), publicObject(item.id));
        } else {
            const auto found = mazes_.find(item.id);
            if (found == mazes_.end() || !found->second.active_) continue;
            const MazeCallback callback = found->second.update_;
            if (callback != nullptr) callback(publicEngine(), publicMaze(item.id));
        }
    }
}

inline void GameEngineImpl::collideObjects(const std::vector<WorldId>& frame_world) {
    std::vector<std::uint64_t> ids;
    for (const WorldId item : frame_world) {
        if (item.type == WorldType::Object) ids.push_back(item.id);
    }

    const auto can_collide = [&](std::uint64_t id) {
        const auto found = objects_.find(id);
        return found != objects_.end() && found->second.active_ && found->second.visible_ && found->second.x_ >= 0 &&
               found->second.x_ < cols_ && found->second.y_ >= 0 && found->second.y_ < rows_;
    };

    for (std::size_t i = 0; i < ids.size(); ++i) {
        if (!can_collide(ids[i])) continue;
        for (std::size_t j = i + 1; j < ids.size(); ++j) {
            if (!can_collide(ids[j])) continue;
            const GridObjectImpl& left = objects_.find(ids[i])->second;
            const GridObjectImpl& right = objects_.find(ids[j])->second;
            if (left.x_ != right.x_ || left.y_ != right.y_) continue;

            const CollisionCallback left_callback = left.collide_;
            if (left_callback != nullptr) {
                left_callback(publicEngine(), publicObject(ids[i]), publicObject(ids[j]));
            }
            if (!can_collide(ids[i])) break;
            if (!can_collide(ids[j])) continue;

            const CollisionCallback right_callback = objects_.find(ids[j])->second.collide_;
            if (right_callback != nullptr) {
                right_callback(publicEngine(), publicObject(ids[j]), publicObject(ids[i]));
            }
            if (!can_collide(ids[i])) break;
        }
    }
}

inline void GameEngineImpl::updateOverlays(const std::vector<std::uint64_t>& frame_overlays) {
    for (const std::uint64_t id : frame_overlays) {
        const auto found = overlays_.find(id);
        if (found == overlays_.end() || !found->second.active_) continue;
        const OverlayCallback update = found->second.update_;
        if (update != nullptr) update(publicEngine(), publicOverlay(id));

        const auto current = overlays_.find(id);
        if (current == overlays_.end() || !current->second.active_ || !current->second.visible_ ||
            current->second.type_ != OverlayType::Button) {
            continue;
        }
        OverlayImpl& button = current->second;
        const Vector2 mouse = GetMousePosition();
        button.hover_ = mouse.x >= button.x_ && mouse.x <= button.x_ + button.width_ && mouse.y >= button.y_ &&
                        mouse.y <= button.y_ + button.height_;
        const OverlayCallback click = button.click_;
        if (button.hover_ && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && click != nullptr) {
            click(publicEngine(), publicOverlay(id));
        }
    }
}

inline int GameEngineImpl::worldLayer(const WorldId& item) const {
    if (item.type == WorldType::Maze) return 0;
    const auto found = objects_.find(item.id);
    return found == objects_.end() ? 0 : found->second.layer_;
}

inline bool GameEngineImpl::worldActive(const WorldId& item) const {
    if (item.type == WorldType::Object) {
        const auto found = objects_.find(item.id);
        return found != objects_.end() && found->second.active_;
    }
    const auto found = mazes_.find(item.id);
    return found != mazes_.end() && found->second.active_;
}

inline void GameEngineImpl::tick() {
    const std::vector<WorldId> frame_world = world_order_;
    const std::vector<std::uint64_t> frame_overlays = overlay_order_;
    updateWorld(frame_world);
    collideObjects(frame_world);
    updateOverlays(frame_overlays);
    draw(frame_world, frame_overlays);
    initializePending();
}

inline void GameEngineImpl::run() {
    initializePending();  // tick 0
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg([](void* self) { static_cast<GameEngineImpl*>(self)->tick(); }, this, 0, 1);
#else
    while (!WindowShouldClose()) tick();
#endif
}

// Drawing

inline void GameEngineImpl::draw(const std::vector<WorldId>& frame_world,
                                 const std::vector<std::uint64_t>& frame_overlays) {
    BeginDrawing();
    ClearBackground(background_color_);
    if (show_grid_) drawGrid();

    std::vector<WorldId> draw_order;
    for (const WorldId item : frame_world) {
        if (worldActive(item)) draw_order.push_back(item);
    }
    std::stable_sort(draw_order.begin(), draw_order.end(),
                     [&](const WorldId& left, const WorldId& right) { return worldLayer(left) < worldLayer(right); });
    for (const WorldId item : draw_order) {
        if (item.type == WorldType::Object) {
            const auto found = objects_.find(item.id);
            if (found != objects_.end() && found->second.visible_) drawObject(found->second);
        } else {
            const auto found = mazes_.find(item.id);
            if (found != mazes_.end()) drawMaze(found->second);
        }
    }
    for (const std::uint64_t id : frame_overlays) {
        const auto found = overlays_.find(id);
        if (found != overlays_.end() && found->second.active_ && found->second.visible_) drawOverlay(found->second);
    }
    EndDrawing();
}

inline void GameEngineImpl::drawObject(const GridObjectImpl& object) {
    if (object.type_ == GridObjectType::Image) {
        drawCell(object.image_, object.x_, object.y_, object.direction_, object.color_);
        return;
    }

    const float center_x = (static_cast<float>(object.x_) + 0.5f) * grid_size_;
    const float center_y = (static_cast<float>(object.y_) + 0.5f) * grid_size_;
    const Vector2 center = {center_x, center_y};
    if (object.type_ == GridObjectType::Square) {
        DrawRectangle(object.x_ * grid_size_ + (grid_size_ - object.size_) / 2,
                      object.y_ * grid_size_ + (grid_size_ - object.size_) / 2, object.size_, object.size_,
                      object.color_);
    } else if (object.type_ == GridObjectType::Circle) {
        DrawCircle(static_cast<int>(center_x), static_cast<int>(center_y), object.size_ / 2.0f, object.color_);
    } else if (object.type_ == GridObjectType::Triangle) {
        DrawPoly(center, 3, object.size_ / 2.0f, -90.0f, object.color_);
    } else if (object.type_ == GridObjectType::Pentagon) {
        DrawPoly(center, 5, object.size_ / 2.0f, -90.0f, object.color_);
    } else {
        constexpr float kPi = 3.14159265358979323846f;
        std::array<Vector2, 12> points = {};
        points[0] = center;
        for (std::size_t i = 0; i <= 10; ++i) {
            const std::size_t vertex = i % 10;
            const float radius = object.size_ / 2.0f * (vertex % 2 == 0 ? 1.0f : 0.45f);
            const float angle = (-90.0f + static_cast<float>(vertex) * 36.0f) * kPi / 180.0f;
            points[i + 1] = {center.x + std::cos(angle) * radius, center.y + std::sin(angle) * radius};
        }
        DrawTriangleFan(points.data(), static_cast<int>(points.size()), object.color_);
    }
}

inline void GameEngineImpl::drawMaze(const MazeImpl& maze) {
    for (int y = 0; y < maze.height_; ++y) {
        for (int x = 0; x < maze.width_; ++x) {
            if (!maze.walls_[y][x]) continue;
            if (maze.use_wall_images_) {
                const auto [shape, direction] = MazeImpl::shapeFor(maze.connectivityMask(x, y));
                drawCell(maze.wall_images_[shape], x, y, direction);
            } else {
                drawCell(maze.single_wall_, x, y);
            }
        }
    }
}

inline void GameEngineImpl::drawOverlay(const OverlayImpl& overlay) {
    if (overlay.type_ == OverlayType::Image) {
        drawOverlayImage(overlay.content_, overlay.x_, overlay.y_, overlay.color_);
    } else if (overlay.type_ == OverlayType::Text) {
        DrawText(overlay.content_.c_str(), overlay.x_, overlay.y_, overlay.font_size_, overlay.color_);
    } else {
        DrawRectangle(overlay.x_, overlay.y_, overlay.width_, overlay.height_, overlay.hover_ ? SKYBLUE : LIGHTGRAY);
        DrawRectangleLines(overlay.x_, overlay.y_, overlay.width_, overlay.height_, DARKGRAY);
        const int text_width = MeasureText(overlay.content_.c_str(), overlay.font_size_);
        DrawText(overlay.content_.c_str(), overlay.x_ + (overlay.width_ - text_width) / 2,
                 overlay.y_ + (overlay.height_ - overlay.font_size_) / 2, overlay.font_size_, overlay.color_);
    }
}

inline void GameEngineImpl::drawCell(const std::string& image, int grid_x, int grid_y, int direction, Color tint) {
    const int pixel_x = grid_x * grid_size_;
    const int pixel_y = grid_y * grid_size_;
    if (!image.empty() && assets_.Has(image)) {
        const Texture2D texture = assets_.Get(image);
        const Rectangle source = {0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)};
        const Rectangle destination = {static_cast<float>(pixel_x) + grid_size_ / 2.0f,
                                       static_cast<float>(pixel_y) + grid_size_ / 2.0f, static_cast<float>(grid_size_),
                                       static_cast<float>(grid_size_)};
        const Vector2 origin = {grid_size_ / 2.0f, grid_size_ / 2.0f};
        DrawTexturePro(texture, source, destination, origin, -90.0f * direction, tint);
    } else {
        DrawRectangle(pixel_x, pixel_y, grid_size_, grid_size_, RED);
    }
}

inline void GameEngineImpl::drawOverlayImage(const std::string& image, int pixel_x, int pixel_y, Color tint) {
    constexpr int kAssetSize = 32;
    if (!image.empty() && assets_.Has(image)) {
        const Texture2D texture = assets_.Get(image);
        const Rectangle source = {0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)};
        const Rectangle destination = {static_cast<float>(pixel_x), static_cast<float>(pixel_y),
                                       static_cast<float>(kAssetSize), static_cast<float>(kAssetSize)};
        DrawTexturePro(texture, source, destination, {0, 0}, 0, tint);
    } else {
        DrawRectangle(pixel_x, pixel_y, kAssetSize, kAssetSize, RED);
    }
}

inline void GameEngineImpl::drawGrid() {
    const Color color = Fade(LIGHTGRAY, 0.5f);
    for (int x = 0; x <= cols_; ++x) DrawLine(x * grid_size_, 0, x * grid_size_, rows_ * grid_size_, color);
    for (int y = 0; y <= rows_; ++y) DrawLine(0, y * grid_size_, cols_ * grid_size_, y * grid_size_, color);
}

// MazeImpl

inline MazeImpl::MazeImpl(int cols, int rows) : width_(cols), height_(rows) {
    if (cols < 1 || rows < 1 || cols > kMaxWidth || rows > kMaxHeight) {
        throw std::invalid_argument("Grid++ Error: maze dimensions must be between 1x1 and 64x64");
    }
}

inline int MazeImpl::connectivityMask(int x, int y) const {
    const auto wall = [&](int test_x, int test_y) {
        return test_x < 0 || test_y < 0 || test_x >= width_ || test_y >= height_ || walls_[test_y][test_x];
    };
    int mask = 0;
    if (wall(x, y - 1)) mask |= 1;
    if (wall(x + 1, y)) mask |= 2;
    if (wall(x, y + 1)) mask |= 4;
    if (wall(x - 1, y)) mask |= 8;
    return mask;
}

inline std::pair<int, int> MazeImpl::shapeFor(int mask) {
    static constexpr int kShapes[16] = {0, 1, 1, 3, 1, 2, 3, 4, 1, 3, 2, 4, 3, 4, 4, 5};
    static constexpr int kDirections[16] = {0, 0, 3, 0, 2, 0, 3, 0, 1, 1, 1, 1, 2, 2, 3, 0};
    return {kShapes[mask], kDirections[mask]};
}
/** @endcond */

// GridObject handler

/** @cond */
inline GridObject::GridObject(std::weak_ptr<GameEngineImpl> engine, std::uint64_t id)
    : engine_(std::move(engine)), id_(id) {}

inline std::shared_ptr<GameEngineImpl> GridObject::lockEngine() const {
    std::shared_ptr<GameEngineImpl> engine = engine_.lock();
    if (engine == nullptr) throw std::runtime_error("Grid++ Error: GridObject's GameEngine no longer exists");
    return engine;
}
/** @endcond */

inline bool GridObject::exists() const {
    const std::shared_ptr<GameEngineImpl> engine = engine_.lock();
    return engine != nullptr && engine->hasObject(id_);
}

inline void GridObject::remove() {
    const std::shared_ptr<GameEngineImpl> engine = lockEngine();
    engine->requireObject(id_);
    engine->removeObject(id_);
}

inline GridObject GridObject::deepCopy() const { return lockEngine()->cloneObject(id_); }
inline int GridObject::x() const { return lockEngine()->requireObject(id_).x_; }
inline int GridObject::y() const { return lockEngine()->requireObject(id_).y_; }

inline void GridObject::setPosition(int x, int y) {
    GridObjectImpl& object = lockEngine()->requireObject(id_);
    object.x_ = x;
    object.y_ = y;
}

inline void GridObject::move(int dx, int dy) {
    GridObjectImpl& object = lockEngine()->requireObject(id_);
    object.x_ += dx;
    object.y_ += dy;
}

inline std::string GridObject::image() const {
    const GridObjectImpl& object = lockEngine()->requireObject(id_);
    if (object.type_ != GridObjectType::Image)
        throw std::runtime_error("Grid++ Error: image() requires an image object");
    return object.image_;
}

inline void GridObject::setImage(const std::string& image) {
    GridObjectImpl& object = lockEngine()->requireObject(id_);
    if (object.type_ != GridObjectType::Image) {
        throw std::runtime_error("Grid++ Error: setImage() requires an image object");
    }
    object.image_ = image;
}

inline int GridObject::direction() const { return lockEngine()->requireObject(id_).direction_; }
inline void GridObject::setDirection(int direction) {
    lockEngine()->requireObject(id_).direction_ = ((direction % 4) + 4) % 4;
}
inline Color GridObject::color() const { return lockEngine()->requireObject(id_).color_; }
inline void GridObject::setColor(Color color) { lockEngine()->requireObject(id_).color_ = color; }
inline int GridObject::layer() const { return lockEngine()->requireObject(id_).layer_; }
inline void GridObject::setLayer(int layer) { lockEngine()->requireObject(id_).layer_ = layer; }
inline bool GridObject::visible() const { return lockEngine()->requireObject(id_).visible_; }
inline void GridObject::show() { lockEngine()->requireObject(id_).visible_ = true; }
inline void GridObject::hide() { lockEngine()->requireObject(id_).visible_ = false; }

inline void GridObject::set(const std::string& key, int value) { set(key, static_cast<long long>(value)); }
inline void GridObject::set(const std::string& key, long long value) {
    lockEngine()->requireObject(id_).values_.set(key, value);
}
inline void GridObject::set(const std::string& key, double value) {
    lockEngine()->requireObject(id_).values_.set(key, value);
}
inline void GridObject::set(const std::string& key, bool value) {
    lockEngine()->requireObject(id_).values_.set(key, value);
}
inline void GridObject::set(const std::string& key, const std::string& value) {
    lockEngine()->requireObject(id_).values_.set(key, value);
}
inline void GridObject::set(const std::string& key, const char* value) { set(key, std::string(value)); }

inline int GridObject::get(const std::string& key, int& value) const {
    long long stored = 0;
    const int status = get(key, stored);
    if (stored < std::numeric_limits<int>::min() || stored > std::numeric_limits<int>::max()) {
        throw std::runtime_error("Grid++ Error: Value '" + key + "' does not fit in int");
    }
    value = static_cast<int>(stored);
    return status;
}
inline int GridObject::get(const std::string& key, long long& value) const {
    return lockEngine()->requireObject(id_).values_.get(key, value);
}
inline int GridObject::get(const std::string& key, double& value) const {
    return lockEngine()->requireObject(id_).values_.get(key, value);
}
inline int GridObject::get(const std::string& key, bool& value) const {
    return lockEngine()->requireObject(id_).values_.get(key, value);
}
inline int GridObject::get(const std::string& key, std::string& value) const {
    return lockEngine()->requireObject(id_).values_.get(key, value);
}
template <typename T>
inline void GridObject::set(const std::string& key, const std::vector<T>& value) {
    lockEngine()->requireObject(id_).values_.set(key, value);
}
template <typename T>
inline int GridObject::get(const std::string& key, std::vector<T>& value) const {
    return lockEngine()->requireObject(id_).values_.get(key, value);
}
inline void GridObject::setInitFunction(GridObjectCallback function) {
    lockEngine()->requireObject(id_).init_ = function;
}
inline void GridObject::setUpdateFunction(GridObjectCallback function) {
    lockEngine()->requireObject(id_).update_ = function;
}
inline void GridObject::setCollideFunction(CollisionCallback function) {
    lockEngine()->requireObject(id_).collide_ = function;
}

// Overlay handler

/** @cond */
inline Overlay::Overlay(std::weak_ptr<GameEngineImpl> engine, std::uint64_t id) : engine_(std::move(engine)), id_(id) {}

inline std::shared_ptr<GameEngineImpl> Overlay::lockEngine() const {
    std::shared_ptr<GameEngineImpl> engine = engine_.lock();
    if (engine == nullptr) throw std::runtime_error("Grid++ Error: Overlay's GameEngine no longer exists");
    return engine;
}
/** @endcond */

inline bool Overlay::exists() const {
    const std::shared_ptr<GameEngineImpl> engine = engine_.lock();
    return engine != nullptr && engine->hasOverlay(id_);
}
inline void Overlay::remove() {
    const std::shared_ptr<GameEngineImpl> engine = lockEngine();
    engine->requireOverlay(id_);
    engine->removeOverlay(id_);
}
inline Overlay Overlay::deepCopy() const { return lockEngine()->cloneOverlay(id_); }
inline int Overlay::x() const { return lockEngine()->requireOverlay(id_).x_; }
inline int Overlay::y() const { return lockEngine()->requireOverlay(id_).y_; }
inline void Overlay::setPosition(int x, int y) {
    OverlayImpl& overlay = lockEngine()->requireOverlay(id_);
    overlay.x_ = x;
    overlay.y_ = y;
}
inline void Overlay::move(int dx, int dy) {
    OverlayImpl& overlay = lockEngine()->requireOverlay(id_);
    overlay.x_ += dx;
    overlay.y_ += dy;
}
inline bool Overlay::visible() const { return lockEngine()->requireOverlay(id_).visible_; }
inline void Overlay::show() { lockEngine()->requireOverlay(id_).visible_ = true; }
inline void Overlay::hide() { lockEngine()->requireOverlay(id_).visible_ = false; }

inline std::string Overlay::image() const {
    const OverlayImpl& overlay = lockEngine()->requireOverlay(id_);
    if (overlay.type_ != OverlayType::Image)
        throw std::runtime_error("Grid++ Error: image() requires an image overlay");
    return overlay.content_;
}
inline void Overlay::setImage(const std::string& image) {
    OverlayImpl& overlay = lockEngine()->requireOverlay(id_);
    if (overlay.type_ != OverlayType::Image) {
        throw std::runtime_error("Grid++ Error: setImage() requires an image overlay");
    }
    overlay.content_ = image;
}
inline std::string Overlay::text() const {
    const OverlayImpl& overlay = lockEngine()->requireOverlay(id_);
    if (overlay.type_ == OverlayType::Image) {
        throw std::runtime_error("Grid++ Error: text() requires a text overlay or button");
    }
    return overlay.content_;
}
inline void Overlay::setText(const std::string& text) {
    OverlayImpl& overlay = lockEngine()->requireOverlay(id_);
    if (overlay.type_ == OverlayType::Image) {
        throw std::runtime_error("Grid++ Error: setText() requires a text overlay or button");
    }
    overlay.content_ = text;
}
inline void Overlay::setColor(Color color) { lockEngine()->requireOverlay(id_).color_ = color; }

inline void Overlay::set(const std::string& key, int value) { set(key, static_cast<long long>(value)); }
inline void Overlay::set(const std::string& key, long long value) {
    lockEngine()->requireOverlay(id_).values_.set(key, value);
}
inline void Overlay::set(const std::string& key, double value) {
    lockEngine()->requireOverlay(id_).values_.set(key, value);
}
inline void Overlay::set(const std::string& key, bool value) {
    lockEngine()->requireOverlay(id_).values_.set(key, value);
}
inline void Overlay::set(const std::string& key, const std::string& value) {
    lockEngine()->requireOverlay(id_).values_.set(key, value);
}
inline void Overlay::set(const std::string& key, const char* value) { set(key, std::string(value)); }
inline int Overlay::get(const std::string& key, int& value) const {
    long long stored = 0;
    const int status = get(key, stored);
    if (stored < std::numeric_limits<int>::min() || stored > std::numeric_limits<int>::max()) {
        throw std::runtime_error("Grid++ Error: Value '" + key + "' does not fit in int");
    }
    value = static_cast<int>(stored);
    return status;
}
inline int Overlay::get(const std::string& key, long long& value) const {
    return lockEngine()->requireOverlay(id_).values_.get(key, value);
}
inline int Overlay::get(const std::string& key, double& value) const {
    return lockEngine()->requireOverlay(id_).values_.get(key, value);
}
inline int Overlay::get(const std::string& key, bool& value) const {
    return lockEngine()->requireOverlay(id_).values_.get(key, value);
}
inline int Overlay::get(const std::string& key, std::string& value) const {
    return lockEngine()->requireOverlay(id_).values_.get(key, value);
}
template <typename T>
inline void Overlay::set(const std::string& key, const std::vector<T>& value) {
    lockEngine()->requireOverlay(id_).values_.set(key, value);
}
template <typename T>
inline int Overlay::get(const std::string& key, std::vector<T>& value) const {
    return lockEngine()->requireOverlay(id_).values_.get(key, value);
}
inline void Overlay::setInitFunction(OverlayCallback function) { lockEngine()->requireOverlay(id_).init_ = function; }
inline void Overlay::setUpdateFunction(OverlayCallback function) {
    lockEngine()->requireOverlay(id_).update_ = function;
}
inline void Overlay::setClickFunction(OverlayCallback function) {
    OverlayImpl& overlay = lockEngine()->requireOverlay(id_);
    if (overlay.type_ != OverlayType::Button) {
        throw std::runtime_error("Grid++ Error: setClickFunction() requires a button");
    }
    overlay.click_ = function;
}

// Maze handler

/** @cond */
inline Maze::Maze(std::weak_ptr<GameEngineImpl> engine, std::uint64_t id) : engine_(std::move(engine)), id_(id) {}
inline std::shared_ptr<GameEngineImpl> Maze::lockEngine() const {
    std::shared_ptr<GameEngineImpl> engine = engine_.lock();
    if (engine == nullptr) throw std::runtime_error("Grid++ Error: Maze's GameEngine no longer exists");
    return engine;
}
/** @endcond */
inline bool Maze::exists() const {
    const std::shared_ptr<GameEngineImpl> engine = engine_.lock();
    return engine != nullptr && engine->hasMaze(id_);
}
inline void Maze::remove() {
    const std::shared_ptr<GameEngineImpl> engine = lockEngine();
    engine->requireMaze(id_);
    engine->removeMaze(id_);
}
inline Maze Maze::deepCopy() const { return lockEngine()->cloneMaze(id_); }
inline void Maze::setWall(int x, int y, bool wall) {
    MazeImpl& maze = lockEngine()->requireMaze(id_);
    if (x < 0 || y < 0 || x >= maze.width_ || y >= maze.height_) {
        throw std::out_of_range("Grid++ Error: wall position is outside the maze");
    }
    maze.walls_[y][x] = wall;
}
inline bool Maze::isWall(int x, int y) const {
    const MazeImpl& maze = lockEngine()->requireMaze(id_);
    return x < 0 || y < 0 || x >= maze.width_ || y >= maze.height_ || maze.walls_[y][x];
}
inline void Maze::setWallImage(const std::string& image) {
    MazeImpl& maze = lockEngine()->requireMaze(id_);
    maze.single_wall_ = image;
    maze.use_wall_images_ = false;
}
inline void Maze::setWallImages(const std::string& isolated, const std::string& end, const std::string& straight,
                                const std::string& corner, const std::string& tee, const std::string& cross) {
    MazeImpl& maze = lockEngine()->requireMaze(id_);
    maze.wall_images_[0] = isolated;
    maze.wall_images_[1] = end;
    maze.wall_images_[2] = straight;
    maze.wall_images_[3] = corner;
    maze.wall_images_[4] = tee;
    maze.wall_images_[5] = cross;
    maze.use_wall_images_ = true;
}
inline int Maze::width() const { return lockEngine()->requireMaze(id_).width_; }
inline int Maze::height() const { return lockEngine()->requireMaze(id_).height_; }
inline void Maze::setInitFunction(MazeCallback function) { lockEngine()->requireMaze(id_).init_ = function; }
inline void Maze::setUpdateFunction(MazeCallback function) { lockEngine()->requireMaze(id_).update_ = function; }

// GameEngine handler

inline GameEngine::GameEngine(int cols, int rows, int grid_size)
    : impl_(std::shared_ptr<GameEngineImpl>(new GameEngineImpl(cols, rows, grid_size))) {}
/** @cond */
inline GameEngine::GameEngine(std::shared_ptr<GameEngineImpl> impl) : impl_(std::move(impl)) {}
/** @endcond */
inline void GameEngine::loadAssets(const std::string& database_path) { impl_->assets_.Load(database_path); }
inline void GameEngine::setBackgroundColor(Color color) { impl_->background_color_ = color; }
inline void GameEngine::showGrid(bool show) { impl_->show_grid_ = show; }
inline int GameEngine::cols() const { return impl_->cols_; }
inline int GameEngine::rows() const { return impl_->rows_; }
inline int GameEngine::gridSize() const { return impl_->grid_size_; }
inline GridObject GameEngine::addObject(const std::string& image, GridObjectCallback init, GridObjectCallback update,
                                        CollisionCallback collide) {
    return impl_->addObject(image, init, update, collide);
}
inline GridObject GameEngine::addSquare(int x, int y, int size, Color color, GridObjectCallback init,
                                        GridObjectCallback update, CollisionCallback collide) {
    return impl_->addShape(GridObjectType::Square, x, y, size, color, init, update, collide);
}
inline GridObject GameEngine::addCircle(int x, int y, int size, Color color, GridObjectCallback init,
                                        GridObjectCallback update, CollisionCallback collide) {
    return impl_->addShape(GridObjectType::Circle, x, y, size, color, init, update, collide);
}
inline GridObject GameEngine::addTriangle(int x, int y, int size, Color color, GridObjectCallback init,
                                          GridObjectCallback update, CollisionCallback collide) {
    return impl_->addShape(GridObjectType::Triangle, x, y, size, color, init, update, collide);
}
inline GridObject GameEngine::addPentagon(int x, int y, int size, Color color, GridObjectCallback init,
                                          GridObjectCallback update, CollisionCallback collide) {
    return impl_->addShape(GridObjectType::Pentagon, x, y, size, color, init, update, collide);
}
inline GridObject GameEngine::addStar(int x, int y, int size, Color color, GridObjectCallback init,
                                      GridObjectCallback update, CollisionCallback collide) {
    return impl_->addShape(GridObjectType::Star, x, y, size, color, init, update, collide);
}
inline Maze GameEngine::addMaze(int cols, int rows, MazeCallback init, MazeCallback update) {
    return impl_->addMaze(cols, rows, init, update);
}
inline void GameEngine::clearObjects() { impl_->clearObjects(); }
inline Overlay GameEngine::addOverlay(const std::string& image, OverlayCallback init, OverlayCallback update) {
    return impl_->addImageOverlay(image, init, update);
}
inline Overlay GameEngine::addTextOverlay(const std::string& text, int x, int y, int font_size, Color color,
                                          OverlayCallback init, OverlayCallback update) {
    return impl_->addTextOverlay(text, x, y, font_size, color, init, update);
}
inline Overlay GameEngine::addButton(const std::string& text, int x, int y, int width, int height,
                                     OverlayCallback click) {
    return impl_->addButton(text, x, y, width, height, click);
}
inline void GameEngine::clearOverlays() { impl_->clearOverlays(); }
inline bool GameEngine::keyPressed(int key) const { return IsKeyPressed(key); }
inline bool GameEngine::keyDown(int key) const { return IsKeyDown(key); }
inline bool GameEngine::mousePressed(int button) const { return IsMouseButtonPressed(button); }
inline int GameEngine::mouseX() const { return static_cast<int>(GetMousePosition().x); }
inline int GameEngine::mouseY() const { return static_cast<int>(GetMousePosition().y); }
inline double GameEngine::time() const { return GetTime(); }
inline int GameEngine::random(int min, int max) const { return GetRandomValue(min, max); }
inline void GameEngine::run() { impl_->run(); }

}  // namespace gridpp

#endif  // GRID_PLUS_PLUS_GAME_ENGINE_H_
