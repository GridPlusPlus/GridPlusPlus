# 生命週期方法

自訂 GridObject 透過虛擬函式回應 Engine 的事件。每個方法對應物件生命週期中的一個階段；衍生類別只覆寫需要的行為，其餘部分沿用 GridObject 的預設實作。

| 方法 | 執行時機 | 常見用途 |
|---|---|---|
| `OnSpawn()` | 成功加入 Engine 後一次 | 讀取 Engine 尺寸、建立依賴 Engine 的初始狀態 |
| `OnUpdate()` | 每幀更新階段 | 輸入、計時、移動與規則更新 |
| `OnCollide(other)` | 與另一個有效物件同格 | 收集、傷害與勝負判斷 |
| `Render(engine)` | 每幀繪製階段 | 素材繪製或自訂 raylib 圖形 |

## 加入 Engine

建構子執行時，物件還沒有交給任何 Engine，`engine()` 會回傳 `nullptr`。建構子適合設定素材、座標、tag 與不依賴 Engine 的成員變數。需要讀取網格尺寸或生成其他物件時，使用 `OnSpawn()`。

```cpp title="類別節錄：OnSpawn()"
class Enemy : public GridObject {
public:
    Enemy() : GridObject("enemy", 0, 0) {}

    void OnSpawn() override {
        set_x(engine()->cols() - 1);
        set_y(engine()->rows() / 2);
    }
};
```

建構子先讓物件本身處於有效狀態，`OnSpawn()` 才處理必須等物件加入世界後才能完成的工作。遊戲執行期間生成物件時，加入、初始化與第一次更新分屬哪一個時點，統一由第 8 章的[每幀執行順序](../08-lifecycle/03-frame-order.md)說明。

## 更新狀態

`OnUpdate()` 每幀執行一次。方法可讀寫成員變數，也能透過 `engine()` 查詢遊戲世界。下列地鼠各自保存 `next_move_`，到達時間後隨機移動。

```cpp title="Mole 類別節錄：OnUpdate()"
void OnUpdate() override {
    if (GetTime() < next_move_) return;

    set_x(GetRandomValue(0, engine()->cols() - 1));
    set_y(GetRandomValue(0, engine()->rows() - 1));
    next_move_ = GetTime() + interval_;
}
```

隱藏的 GridObject 仍會收到 `OnUpdate()`，因此物件可以在看不見時繼續倒數，時間到後再呼叫 `set_visible(true)`。永久移除與執行期間刪除的規則留到第 8 章處理。

## 回應碰撞

`OnCollide(GridObject* other)` 的參數是另一個物件的借用指標，判斷對象的方式與第 4 章的 `CollideFn` 相同；本節只關心衍生類別如何回應碰撞，雙向通知與生成順序則統一留在第 8 章。

```cpp title="玩家類別節錄：OnCollide()"
void OnCollide(GridObject* other) override {
    if (other->tag() == "ghost") {
        defeated_ = true;
        set_visible(false);
    }
}
```

若碰撞行為只需改變自己的成員變數，衍生類別可以直接完成，不必建立額外全域資料。跨物件或全域的規則仍可由遊戲狀態管理，例如 Pacman 用剩餘豆子數決定勝利。

## 自訂繪製

GridObject 的預設 `Render()` 會以 `asset_name()`、`direction()` 和 `tint()` 繪製所在格。覆寫 `Render()` 後可直接使用 raylib，或先呼叫基底版本再加上額外效果。

```cpp title="物件類別節錄：Render()"
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

`Render()` 應負責畫面，不應在此修改會影響碰撞或遊戲結果的狀態。Engine 會依 z-index 排序繪製，但更新與碰撞仍保留原始生成順序。

## 本節小結

- 建構子建立物件資料，`OnSpawn()` 處理依賴 Engine 的初始化。
- `OnUpdate()` 修改每個實例的狀態。
- `OnCollide()` 回應同格物件，辨認方式與 `CollideFn` 相同。
- `Render()` 控制物件畫面，預設實作會繪製素材。
