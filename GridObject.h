/** @file GridObject.h
 *  @brief 定義網格物件 handler 與實體資料。
 */
#ifndef GRID_PLUS_PLUS_GRID_OBJECT_H_
#define GRID_PLUS_PLUS_GRID_OBJECT_H_

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "GridValue.h"
#include "raylib.h"

namespace gridpp {

class GameEngine;
class GridObject;

/** @cond */
class GameEngineImpl;

using GridObjectCallback = void (*)(GameEngine engine, GridObject self);
using CollisionCallback = void (*)(GameEngine engine, GridObject self, GridObject other);

enum class GridObjectType {
    Image,
    Square,
    Circle,
    Triangle,
    Pentagon,
    Star,
};

class GridObjectImpl {
public:
    GridObjectImpl(const GridObjectImpl&) = default;
    GridObjectImpl& operator=(const GridObjectImpl&) = default;

private:
    friend class GameEngineImpl;
    friend class GridObject;

    explicit GridObjectImpl(std::string image);
    GridObjectImpl(GridObjectType type, int x, int y, int size, Color color);

    GridObjectType type_ = GridObjectType::Image;
    std::string image_;
    int x_ = 0;
    int y_ = 0;
    int size_ = 0;
    int direction_ = 0;
    Color color_ = WHITE;
    int layer_ = 0;
    bool visible_ = true;
    bool initialized_ = false;
    bool active_ = false;
    GridObjectCallback init_ = nullptr;
    GridObjectCallback update_ = nullptr;
    CollisionCallback collide_ = nullptr;
    GridValueStore values_;
};
/** @endcond */

/**
 * 操作遊戲世界中單一網格物件的 handler。
 *
 * 複製 handler 不會複製物件；所有複本仍指向同一個遊戲實體。物件被移除或 GameEngine
 * 銷毀後，可以用 exists() 檢查 handler 是否仍有效。除 exists() 之外，對失效 handler
 * 呼叫其他函式會拋出 std::runtime_error。
 */
class GridObject {
public:
    /** 建立不指向任何物件的 handler。 */
    GridObject() = default;

    /** @return handler 是否仍指向存在的物件。 */
    bool exists() const;

    /** 從引擎移除物件；呼叫後所有指向它的 handler 都會失效。 */
    void remove();

    /** @return 一個擁有獨立狀態、callback 與新 handler 的物件複本。 */
    GridObject deepCopy() const;

    /** @return 物件的網格 x 座標。 */
    int x() const;

    /** @return 物件的網格 y 座標。 */
    int y() const;

    /** 將物件移到指定網格座標。 */
    void setPosition(int x, int y);

    /** 將物件相對移動 dx 欄、dy 列。 */
    void move(int dx, int dy);

    /**
     * @return 圖片物件的素材名稱。
     * @throws std::runtime_error 若此 handler 指向幾何形狀。
     */
    std::string image() const;

    /**
     * 設定圖片物件使用的素材名稱。
     * @throws std::runtime_error 若此 handler 指向幾何形狀。
     */
    void setImage(const std::string& image);

    /** @return 繪製方向，0 到 3 分別表示逆時針旋轉 0°、90°、180° 與 270°。 */
    int direction() const;

    /** 設定繪製方向；輸入會正規化至 0 到 3。 */
    void setDirection(int direction);

    /** @return 圖片 tint 或幾何形狀的顏色。 */
    Color color() const;

    /** 設定圖片 tint 或幾何形狀的顏色。 */
    void setColor(Color color);

    /** @return 物件的繪製層級。 */
    int layer() const;

    /** 設定繪製層級；數值越大越晚繪製，相同層級保持建立順序。 */
    void setLayer(int layer);

    /** @return 物件是否可見。 */
    bool visible() const;

    /** 顯示物件。 */
    void show();

    /** 隱藏物件；隱藏期間仍會更新，但不會繪製或參與碰撞。 */
    void hide();

    /**
     * @name 自訂狀態
     * set() 會拒絕以不同類型覆寫已存在的 key；get() 使用的類型也必須與儲存類型相同。
     */
    ///@{
    /** 以 key 儲存整數；int 與 long long 都以 long long 保存。 */
    void set(const std::string& key, int value);
    /** 以 key 儲存長整數。 */
    void set(const std::string& key, long long value);
    /** 以 key 儲存浮點數。 */
    void set(const std::string& key, double value);
    /** 以 key 儲存布林值。 */
    void set(const std::string& key, bool value);
    /** 以 key 儲存字串。 */
    void set(const std::string& key, const std::string& value);
    /** 以 key 儲存 C 字串；內部會複製為 std::string。 */
    void set(const std::string& key, const char* value);

    /**
     * @return 找到 key 時為 0；找不到時將 value 清為預設值並回傳 1。
     * @throws std::runtime_error 若類型不同，或儲存的整數無法容納於 int。
     */
    int get(const std::string& key, int& value) const;
    /**
     * @return 找到 key 時為 0；找不到時將 value 清為預設值並回傳 1。
     * @throws std::runtime_error 若取值類型與儲存類型不同。
     */
    int get(const std::string& key, long long& value) const;
    /**
     * @return 找到 key 時為 0；找不到時將 value 清為預設值並回傳 1。
     * @throws std::runtime_error 若取值類型與儲存類型不同。
     */
    int get(const std::string& key, double& value) const;
    /**
     * @return 找到 key 時為 0；找不到時將 value 清為預設值並回傳 1。
     * @throws std::runtime_error 若取值類型與儲存類型不同。
     */
    int get(const std::string& key, bool& value) const;
    /**
     * @return 找到 key 時為 0；找不到時將 value 清為預設值並回傳 1。
     * @throws std::runtime_error 若取值類型與儲存類型不同。
     */
    int get(const std::string& key, std::string& value) const;

    /** 以 key 儲存指定類型的 vector。 */
    template <typename T>
    void set(const std::string& key, const std::vector<T>& value);

    /**
     * @return 找到 key 時為 0；找不到時將 value 清為空 vector 並回傳 1。
     * @throws std::runtime_error 若取值類型與儲存類型不同。
     */
    template <typename T>
    int get(const std::string& key, std::vector<T>& value) const;
    ///@}

    /** 取代初始化 callback；若物件已初始化，新函式不會自動補呼叫。 */
    void setInitFunction(GridObjectCallback function);

    /** 取代每幀更新 callback。 */
    void setUpdateFunction(GridObjectCallback function);

    /** 取代碰撞 callback。 */
    void setCollideFunction(CollisionCallback function);

    /** @cond */
private:
    friend class GameEngineImpl;

    GridObject(std::weak_ptr<GameEngineImpl> engine, std::uint64_t id);
    std::shared_ptr<GameEngineImpl> lockEngine() const;

    std::weak_ptr<GameEngineImpl> engine_;
    std::uint64_t id_ = 0;
    /** @endcond */
};

/** @cond */
inline GridObjectImpl::GridObjectImpl(std::string image) : image_(std::move(image)) {}

inline GridObjectImpl::GridObjectImpl(GridObjectType type, int x, int y, int size, Color color)
    : type_(type), x_(x), y_(y), size_(size), color_(color) {}
/** @endcond */

}  // namespace gridpp

#endif  // GRID_PLUS_PLUS_GRID_OBJECT_H_
