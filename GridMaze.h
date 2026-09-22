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

/**
 * 操作固定尺寸的迷宮牆面網格。
 *
 * 複製 handler 不會複製迷宮；所有複本仍操作同一個實體。迷宮被移除或 GameEngine 銷毀後，
 * 可以用 exists() 檢查 handler 是否仍有效。除 exists() 之外，對失效 handler 呼叫其他函式
 * 會拋出 std::runtime_error。
 */
class Maze {
public:
    /** 建立不指向任何迷宮的 handler。 */
    Maze() = default;

    /** @return handler 是否仍指向存在的迷宮。 */
    bool exists() const;
    /** 從引擎移除迷宮；呼叫後所有指向它的 handler 都會失效。 */
    void remove();
    /** @return 一個擁有獨立牆面、素材設定、callback 與新 handler 的迷宮複本。 */
    Maze deepCopy() const;

    /**
     * 設定一格是否為牆。
     * @param x 迷宮內的網格 x 座標。
     * @param y 迷宮內的網格 y 座標。
     * @param wall true 表示牆，false 表示通道。
     * @throws std::out_of_range 若座標位於迷宮外。
     */
    void setWall(int x, int y, bool wall = true);

    /** @return 指定位置是否為牆；迷宮外一律回傳 true。 */
    bool isWall(int x, int y) const;

    /** 設定所有牆共用的素材，並切換至單一素材模式。 */
    void setWallImage(const std::string& image);

    /**
     * 設定自動拼接牆面使用的六種素材，並切換至自動拼接模式。
     * @param isolated 孤立牆素材。
     * @param end 端點素材。
     * @param straight 直線素材。
     * @param corner 轉角素材。
     * @param tee T 形素材。
     * @param cross 十字素材。
     */
    void setWallImages(const std::string& isolated, const std::string& end, const std::string& straight,
                       const std::string& corner, const std::string& tee, const std::string& cross);

    /** @return 迷宮欄數。 */
    int width() const;
    /** @return 迷宮列數。 */
    int height() const;

    /** 取代初始化 callback；若迷宮已初始化，新函式不會自動補呼叫。 */
    void setInitFunction(MazeCallback function);
    /** 取代每幀更新 callback。 */
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
