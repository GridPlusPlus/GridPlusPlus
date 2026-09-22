/** @file GridMaze.h
 *  @brief 定義迷宮 handler 與實體資料。
 */
#ifndef GRID_PLUS_PLUS_GRID_MAZE_H_
#define GRID_PLUS_PLUS_GRID_MAZE_H_

#include <cstdint>
#include <memory>
#include <string>
#include <utility>

namespace gridpp {

class GameEngine;
class Maze;

/** @cond */
class GameEngineImpl;

using MazeCallback = void (*)(GameEngine engine, Maze self);

class MazeImpl {
public:
    static constexpr int kMaxWidth = 64;
    static constexpr int kMaxHeight = 64;

    MazeImpl(const MazeImpl&) = default;
    MazeImpl& operator=(const MazeImpl&) = default;

private:
    friend class GameEngineImpl;
    friend class Maze;

    MazeImpl(int cols, int rows);
    int connectivityMask(int x, int y) const;
    static std::pair<int, int> shapeFor(int mask);

    int width_ = 0;
    int height_ = 0;
    bool walls_[kMaxHeight][kMaxWidth] = {};
    std::string single_wall_;
    std::string wall_images_[6];
    bool use_wall_images_ = false;
    bool initialized_ = false;
    bool active_ = false;
    MazeCallback init_ = nullptr;
    MazeCallback update_ = nullptr;
};
/** @endcond */

/** 可複製的迷宮 handler。 */
class Maze {
public:
    Maze() = default;

    bool exists() const;
    void remove();
    Maze deepCopy() const;

    void setWall(int x, int y, bool wall = true);
    bool isWall(int x, int y) const;
    void setWallImage(const std::string& image);
    void setWallImages(const std::string& isolated, const std::string& end, const std::string& straight,
                       const std::string& corner, const std::string& tee, const std::string& cross);

    int width() const;
    int height() const;

    void setInitFunction(MazeCallback function);
    void setUpdateFunction(MazeCallback function);

    /** @cond */
private:
    friend class GameEngineImpl;

    Maze(std::weak_ptr<GameEngineImpl> engine, std::uint64_t id);
    std::shared_ptr<GameEngineImpl> lockEngine() const;

    std::weak_ptr<GameEngineImpl> engine_;
    std::uint64_t id_ = 0;
    /** @endcond */
};

}  // namespace gridpp

#endif  // GRID_PLUS_PLUS_GRID_MAZE_H_
