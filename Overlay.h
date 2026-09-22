/** @file Overlay.h
 *  @brief 定義畫面覆蓋元素 handler 與實體資料。
 */
#ifndef GRID_PLUS_PLUS_OVERLAY_H_
#define GRID_PLUS_PLUS_OVERLAY_H_

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "GridValue.h"
#include "raylib.h"

namespace gridpp {

class GameEngine;
class Overlay;

/** @cond */
class GameEngineImpl;

using OverlayCallback = void (*)(GameEngine engine, Overlay self);

enum class OverlayType {
    Image,
    Text,
    Button,
};

class OverlayImpl {
public:
    OverlayImpl(const OverlayImpl&) = default;
    OverlayImpl& operator=(const OverlayImpl&) = default;

private:
    friend class GameEngineImpl;
    friend class Overlay;

    explicit OverlayImpl(OverlayType type);

    OverlayType type_;
    std::string content_;
    int x_ = 0;
    int y_ = 0;
    int width_ = 0;
    int height_ = 0;
    int font_size_ = 20;
    Color color_ = BLACK;
    bool visible_ = true;
    bool hover_ = false;
    bool initialized_ = false;
    bool active_ = false;
    OverlayCallback init_ = nullptr;
    OverlayCallback update_ = nullptr;
    OverlayCallback click_ = nullptr;
    GridValueStore values_;
};
/** @endcond */

/**
 * 操作繪製在遊戲世界上方的畫面覆蓋元素。
 *
 * Overlay 使用視窗像素座標。複製 handler 不會複製元素；所有複本仍操作同一個實體。
 * 除 exists() 之外，對已移除或引擎已銷毀的 handler 呼叫其他函式會拋出 std::runtime_error。
 */
class Overlay {
public:
    /** 建立不指向任何元素的 handler。 */
    Overlay() = default;

    /** @return handler 是否仍指向存在的元素。 */
    bool exists() const;

    /** 從引擎移除元素；呼叫後所有指向它的 handler 都會失效。 */
    void remove();

    /** @return 一個擁有獨立狀態、callback 與新 handler 的元素複本。 */
    Overlay deepCopy() const;

    /** @return 元素的視窗像素 x 座標。 */
    int x() const;
    /** @return 元素的視窗像素 y 座標。 */
    int y() const;
    /** 將元素移到指定像素座標。 */
    void setPosition(int x, int y);
    /** 將元素相對移動 dx、dy 像素。 */
    void move(int dx, int dy);

    /** @return 元素是否可見。 */
    bool visible() const;
    /** 顯示元素。 */
    void show();
    /** 隱藏元素；隱藏期間仍會執行更新 callback，但按鈕不會接受點擊。 */
    void hide();

    /**
     * @return 圖片覆蓋元件的素材名稱。
     * @throws std::runtime_error 若此 handler 指向文字或按鈕。
     */
    std::string image() const;
    /**
     * 設定圖片覆蓋元件的素材名稱。
     * @throws std::runtime_error 若此 handler 指向文字或按鈕。
     */
    void setImage(const std::string& image);
    /**
     * @return 文字覆蓋元件或按鈕的文字。
     * @throws std::runtime_error 若此 handler 指向圖片。
     */
    std::string text() const;
    /**
     * 設定文字覆蓋元件或按鈕的文字。
     * @throws std::runtime_error 若此 handler 指向圖片。
     */
    void setText(const std::string& text);
    /** 設定圖片 tint、文字顏色或按鈕文字顏色。 */
    void setColor(Color color);

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

    /** 取代初始化 callback；若元件已初始化，新函式不會自動補呼叫。 */
    void setInitFunction(OverlayCallback function);
    /** 取代每幀更新 callback。 */
    void setUpdateFunction(OverlayCallback function);
    /**
     * 取代按鈕點擊 callback。
     * @throws std::runtime_error 若此 handler 不是按鈕。
     */
    void setClickFunction(OverlayCallback function);

    /** @cond */
private:
    friend class GameEngineImpl;

    Overlay(std::weak_ptr<GameEngineImpl> engine, std::uint64_t id);
    std::shared_ptr<GameEngineImpl> lockEngine() const;

    std::weak_ptr<GameEngineImpl> engine_;
    std::uint64_t id_ = 0;
    /** @endcond */
};

/** @cond */
inline OverlayImpl::OverlayImpl(OverlayType type) : type_(type) {}
/** @endcond */

}  // namespace gridpp

#endif  // GRID_PLUS_PLUS_OVERLAY_H_
