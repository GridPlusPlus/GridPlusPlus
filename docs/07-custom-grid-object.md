# 物件狀態與自訂 GridObject

遊戲狀態是會隨執行過程改變，而且之後仍需要讀取的資料。分數屬於整局遊戲的狀態；位置是每個 GridObject 已經內建的狀態；敵人的生命值、移動間隔和追蹤目標則可能只屬於某一個 instance。

Callback 函式本身不會替每個物件建立一份區域資料。兩個物件使用同一個 callback 時，函式內容相同，函式中的全域或 static 變數也只有一份。只要每個 instance 需要記住不同資訊，共用變數就無法直接表達資料屬於誰。

C++ class 可以把資料與使用資料的行為放在一起。建立一個繼承 `GridObject` 的 class 後，每次建立 instance 都會得到自己的一組成員變數；覆寫的生命週期函式則可以直接讀寫這些變數。這種做法在 GridObject 的共通能力上增加特定遊戲需要的狀態，並繼續使用原有的 Grid++ API。

例如每隻 Pacman 鬼都需要自己的方向、移動時間和追蹤目標，因此可建立 `Ghost` class：

```cpp
class Ghost : public GridObject {
public:
    Ghost(int x, int y, Color color)
        : GridObject("ghost", x, y), move_time_(0.0) {
        set_tint(color);
        set_tag("ghost");
    }

    void OnUpdate() override {
        if (GetTime() < move_time_) return;
        Move(GetRandomValue(-1, 1), 0);
        move_time_ = GetTime() + 0.25;
    }

private:
    double move_time_;
};
```

每次 `new Ghost(...)` 都會建立獨立的 `move_time_`。更新其中一隻鬼不會修改其他鬼的計時器，這正是 callback 搭配單一全域變數無法表達的狀態。

## 讓多隻地鼠保存自己的時間

單一地鼠可以使用全域 `next_move`。若同時 spawn 三隻地鼠，三個 callback 都會讀寫同一個變數：第一隻更新時間後，另外兩隻也會被迫等待。將時間改成 `Mole` 的成員變數後，每個 instance 都有獨立的出現週期。

```cpp
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

`interval_` 和 `next_move_` 都屬於單一 `Mole`。建構子參數還能讓每隻地鼠使用不同速度，而不需要建立多個名稱不同但內容相同的 callback。

```cpp
game.Spawn(new Mole(1, 1, 1.0));
game.Spawn(new Mole(3, 3, 0.8));
game.Spawn(new Mole(6, 5, 1.4));
```

這項修改沒有改變 Engine 的使用方式。`Mole` 仍是 `GridObject`，所以相同的座標、素材、顯示、碰撞、z-index 和所有權規則全部適用。物件導向版本只為每個 instance 增加獨立的資料空間。

## 建構子與 OnSpawn

建構子負責建立物件本身，不應假設物件已經屬於某個引擎。此時 `engine()` 仍是 `nullptr`。需要讀取網格尺寸、生成其他物件或執行依賴引擎的初始化時，覆寫 `OnSpawn()`。

```cpp
class Enemy : public GridObject {
public:
    Enemy(int x, int y) : GridObject("enemy", x, y) {}

    void OnSpawn() override {
        set_x(engine()->cols() - 1);
    }
};
```

在主迴圈外呼叫 `Spawn()` 時，`OnSpawn()` 會在 `Spawn()` 回傳前執行。在 `OnUpdate()` 或碰撞 callback 內 spawn 的物件會排到幀末，屆時執行 `OnSpawn()`，並從下一幀開始更新、碰撞與繪製。若 `OnSpawn()` 丟出例外，引擎會移除並釋放該物件，再將例外交給呼叫端。

## 覆寫行為

`GridObject` 提供四個可覆寫函式：

| 函式 | 執行時機 |
|---|---|
| `OnSpawn()` | 物件成功加入引擎後執行一次。 |
| `OnUpdate()` | 每幀更新階段執行。 |
| `OnCollide(other)` | 與另一個物件位於同一格時執行。 |
| `Render(engine)` | 每幀繪製階段執行。 |

只覆寫需要改變的行為。未覆寫 `Render()` 時，基底類別會使用 `asset_name()`、`direction()` 和 `tint()` 繪製素材。自訂繪製可直接使用 raylib，也可呼叫 `engine->DrawCell()` 重用素材繪製。

```cpp
void Render(GridEngine* engine) override {
    engine->DrawCell(asset_name(), x(), y(), direction(), tint());
    DrawCircle(
        x() * engine->grid_size() + engine->grid_size() / 2,
        y() * engine->grid_size() + engine->grid_size() / 2,
        4,
        RED
    );
}
```

## 建立與借用物件

自訂物件仍透過 `Spawn()` 加入引擎。`Spawn(GridObject*)` 的回傳型別是 `GridObject*`；需要呼叫衍生類別特有函式時，可先保留原本的衍生類別指標。

```cpp
Ghost* ghost = new Ghost(3, 4, RED);
game.Spawn(ghost);

// ghost 是借用指標，所有權已轉交給 game。
```

不要在 `Spawn()` 後自行釋放 `ghost`。如果只使用 `GridObject` 的共通操作，可以直接寫成 `GridObject* ghost = game.Spawn(new Ghost(...));`，降低程式對實際類別的依賴。

## Callback 與自訂類別的選擇

兩種 API 使用相同的引擎、碰撞與繪製機制。選擇時應判斷物件是否需要自己的額外狀態；遊戲規模不影響這項選擇。

| 情況 | 建議方式 |
|---|---|
| 物件只需要位置、素材、tag 等內建狀態 | Callback |
| 所有物件共用一份遊戲狀態 | Callback |
| 每個 instance 需要不同的計時器或方向 | 自訂 `GridObject` |
| 行為需要多個互相配合的函式 | 自訂 `GridObject` |
| 需要自訂繪製 | 自訂 `GridObject` |

Pacman 的玩家可以使用任一方式；多隻鬼通常適合自訂類別，因為每隻鬼需要獨立保存移動狀態。Callback 適合處理無額外 instance 狀態的物件，並不代表較低階或暫時性的 API。

完整 Pacman 範例中的 `Ghost` 各自保存移動時間、顏色和行為模式，`Player` 則保存目前方向與下一個想轉向的方向。這些資料在同一類別的不同 instance 之間不能共用，因此適合作為衍生 `GridObject` 的案例。迷宮本身的牆面資料則由後續介紹的 `GridMaze` 保存。
