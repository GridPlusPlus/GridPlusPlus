# 製作打地鼠

Pac-Man 教學用鍵盤操作，這一章換個玩法，用滑鼠來打地鼠，同時把第 3 章整理的物件、行為、Engine、Overlay、輸入與時間實際用一次。遊戲的規則是：8×8 網格中有一隻每秒換位的地鼠，玩家在 30 秒內點中它便得到一分；倒數結束後，按 R 可以重設狀態並開始下一局。

接下來會依序加入地鼠、定時移動、點擊計分、倒數與重設；四個里程碑各自附有當下完整的 `main.cpp`，可以直接編譯並觀察新增的遊戲行為。

!!! tip "在新的專案裡做"

    打地鼠是一個新遊戲，建議不要寫在 Pac-Man 專案裡。用[取得你的專案](../01-getting-started/03-get-project.md)的方法 fork **GridPlusPlus-Template**（把網址中的 `GridPlusPlus-Pacman` 換成 `GridPlusPlus-Template`），再在新專案的 `main.cpp` 裡進行。

## 里程碑一：建立靜態地鼠

打地鼠從一個顯示格線的空白世界開始，現在缺少的是存在於世界中的遊戲目標。第一個里程碑暫時不處理移動和計分，只建立一隻靜止的地鼠，用來確認 GridObject 已加入 Engine，且指定的網格座標會正確反映在畫面上。把 `main.cpp` 改成：

```cpp title="main.cpp"
#include "GridPlusPlus.h"

using gridpp::GameEngine;

int main() {
    GameEngine game(8, 8, 64);
    game.showGrid(true);
    game.addObject("mole");
    game.run();
    return 0;
}
```

`addObject("mole")` 建立一個使用 `mole` 素材的物件；因為沒有交給它任何函式，這隻地鼠每幀只會維持原本狀態。由於尚未載入名為 `mole` 的素材，Engine 會用紅色方塊代替；這讓遊戲規則不必等待美術素材。

執行後，紅色方塊應位於左上角 `(0, 0)`，因為新物件的預設位置就是原點。接著試著把它放到別的地方：保留 `addObject()` 回傳的 Handler，再呼叫 `setPosition()`。

```cpp title="main() 節錄：移動靜態地鼠"
gridpp::GridObject mole = game.addObject("mole");
mole.setPosition(3, 2);
```

重新編譯執行，方塊應出現在第 4 欄、第 3 列。若能在執行前預測位置，就已理解零起算的網格座標。

## 里程碑二：讓地鼠每秒移動

靜止的方塊已經證明物件能進入世界，但打地鼠的目標必須定期換位，玩家才需要追蹤它。第 3 章提到 Engine 每幀都會執行物件的 Update 函式；如果函式每次執行都更換座標，地鼠一秒可能移動約 60 次，快到無法遊玩。因此這一步除了加入 Update 函式，還要保存「下一次允許移動的時間」，讓尚未到達指定時間的幀直接返回。

```cpp title="main.cpp"
#include "GridPlusPlus.h"

using gridpp::GameEngine;
using gridpp::GridObject;

double next_move = 0.0;

void UpdateMole(GameEngine game, GridObject mole) {
    if (game.time() < next_move) return;

    mole.setPosition(game.random(0, game.cols() - 1), game.random(0, game.rows() - 1));
    next_move = game.time() + 1.0;
}

int main() {
    GameEngine game(8, 8, 64);
    game.showGrid(true);
    game.addObject("mole", nullptr, UpdateMole);
    game.run();
    return 0;
}
```

`addObject()` 的第二個參數是 Init、第三個是 Update。這裡暫時不需要 Init，所以傳入 `nullptr`，並把 `UpdateMole` 放在第三個位置。函式的第二個參數取名為 `mole` 而不是 `self`，只是讓程式讀起來更貼近這個遊戲；名稱可以自由決定，型別與順序才是 Engine 在意的部分。

第一次更新時 `next_move` 是 0，因此地鼠立即選擇新位置，再把下次移動排在一秒後。後續大部分幀都會在第一個 `if` 返回。`random(min, max)` 會回傳包含上下界的整數；有效 x 座標是 0 到 `cols() - 1`，有效 y 座標則是 0 到 `rows() - 1`，所以地鼠不會跑出網格。

此處的 `next_move` 在概念上屬於地鼠，卻暫時放在全域，讓普通函式能保存下一幀仍要使用的資料。這種寫法只適合目前的一隻地鼠：若加入第二隻，兩者會共用同一個移動時間。第 6 章會把個別狀態存回各自的物件。

這個版本執行後，紅色方塊應每秒移動一次，而且永遠留在網格範圍內。為了確認真正控制速度的是下一次移動時間，而不是 Engine 的幀率，可以把 `1.0` 暫時改成 `0.2`：在重新執行前，先預測地鼠會變成每 0.2 秒移動，再以畫面驗證推論，最後將數值改回來。

