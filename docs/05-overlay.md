# Overlay

有些內容屬於遊戲畫面，卻不屬於遊戲世界。分數顯示的是整局遊戲的資料，不是站在某一格的物件；按鈕接收滑鼠操作，也不應該因為玩家走到相同位置而發生網格碰撞。若把這些內容表示成 GridObject，就會把畫面配置和遊戲規則混在一起。

`Overlay` 是 Grid++ 對這類畫面內容的描述。它存在於網格世界上方，適合表示分數、剩餘時間、提示訊息、選單和按鈕。Overlay 不參與 GridObject 的同格碰撞，也不使用 GridObject 的 z-index；Engine 會在所有網格物件完成後才繪製 Overlay。

## 兩種座標系統

Overlay 的原點是視窗左上角。x 向右增加，y 向下增加，單位是像素，不受 `grid_size` 影響。

GridObject 使用網格座標，是因為它的位置代表遊戲規則中的一格。Overlay 使用像素座標，是因為文字和按鈕通常需要精確控制畫面排版。例如 `Label` 位於 `(12, 12)` 表示距離視窗左方與上方各 12 像素，不是位於第 12 欄、第 12 列。

Overlay 和 GridObject 的選擇取決於內容是否屬於遊戲世界。固定顯示在畫面上的準星可以是 Overlay；會占據格子並阻擋玩家的控制台則應是 GridObject。

## 顯示文字

`Label` 顯示一行文字。建構子依序接受文字、x、y、字體大小與顏色；字體大小預設為 20，顏色預設為 `BLACK`。

```cpp
auto* score_label = new gridpp::Label("Score: 0", 12, 12, 24, BLACK);
game.AddOverlay(score_label);

score_label->set_text("Score: 10");
```

`set_text()` 可以在遊戲執行期間更新內容。`AddOverlay()` 接管 Label 的所有權，`score_label` 只作為借用指標使用，不可自行 `delete`。

## 加入打地鼠的畫面資訊

打地鼠需要顯示分數與剩餘時間，但這兩項資料不屬於任何一格，因此使用 `Label`。Label 的借用指標保存在程式中，之後 callback 可以透過 `set_text()` 更新文字。

```cpp
#include <string>

int score = 0;
gridpp::Label* score_label = nullptr;
gridpp::Label* time_label = nullptr;

int main() {
    gridpp::GridEngine game(8, 8, 64);
    game.set_show_grid(true);

    game.Spawn("mole", 3, 4, nullptr);

    score_label = new gridpp::Label("Score: 0", 12, 12, 24, BLACK);
    time_label = new gridpp::Label("Time: 30", 12, 44, 24, BLACK);
    game.AddOverlay(score_label);
    game.AddOverlay(time_label);

    game.Run();
}
```

兩個 Label 會顯示在網格物件上方，即使地鼠移到左上角也不會遮住文字。這是固定的繪製層級關係，不需要替 Label 設定 z-index。若畫面資訊不應蓋住可點擊區域，可以增加 Engine 的列數保留資訊區，或把文字放在不會生成地鼠的格子範圍。

將借用指標設為全域變數是函式版小型範例的直接做法。較大型的程式可以把 UI 指標和遊戲狀態放入管理遊戲流程的類別；所有權仍由 Engine 持有。

## 顯示按鈕

`Button` 提供矩形按鈕、hover 顯示與滑鼠點擊判斷。建立自訂按鈕時，衍生 `Button` 並覆寫 `OnClick()`。

```cpp
class RestartButton : public gridpp::Button {
public:
    RestartButton() : Button("Restart", 12, 52, 120, 40) {}

    void OnClick() override {
        RestartGame();
    }
};

game.AddOverlay(new RestartButton());
```

`Button` 的位置與大小均使用像素。內建繪製使用固定 20 像素文字，並在按鈕範圍內置中。需要不同外觀、圖片或更複雜輸入行為時，應直接衍生 `Overlay`。

## 自訂 Overlay

自訂 Overlay 可覆寫 `OnUpdate()` 與 `Draw()`。`OnUpdate()` 在每幀的物件更新之後執行，`Draw()` 在全部 `GridObject` 繪製之後執行。

```cpp
class Crosshair : public gridpp::Overlay {
public:
    void Draw() override {
        const Vector2 mouse = GetMousePosition();
        DrawCircleLines(mouse.x, mouse.y, 10, RED);
    }
};

game.AddOverlay(new Crosshair());
```

在 Overlay callback 執行期間呼叫 `AddOverlay()` 是安全的。新加入的 Overlay 從下一幀開始更新與繪製，不會插入正在執行的本幀序列。Grid++ 目前沒有移除單一 Overlay 的公開 API；需要暫時顯示的內容可在自訂 `Draw()` 中根據遊戲狀態決定是否繪製。

Overlay 沒有 GridObject 的 tag、visible、z-index 或同格碰撞。若內容需要這些功能，它應該是 `GridObject`，即使外觀上看起來像畫面元件。

Label 顯示遊戲狀態轉換後的文字；分數的計算仍由遊戲規則負責。
