# 生命週期方法

自訂 GridObject 透過虛擬函式回應 Engine 的事件。每個方法對應物件生命週期中的一個階段；衍生 class 只覆寫需要的行為，其餘部分沿用 GridObject 的預設實作。

| 方法 | 執行時機 | 常見用途 |
|---|---|---|
| `OnSpawn()` | 成功加入 Engine 後一次 | 讀取 Engine 尺寸、建立依賴 Engine 的初始狀態 |
| `OnUpdate()` | 每幀更新階段 | 輸入、計時、移動與規則更新 |
| `OnCollide(other)` | 與另一個有效物件同格 | 收集、傷害與勝負判斷 |
| `Render(engine)` | 每幀繪製階段 | 素材繪製或自訂 raylib 圖形 |

## 加入 Engine

建構子執行時，物件還沒有交給任何 Engine，`engine()` 會回傳 `nullptr`。建構子適合設定素材、座標、tag 與不依賴 Engine 的成員變數。需要讀取網格尺寸或生成其他物件時，使用 `OnSpawn()`。

```cpp
class Enemy : public GridObject {
public:
    Enemy() : GridObject("enemy", 0, 0) {}

    void OnSpawn() override {
        set_x(engine()->cols() - 1);
        set_y(engine()->rows() / 2);
    }
};
```

在主迴圈開始前呼叫 `Spawn()`，`OnSpawn()` 會在 `Spawn()` 回傳前完成。遊戲執行期間產生的物件會排到幀末，屆時執行 `OnSpawn()`，並從下一幀開始更新。`OnSpawn()` 丟出例外時，Engine 會移除並釋放該物件，再把例外交給呼叫端。

## 更新狀態

`OnUpdate()` 每幀執行一次。方法可讀寫成員變數，也能透過 `engine()` 查詢遊戲世界。下列地鼠各自保存 `next_move_`，到達時間後隨機移動。

```cpp
void OnUpdate() override {
    if (GetTime() < next_move_) return;

    set_x(GetRandomValue(0, engine()->cols() - 1));
    set_y(GetRandomValue(0, engine()->rows() - 1));
    next_move_ = GetTime() + interval_;
}
```

隱藏的 GridObject 仍會收到 `OnUpdate()`。這使物件可以在看不見時繼續倒數，時間到後自行呼叫 `set_visible(true)`。已由 `Destroy()` 排定刪除的物件不再更新。

## 回應碰撞

`OnCollide(GridObject* other)` 的參數是另一個物件的借用指標。規則和函式式 collision callback 相同，包括有效座標、雙向通知與 spawn 順序。

```cpp
void OnCollide(GridObject* other) override {
    if (other->tag() == "ghost") {
        defeated_ = true;
        set_visible(false);
    }
}
```

若碰撞行為只需改變自己的成員變數，衍生 class 可以直接完成，不必建立額外全域資料。跨物件或全局的規則仍可由遊戲狀態管理，例如 Pacman 用剩餘豆子數決定勝利。

## 自訂繪製

GridObject 的預設 `Render()` 會以 `asset_name()`、`direction()` 和 `tint()` 繪製所在格。覆寫 `Render()` 後可直接使用 raylib，或先呼叫基底版本再加上額外效果。

```cpp
void Render(GridEngine* engine) override {
    GridObject::Render(engine);
    DrawCircle(
        x() * engine->grid_size() + engine->grid_size() / 2,
        y() * engine->grid_size() + engine->grid_size() / 2,
        4,
        RED
    );
}
```

`Render()` 應負責畫面，不應在此修改會影響碰撞或遊戲結果的狀態。Engine 會依 z-index 排序繪製，但更新與碰撞仍保留原始 spawn 順序。

## Summary

- 建構子建立物件資料，`OnSpawn()` 處理依賴 Engine 的初始化。
- `OnUpdate()` 修改每個 instance 的狀態。
- `OnCollide()` 回應同格物件，規則與 collision callback 相同。
- `Render()` 控制物件畫面，預設實作會繪製素材。
