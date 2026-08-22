# 製作打地鼠

現在我們用一個完整專案接觸 Grid++ 的主要功能。遊戲會在 8×8 網格中顯示一個地鼠；地鼠每秒更換位置，玩家在 30 秒內點中它就得到一分。時間結束後，按 R 可以重新開始。

這一章會使用 `GridEngine`、`GridObject`、callback、借用指標與 `Label`。我們先學會在實際程式中使用它們，後面的章節再分別討論完整行為。

## 建立遊戲板

從上一章的最小程式開始。建立 `main.cpp`，先設定 8×8 網格：

```cpp title="main.cpp"
#include "GridPlusPlus.h"

using gridpp::GridEngine;

int main() {
    GridEngine game(8, 8, 64);
    game.set_show_grid(true);
    game.Run();
    return 0;
}
```

參考程式：[step_01_board.cpp](https://github.com/GridPlusPlus/GridPlusPlus/blob/main/examples/whack_a_mole/step_01_board.cpp)

在遊戲專案根目錄編譯並執行這個版本。以下指令適用於 WSL 與 Linux；其他平台使用上一章的對應連結參數。

```bash
g++ -std=c++17 main.cpp -o game \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```

畫面會顯示 8×8 空白網格，接著要把地鼠放進去。

## 生成第一個物件

Grid++ 用 `GridObject` 表示存在網格中的物件。玩家、敵人與道具都可以使用這個共同介面。`Spawn()` 會建立物件、把它加入 Engine，並回傳指向該物件的指標。

在 `Run()` 前加入以下程式：

```cpp
game.Spawn("mole", 0, 0, nullptr);
```

這邊的四個參數分別是素材名稱、x 座標、 y 座標與每個 frame 的更新函式。最後的 `nullptr` 表示目前沒有更新函式。完整的 `main()` 變成：

```cpp title="main.cpp"
int main() {
    GridEngine game(8, 8, 64);
    game.set_show_grid(true);
    game.Spawn("mole", 0, 0, nullptr);
    game.Run();
    return 0;
}
```

執行後，左上角會出現紅色方塊。我們還沒有載入名為 `mole` 的素材，Grid++ 會用紅色方塊顯示缺少素材的物件。這個 fallback 讓遊戲規則可以在美術完成前先運作。

網格左上角的座標是 `(0, 0)`。x 向右增加，y 向下增加，所以 `(3, 2)` 表示第 4 欄、第 3 列。

## 把函式交給物件

地鼠需要定期移動。Engine 已經每秒執行約 60 個 frame，我們只要提供一個函式，Engine 就會在每個 frame 呼叫它。這種交給其他程式、等待特定時機被呼叫的函式稱為 callback。

在 `main()` 上方加入更新函式與下一次移動時間：

```cpp
using gridpp::GridObject;

double next_move = 0.0;

void UpdateMole(GridObject* mole) {
    if (GetTime() < next_move) return;

    mole->set_x(GetRandomValue(0, mole->engine()->cols() - 1));
    mole->set_y(GetRandomValue(0, mole->engine()->rows() - 1));
    next_move = GetTime() + 1.0;
}
```

`mole` 指向目前正在更新的物件。`set_x()` 和 `set_y()` 修改它的網格座標；`engine()` 取得所屬 Engine，讓函式能讀取欄數與列數。

`GetRandomValue(min, max)` 是 raylib 的隨機整數函式，回傳值包含 `min` 和 `max`。因此 `GetRandomValue(0, cols - 1)` 會選出有效的欄座標，列座標也使用相同方式產生。

`GetTime()` 回傳 raylib 視窗建立後經過的秒數，型別是 `double`。`next_move` 保存下一次移動的時間；目前時間到達它時，地鼠移動，再以 `GetTime() + 1.0` 將下一次移動排在一秒後。

把 `Spawn()` 的最後一個參數改成 `UpdateMole`：

```cpp
game.Spawn("mole", 0, 0, UpdateMole);
```

現在 Engine 會在每個 frame 呼叫 `UpdateMole`。大部分 frame 都會在第一個 `if` 返回；時間到達 `next_move` 時，地鼠才換到新位置，並把下一次移動排到一秒後。

此時的完整程式如下：

```cpp title="main.cpp"
#include "GridPlusPlus.h"

using gridpp::GridEngine;
using gridpp::GridObject;

double next_move = 0.0;

void UpdateMole(GridObject* mole) {
    if (GetTime() < next_move) return;

    mole->set_x(GetRandomValue(0, mole->engine()->cols() - 1));
    mole->set_y(GetRandomValue(0, mole->engine()->rows() - 1));
    next_move = GetTime() + 1.0;
}

int main() {
    GridEngine game(8, 8, 64);
    game.set_show_grid(true);
    game.Spawn("mole", 0, 0, UpdateMole);
    game.Run();
    return 0;
}
```

參考程式：[step_02_mole.cpp](https://github.com/GridPlusPlus/GridPlusPlus/blob/main/examples/whack_a_mole/step_02_mole.cpp)

重新編譯並執行第二個里程碑：

```bash
g++ -std=c++17 main.cpp -o game \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```

紅色方塊應該每秒移動一次，而且不會超出 8×8 網格。

## 判斷玩家點了哪一格

滑鼠位置使用像素座標，地鼠使用網格座標。每格寬 64 像素，因此可以用像素座標除以格子尺寸，得到滑鼠所在的格子。

```cpp
const Vector2 mouse = GetMousePosition();
const int grid_size = mole->engine()->grid_size();
const int mouse_x = static_cast<int>(mouse.x) / grid_size;
const int mouse_y = static_cast<int>(mouse.y) / grid_size;
```

按下滑鼠左鍵，而且兩組座標相同時，玩家就打中了地鼠：

```cpp
if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
    mouse_x == mole->x() && mouse_y == mole->y()) {
    ++score;
    next_move = 0.0;
}
```

把 `next_move` 設成 0 會讓地鼠在同一個 update 的後半段立刻換位，玩家不能在原位置連續得分。

## 顯示分數與時間

分數和剩餘時間固定顯示在視窗上方，不占據遊戲中的某一格。Grid++ 用 `Overlay` 表示這類畫面資訊，`Label` 是內建的文字 Overlay。

在全域狀態加入兩個 Label 指標：

```cpp
int score = 0;
double end_time = 0.0;
Label* score_label = nullptr;
Label* time_label = nullptr;
```

在 `main()` 中建立 Label，再交給 Engine：

```cpp
score_label = new Label("Score: 0", 12, 12, 24, BLACK);
time_label = new Label("Time: 30", 12, 44, 24, BLACK);
game.AddOverlay(score_label);
game.AddOverlay(time_label);
```

`AddOverlay()` 接管 Label 的所有權。`score_label` 與 `time_label` 是借用指標，我們可以用它們更新文字，但不能自行 `delete`。

打中地鼠時更新分數文字：

```cpp
++score;
score_label->set_text("Score: " + std::to_string(score));
```

每個 frame 計算剩餘時間，並用 `std::ceil()` 顯示完整秒數：

```cpp
const double remaining = std::max(0.0, end_time - GetTime());
const int seconds = static_cast<int>(std::ceil(remaining));
time_label->set_text("Time: " + std::to_string(seconds));
```

這些標準函式需要在檔案上方加入 `<algorithm>`、`<cmath>` 與 `<string>`。

## 結束與重新開始

剩餘時間到達 0 時，呼叫 `set_visible(false)` 隱藏地鼠。隱藏物件不會繪製或參與碰撞，但 update callback 仍會執行，所以我們可以繼續等待 R 鍵。

```cpp
if (remaining <= 0.0) {
    mole->set_visible(false);
    if (IsKeyPressed(KEY_R)) ResetGame(mole);
    return;
}
```

`ResetGame()` 重設共用狀態，重新顯示原本的物件：

```cpp
void ResetGame(GridObject* mole) {
    score = 0;
    end_time = GetTime() + 30.0;
    next_move = 0.0;
    mole->set_visible(true);
    score_label->set_text("Score: 0");
    time_label->set_text("Time: 30");
}
```

## 完整程式

Listing 2-1 是完成的 `main.cpp`。

參考程式：[完整打地鼠](https://github.com/GridPlusPlus/GridPlusPlus/blob/main/examples/whack_a_mole/main.cpp)

```cpp title="main.cpp"
#include "GridPlusPlus.h"

#include <algorithm>
#include <cmath>
#include <string>

using gridpp::GridEngine;
using gridpp::GridObject;
using gridpp::Label;

int score = 0;
double end_time = 0.0;
double next_move = 0.0;
Label* score_label = nullptr;
Label* time_label = nullptr;

void ResetGame(GridObject* mole) {
    score = 0;
    end_time = GetTime() + 30.0;
    next_move = 0.0;
    mole->set_visible(true);
    score_label->set_text("Score: 0");
    time_label->set_text("Time: 30");
}

void UpdateMole(GridObject* mole) {
    const double remaining = std::max(0.0, end_time - GetTime());
    const int seconds = static_cast<int>(std::ceil(remaining));
    time_label->set_text("Time: " + std::to_string(seconds));

    if (remaining <= 0.0) {
        mole->set_visible(false);
        if (IsKeyPressed(KEY_R)) ResetGame(mole);
        return;
    }

    const Vector2 mouse = GetMousePosition();
    const int grid_size = mole->engine()->grid_size();
    const int mouse_x = static_cast<int>(mouse.x) / grid_size;
    const int mouse_y = static_cast<int>(mouse.y) / grid_size;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouse_x == mole->x() && mouse_y == mole->y()) {
        ++score;
        score_label->set_text("Score: " + std::to_string(score));
        next_move = 0.0;
    }

    if (GetTime() >= next_move) {
        mole->set_x(GetRandomValue(0, mole->engine()->cols() - 1));
        mole->set_y(GetRandomValue(0, mole->engine()->rows() - 1));
        next_move = GetTime() + 1.0;
    }
}

int main() {
    GridEngine game(8, 8, 64);
    game.set_show_grid(true);

    score_label = new Label("Score: 0", 12, 12, 24, BLACK);
    time_label = new Label("Time: 30", 12, 44, 24, BLACK);
    game.AddOverlay(score_label);
    game.AddOverlay(time_label);

    GridObject* mole = game.Spawn("mole", 0, 0, UpdateMole);
    ResetGame(mole);
    game.Run();
    return 0;
}
```

再次編譯 `main.cpp`。以下指令適用於 WSL 與 Linux：

```bash
g++ -std=c++17 main.cpp -o game \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```

遊戲現在已經可以計分、倒數、結束與重新開始。我們使用了 Engine、GridObject、callback 和 Overlay，但只解釋了完成專案需要的部分。接下來會回到這份程式，建立這些元素的完整模型。

## Summary

- `GridEngine` 建立並執行 8×8 遊戲世界。
- `Spawn()` 加入網格物件，並回傳借用指標。
- update callback 在每個 frame 收到目前物件的 `GridObject*`。
- `Label` 使用像素座標顯示分數和時間。
- `set_visible(false)` 隱藏物件，同時保留 update 行為。

[理解 Grid++ 核心模型](../03-core-model/index.md){ .md-button .md-button--primary }
