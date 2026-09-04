# 自訂 GridObject

章首頁已經確認問題來自資料歸屬，現在直接改寫地鼠。C++ 類別可以把資料與使用它的行為放在一起；新類別繼承 `GridObject` 後，仍保有座標、素材與碰撞等共通能力，每次建立實例時又會取得自己的一組成員變數。

## 讓多隻地鼠保存自己的時間

`Mole` 將移動間隔與下一次移動時間保存為 `interval_` 和 `next_move_`，並以 `OnUpdate()` 取代原本交給 `Spawn()` 的 `UpdateMole`。Engine 每幀仍然更新物件，只是現在呼叫的是這個實例自己的方法，方法也能直接讀寫同一個實例的成員變數。

```cpp title="main.cpp 節錄：Mole 類別"
class Mole : public GridObject {
public:
    Mole(int x, int y, double interval)
        : GridObject("mole", x, y),
          interval_(interval),
          next_move_(0.0) {
        set_tag("mole");
    }

    void OnUpdate() override {
        if (GetTime() < next_move_) return;

        set_x(GetRandomValue(0, engine()->cols() - 1));
        set_y(GetRandomValue(0, engine()->rows() - 1));
        next_move_ = GetTime() + interval_;
    }

private:
    double interval_;
    double next_move_;
};
```

`interval_` 和 `next_move_` 都屬於單一 `Mole`，建構子參數還能讓每隻地鼠使用不同速度，不必為相同行為建立多個只因數值不同而換名字的函式。接著在 `main()` 中建立三個實例，並逐一把所有權交給 Engine：

```cpp title="main() 節錄：生成三隻速度不同的地鼠"
game.Spawn(new Mole(1, 1, 1.0));
game.Spawn(new Mole(3, 3, 0.8));
game.Spawn(new Mole(6, 5, 1.4));
```

這項修改沒有改變 Engine 看待物件的方式，因為 `class Mole : public GridObject` 這段宣告表示 `Mole` 仍然是一種 `GridObject`。真正改變的是資料的歸屬：每次 `new Mole(...)` 都會建立一份獨立的 `interval_` 和 `next_move_`，所以其中一隻地鼠更新計時器時，不會再干擾另外兩隻。

## 建立與借用物件

自訂物件仍透過 `Spawn()` 加入 Engine。傳入的物件由 `new` 建立，`Spawn()` 成功後所有權便轉交給 Engine；呼叫端保留的指標只能在物件仍存在時借用，不可自行 `delete`。下一章開始前先記住這條安全界線，第 8 章再完整說明所有權與失效時機。

```cpp title="main() 節錄：轉交所有權並保留借用指標"
Mole* fast_mole = new Mole(3, 4, 0.5);
game.Spawn(fast_mole);

// fast_mole 現在是借用指標，所有權已轉交給 game。
```

如果不需要呼叫 `Mole` 特有的方法，也可以直接寫成 `GridObject* mole = game.Spawn(new Mole(...));`，只透過 `GridObject` 的共通介面操作它。

## 回頭理解回呼函式與繼承

第 4 章把函式先交給 Engine，等更新或碰撞發生時再由 Engine 呼叫；走到這裡，才需要替這種安排補上名稱：它通常稱為回呼函式（callback）。Grid++ 內部使用 `CallbackGridObject` 保存 `UpdateFn` 與 `CollideFn`，但使用函式式 `Spawn()` 時不必直接操作這個類別。

回呼函式與自訂類別共用同一套 Engine、碰撞及繪製流程，選擇標準不是遊戲規模，而是物件是否需要額外的個別資料。只使用座標、素材、tag 等既有狀態時，`UpdateFn`／`CollideFn` 較直接；需要各自保存計時器、方向或生命值時，自訂 `GridObject` 會讓資料歸屬更清楚。

| 情況 | 建議方式 |
|---|---|
| 物件只需要位置、素材、tag 等內建狀態 | `UpdateFn`／`CollideFn` 回呼函式 |
| 所有物件共用一份遊戲狀態 | `UpdateFn`／`CollideFn` 回呼函式 |
| 每個實例需要不同的計時器、方向或生命值 | 自訂 `GridObject` |
| 更新、碰撞與繪製都要共同使用個別資料 | 自訂 `GridObject` |

完整 Pacman 範例中的 `Ghost` 各自保存移動時間、顏色和行為模式，`Pacman` 則保存目前方向與下一個想轉向的方向，因此兩者都採用繼承。同一個遊戲也可以同時使用回呼函式與自訂類別，只要每個選擇都符合資料的實際歸屬。需要查閱函式式物件的內部介面時，可參考 [CallbackGridObject API](../api/classgridpp_1_1_callback_grid_object.md)。

## 本節小結

- 繼承 `GridObject` 後，每個實例都能保存自己的成員變數。
- `Spawn()` 接管自訂物件的所有權，呼叫端只保留借用指標。
- `UpdateFn`／`CollideFn` 適合使用既有或共享狀態，自訂類別適合保存個別狀態。
