# 畫面覆蓋層

遊戲世界是網格，但分數、訊息、按鈕這些東西不該被綁在格子上。Grid++ 提供一個
**畫面覆蓋層**：跳脫網格、用**像素座標**自由繪製，並且畫在所有網格物件「之上」。

- `Overlay`、`Label`、`Button` 都定義在 `Overlay.h`；主標頭 `GridPlusPlus.h` 會自動引入。

## 加入覆蓋層

和 `Spawn(GridObject*)` 對稱，用 `AddOverlay(Overlay*)` 把內容放進覆蓋層：

```cpp
game.AddOverlay(new ScoreOverlay());
game.AddOverlay(new MyButton(10, 10, 80, 30));
```

引擎每幀會對每個覆蓋層呼叫 `OnUpdate()`（互動）與 `Draw()`（繪製），`Draw()` 在網格畫完後才執行，所以一定蓋在最上面。

## 自訂覆蓋層：繼承 Overlay

需要完全自訂的畫面（例如會變動的分數），就繼承 `Overlay`、覆寫 `Draw()`：

```cpp
class ScoreOverlay : public Overlay {
public:
    void Draw() override {
        DrawText(TextFormat("Score: %d", g_score), 8, 8, 20, YELLOW);
    }
};
```

`Draw()` 裡用的是 raylib 的像素繪圖函式（`DrawText`、`DrawRectangle`…），座標是像素、原點在左上角。視窗大小可用 `GetScreenWidth()` / `GetScreenHeight()` 查詢（方便置中）。

## 現成元件：Label

純文字標籤。

```cpp
#include "GridPlusPlus.h"

game.AddOverlay(new Label("方向鍵移動", 8, 8, 20, WHITE));
```

| 建構子 | 說明 |
|---|---|
| `Label(text, x, y, font_size = 20, color = BLACK)` | 在 (x, y) 畫一行文字。 |
| `set_text(text)` | 更新文字內容（例如即時分數）。 |

## 現成元件：Button

一個可點的矩形按鈕。**覆寫 `OnClick()`** 決定被點擊時要做什麼：

```cpp
class StartButton : public Button {
public:
    StartButton(int x, int y, int w, int h) : Button("Start", x, y, w, h) {}
    void OnClick() override {
        g_state = 1;        // 例如：開始遊戲
    }
};

game.AddOverlay(new StartButton(260, 300, 100, 40));
```

!!! tip "讓按鈕只在某個畫面出現"
    若想讓按鈕只在特定階段顯示與作用（例如「開始」只在開始畫面、「重新開始」只在結束畫面），
    可以覆寫 `OnUpdate()` 與 `Draw()`，在裡面判斷遊戲狀態，符合時才呼叫基底版本：

    ```cpp
    void OnUpdate() override { if (g_state == 0) Button::OnUpdate(); }
    void Draw()     override { if (g_state == 0) Button::Draw(); }
    ```

| 成員 | 說明 |
|---|---|
| `Button(text, x, y, w, h)` | 在像素矩形 (x, y, w, h) 畫一個有文字的按鈕。 |
| `OnClick()` | **覆寫它**：按鈕被滑鼠左鍵點擊時呼叫。 |

按鈕會自動偵測滑鼠是否在它上面（hover 時變色），點下去就呼叫 `OnClick()`——這些都由
`Button` 的 `OnUpdate()` 處理好了，你只要管 `OnClick()` 裡要做什麼。

!!! tip "Pac-Man 範例怎麼用"
    範例把分數與「YOU WIN / GAME OVER / PAUSED」訊息放在一個 `ScoreOverlay : Overlay`，
    右上角放一個 `PauseButton : Button`（`OnClick()` 切換暫停）。這樣覆蓋層就和遊戲角色
    完全分開，不再塞在某個角色的繪製裡。
