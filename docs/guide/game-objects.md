# 遊戲物件 (OOP)

遊戲裡每個會動、會畫、會互動的東西，都是一個 `GridObject`。
這是你練習物件導向的主舞台：**繼承它，然後覆寫想要的虛擬函式**。

!!! tip "還沒學過類別？"
    可以先從 [函式版物件](callback-grid-object.md) 開始——用「宣告函式再傳進去」的方式，
    不必先懂繼承就能做出會動的角色；學會 class 後再回到這一頁的正規寫法。

## 三個生命週期函式

引擎會在對的時機自動呼叫它們，你只要覆寫（override）需要的：

| 函式 | 何時被呼叫 | 你通常在裡面做 |
|---|---|---|
| `OnSpawn()` | 被 `Spawn()` 放進世界時，呼叫一次 | 初始化、印訊息 |
| `OnUpdate()` | 每一幀 | 讀鍵盤、改變座標 |
| `OnCollide(other)` | 和另一物件踩在同一格時 | 判斷對方是誰、做反應 |

```cpp
class Player : public GridObject {
public:
    Player() : GridObject("hero", 5, 5) {}

    void OnSpawn() override {
        std::cout << "玩家出生於 (" << x() << ", " << y() << ")\n";
    }
    void OnUpdate() override {
        if (IsKeyPressed(KEY_UP))    Move(0, -1);
        if (IsKeyPressed(KEY_DOWN))  Move(0,  1);
        if (IsKeyPressed(KEY_LEFT))  Move(-1, 0);
        if (IsKeyPressed(KEY_RIGHT)) Move( 1, 0);
    }
};
```

## 座標與封裝

座標是 `private`（封裝起來），外界與子類別都透過方法存取：

| 方法 | 說明 |
|---|---|
| `x()` / `y()` | 讀目前所在格 |
| `set_x(x)` / `set_y(y)` | 直接設定座標 |
| `Move(dx, dy)` | 相對移動（在格子上瞬間移動） |

## 建構子重載

`GridObject` 提供兩個建構子，示範「重載（overload）」：

```cpp
GridObject();                                   // 空的（座標 0,0，無素材）
GridObject(std::string asset_name, int x, int y); // 指定素材與起始座標
```

子類別可以選擇要呼叫哪一個。

## 標籤 (tag)

碰撞時，你常需要知道「撞到的是誰」。用 `tag` 給物件貼一個身分標記：

```cpp
class Ghost : public GridObject {
public:
    Ghost(int x, int y) : GridObject("ghost", x, y) { set_tag("ghost"); }
};

// 在玩家的 OnCollide 裡分辨對方：
void OnCollide(GridObject* other) override {
    if (other->tag() == "ghost") GameOver();
}
```

## 自訂畫法：覆寫 Render

預設情況下，物件會畫出自己 `asset_name` 對應的素材。需要特別的畫法時可以覆寫 `Render()`：

```cpp
void Render(GridEngine* engine) override {
    GridObject::Render(engine);                  // 先用預設方式畫自己
    DrawText("HP: 100", 8, 8, 18, WHITE);        // 再疊加額外的東西
}
```

## 旋轉與調色（重複利用同一張素材）

每個物件都有兩個內建的繪製狀態，讓你用「一張圖」變出多種樣子：

| 方法 | 說明 |
|---|---|
| `set_direction(int dir)` | 朝向 `0~3`，逆時針每 +1 轉 90°（`0` = 素材原本方向）。 |
| `set_tint(Color c)` | 調色：把素材整體乘上一個顏色，預設 `WHITE`（不變）。 |

**旋轉**：例如一張「朝右」的小精靈圖，移動時依方向 `set_direction` 就能朝上下左右，不必畫四張：

```cpp
// 右=0, 上=1, 左=2, 下=3
if (moving_up) set_direction(1);
```

**調色（diffuse color）**：把一張白色的鬼魂素材染成不同顏色，做出四隻不同的鬼：

```cpp
ghost1->set_tint(RED);
ghost2->set_tint(PINK);
ghost3->set_tint(SKYBLUE);
ghost4->set_tint(ORANGE);
```

原理是繪製時把每個像素的 RGB 乘上 tint 顏色：白色乘任何色 = 那個色，所以**素材畫成白/灰底**最容易染色。

!!! tip "找不到素材會怎樣"
    如果物件的 `asset_name` 在素材包裡不存在，引擎會在那一格畫一個紅色方塊，
    方便你一眼看出「素材沒對上」。