參考程式：[step_02_mole.cpp](https://github.com/GridPlusPlus/GridPlusPlus/blob/main/examples/whack_a_mole/step_02_mole.cpp)

## 里程碑三：判斷點擊並顯示分數

能夠移動的地鼠仍然只是動畫，因為玩家目前無法和它互動。要判斷玩家是否點中地鼠，程式必須比較滑鼠與地鼠的位置；問題在於滑鼠使用像素座標，地鼠使用網格座標，兩者不能直接相比。由於每格寬高都是 `gridSize()` 像素，將滑鼠的 x、y 分別除以格子大小，就能換算出滑鼠所在的格子：

```text
mouse pixel (220, 150) ÷ grid size 64 → grid cell (3, 2)
```

這一步新增兩種全域狀態：`score` 是遊戲規則的資料；`score_label` 是分數文字的 Handler。`score_label` 在全域宣告時還沒有指向任何文字，要等 `main()` 呼叫 `addTextOverlay()` 後才被指定；在那之前使用它會丟出錯誤，所以指定必須發生在 `run()` 之前。

```cpp title="main.cpp"
#include <string>

#include "GridPlusPlus.h"

using gridpp::GameEngine;
using gridpp::GridObject;
using gridpp::Overlay;

int score = 0;
double next_move = 0.0;
Overlay score_label;

void UpdateMole(GameEngine game, GridObject mole) {
    const int mouse_x = game.mouseX() / game.gridSize();
    const int mouse_y = game.mouseY() / game.gridSize();
    if (game.mousePressed(MOUSE_BUTTON_LEFT) && mouse_x == mole.x() && mouse_y == mole.y()) {
        ++score;
        score_label.setText("Score: " + std::to_string(score));
        next_move = 0.0;
    }

    if (game.time() >= next_move) {
        mole.setPosition(game.random(0, game.cols() - 1), game.random(0, game.rows() - 1));
        next_move = game.time() + 1.0;
    }
}

int main() {
    GameEngine game(8, 8, 64);
    game.showGrid(true);

    score_label = game.addTextOverlay("Score: 0", 12, 12, 24);
    game.addObject("mole", nullptr, UpdateMole);

    game.run();
    return 0;
}
```

每幀先把滑鼠像素換成網格座標，再檢查左鍵是否剛被按下且兩組座標是否相同。打中後分數增加，文字顯示新值；`std::to_string()` 把整數轉成字串，才能和 `"Score: "` 接在一起。最後 `next_move = 0.0` 讓地鼠在同一次 Update 的後半立刻換位，避免停在原格被連續點擊。

執行後，點中地鼠應使左上角分數增加，而且地鼠立即換位；點擊其他格不應加分。把 `mouse_x == mole.x()` 暫時改成 `mouse_x != mole.x()`，先預測錯誤行為，再執行確認並還原。這個反例能驗證得分來自座標比較，而不是 Overlay 或素材。

## 里程碑四：加入倒數、結束與重設

最後一個里程碑分成兩個小步驟：先讓這一局能夠倒數並停止，再讓玩家重設同一組物件。兩個步驟共同使用 `end_time` 和第二個文字 Overlay；`end_time` 保存這一局結束的絕對時間，每幀用「結束時間減去目前時間」得到剩餘秒數。計算時，`std::max()` 避免畫面顯示負數，`std::ceil()` 則讓尚未完整經過的一秒仍顯示出來，例如剩下 4.2 秒時顯示 5。

### 先讓一局遊戲正確結束

先只處理倒數與停止：在 `UpdateMole()` 開頭更新時間文字，時間歸零後隱藏地鼠並直接返回。此時不必急著加入重設功能，先確認倒數到 0 之後，地鼠確實不再移動或得分。

```cpp title="UpdateMole() 開頭：倒數結束後停止遊戲"
const double remaining = std::max(0.0, end_time - game.time());
const int seconds = static_cast<int>(std::ceil(remaining));
time_label.setText("Time: " + std::to_string(seconds));

if (remaining <= 0.0) {
    mole.hide();
    return;
}
```

這段程式成立後，一局遊戲已經有明確的開始與結束；接下來的重設不是另一套遊戲流程，而是把分數、時間、地鼠位置與畫面重新帶回可玩的初始狀態。

### 再加入重設功能

遊戲結束時，`hide()` 讓地鼠不再繪製。隱藏物件仍會執行 Update 函式，所以 `UpdateMole` 可以繼續等待 R 鍵；按下後，`ResetGame()` 重設整局狀態並重新顯示原物件，不需要建立第二個 Engine 或第二隻地鼠。

一局開始的時刻也需要同樣的重設。這正是 Init 函式的用途：`addObject()` 的第二個參數會在 `run()` 開始、第一幀之前執行一次，因此 `InitMole` 只要呼叫 `ResetGame()`，第一局與之後按 R 開始的每一局就會使用完全相同的初始化程式。

```cpp title="main.cpp"
#include <algorithm>
#include <cmath>
#include <string>

#include "GridPlusPlus.h"

using gridpp::GameEngine;
using gridpp::GridObject;
using gridpp::Overlay;

// 整局遊戲的狀態。
int score = 0;
double end_time = 0.0;

// 單一地鼠的狀態；第 6 章會把它存回物件自己身上。
double next_move = 0.0;

// 兩行文字的 Handler，在 main() 中指定。
Overlay score_label;
Overlay time_label;

void ResetGame(GameEngine game, GridObject mole) {
    score = 0;
    end_time = game.time() + 30.0;
    next_move = 0.0;
    mole.show();
    score_label.setText("Score: 0");
    time_label.setText("Time: 30");
}

void InitMole(GameEngine game, GridObject mole) { ResetGame(game, mole); }

void UpdateMole(GameEngine game, GridObject mole) {
    const double remaining = std::max(0.0, end_time - game.time());
    const int seconds = static_cast<int>(std::ceil(remaining));
    time_label.setText("Time: " + std::to_string(seconds));

    if (remaining <= 0.0) {
        mole.hide();
        if (game.keyPressed(KEY_R)) ResetGame(game, mole);
        return;
    }

    const int mouse_x = game.mouseX() / game.gridSize();
    const int mouse_y = game.mouseY() / game.gridSize();
    if (game.mousePressed(MOUSE_BUTTON_LEFT) && mouse_x == mole.x() && mouse_y == mole.y()) {
        ++score;
        score_label.setText("Score: " + std::to_string(score));
        next_move = 0.0;
    }

    if (game.time() >= next_move) {
        mole.setPosition(game.random(0, game.cols() - 1), game.random(0, game.rows() - 1));
        next_move = game.time() + 1.0;
    }
}

int main() {
    GameEngine game(8, 8, 64);
    game.showGrid(true);

    score_label = game.addTextOverlay("Score: 0", 12, 12, 24);
    time_label = game.addTextOverlay("Time: 30", 12, 44, 24);
    game.addObject("mole", InitMole, UpdateMole);

    game.run();
    return 0;
}
```

注意 `main()` 中建立內容的順序：兩行文字先建立，地鼠後建立。`InitMole` 在 `run()` 開始時才執行，那時 `score_label` 與 `time_label` 都已指向真正的文字，`ResetGame()` 才能安全地修改它們。

參考程式：[完整打地鼠](https://github.com/GridPlusPlus/GridPlusPlus/blob/main/examples/whack_a_mole/main.cpp)

## 編譯與驗收

四個里程碑都是彼此獨立的完整程式，因此每完成一步，都要重新編譯執行：

--8<-- "docs_includes/run-command.md"

編譯成功只代表程式符合語法與連結要求，還不能證明遊戲規則正確。執行完成版後，應依序操作並確認以下行為；任何一項不符合，都表示對應里程碑仍有尚未解決的問題：

- 開始時顯示 `Score: 0` 與 `Time: 30`。
- 地鼠每秒換位，且位置不超出 8×8 網格。
- 只有點中地鼠所在格才加分；加分後地鼠立即換位。
- 倒數不顯示負數；到 0 時地鼠消失且點擊不再得分。
- 按 R 後分數與時間重設，原本的地鼠重新出現。

## 回頭解釋這份程式

完成遊戲後，再回頭看整份程式，各段程式的責任應比一開始更清楚。這個小型範例刻意使用普通函式與全域狀態，讓 Update 函式如何取得目前物件、像素如何換成網格座標，以及 Engine 如何保存內容都直接顯露出來。這不是大型遊戲的最終架構，而是用最少機制建立後續章節所需的共同經驗：

- `GameEngine` 建立世界並逐幀呼叫行為。
- 地鼠是 `GridObject`；`InitMole` 與 `UpdateMole` 是交給它的 Init 與 Update 函式。
- `score` 與 `end_time` 屬於整局遊戲，`next_move` 概念上屬於單一地鼠。
- 分數與時間是文字 Overlay，只呈現狀態；程式透過 Handler 修改它們的文字。
- `hide()` 停止繪製與碰撞，但保留更新行為，因此可等待重設。

目前只有一隻地鼠，因此上述簡化還不會造成衝突；當遊戲開始出現不同碰撞規則或多個各自計時的物件時，全域狀態便會逐漸失去表達能力。第 5 章將完整說明 Init、Update 與 Collide 三種函式，第 6 章再讓每個物件保存自己的資料。如此一來，寫法的改變由實際問題推動，而不是只為了使用更複雜的語法。

[定義物件行為](../05-object-behavior/index.md){ .md-button .md-button--primary }
