# CallbackGridObject

GridObject 說明一個遊戲實體具備哪些共通能力，但它不知道特定物件應如何行動。玩家可能根據按鍵移動，豆子可能保持不動並在碰撞後消失，地鼠則可能定時更換位置。這些規則必須由遊戲程式交給物件。

Callback 是「在指定事件發生時，由另一段程式回頭呼叫的函式」。程式先把函式指標交給 Engine；之後每逢更新或碰撞階段，Engine 會呼叫該函式。呼叫者不需要自行控制主迴圈，也不需要知道某一幀何時開始。

`CallbackGridObject` 是使用 callback 提供行為的 GridObject。它適合用普通函式表達規則，而且物件不需要額外 instance 狀態的情況。使用者通常呼叫函式式 `Spawn()`，由 Engine 在內部建立這個類別。

## 兩種物件行為寫法

Grid++ 提供兩種方式定義 GridObject 的行為。兩者使用相同的 Engine、座標、碰撞與繪製規則，差別在於程式如何保存行為所需的資料。

函式版物件把普通函式交給 `Spawn()`。Engine 建立 `CallbackGridObject`，並在更新或碰撞時呼叫指定函式。這種方式只需要函式和指標，適合牆壁、豆子或只有一份共用狀態的簡單物件。打地鼠使用這種 API。

自訂物件會建立一個繼承 `GridObject` 的 class。每個 instance 可以擁有自己的成員變數，例如每隻鬼各自的方向、速度與計時器。當 callback 開始需要用全域陣列替每個物件保存資料時，自訂 class 能直接表達資料屬於哪個物件。Pacman 範例使用這種方式建立玩家與多隻鬼。

兩種寫法可以出現在同一個遊戲中。玩家和敵人可以使用自訂 class，不需要額外狀態的豆子則繼續使用 callback。

## 函式式 Spawn

```cpp
GridObject* Spawn(
    const std::string& asset_name,
    int x,
    int y,
    CallbackGridObject::UpdateFn update,
    CallbackGridObject::CollideFn collide = nullptr
);
```

呼叫這個多載時，引擎在內部建立 `CallbackGridObject`。回傳值使用 `GridObject*`，因此位置、素材、tag、顏色、顯示狀態與 z-index 都透過一般 `GridObject` API 操作。

## 更新 callback

更新函式的型別是 `void (*)(GridObject* self)`。引擎每幀呼叫一次函式，`self` 指向目前正在更新的物件。下列函式使用方向鍵移動物件，並根據引擎尺寸阻止它離開網格。

```cpp
void MovePlayer(GridObject* self) {
    GridEngine* game = self->engine();

    if (IsKeyPressed(KEY_RIGHT) && self->x() < game->cols() - 1) {
        self->Move(1, 0);
    }
    if (IsKeyPressed(KEY_LEFT) && self->x() > 0) {
        self->Move(-1, 0);
    }
    if (IsKeyPressed(KEY_DOWN) && self->y() < game->rows() - 1) {
        self->Move(0, 1);
    }
    if (IsKeyPressed(KEY_UP) && self->y() > 0) {
        self->Move(0, -1);
    }
}

GridObject* player = game.Spawn("player", 4, 4, MovePlayer);
```

不需要每幀行為時，update 參數可以傳入 `nullptr`。物件仍會繪製並參與碰撞。

## 碰撞 callback

碰撞函式的型別是 `void (*)(GridObject* self, GridObject* other)`。兩個可見物件位於同一個有效網格座標時，引擎會分別呼叫兩邊的 callback。`self` 是目前收到通知的物件，`other` 是同格的另一個物件。

```cpp
int score = 0;

void EatPellet(GridObject* self, GridObject* other) {
    if (other->tag() != "player") return;

    ++score;
    self->engine()->Destroy(self);
}

GridObject* pellet = game.Spawn("pellet", 2, 3, nullptr, EatPellet);
pellet->set_tag("pellet");
```

碰撞 callback 可能在同一幀被呼叫多次，因為一格可以同時存在三個以上物件。引擎依 spawn 順序檢查每一組物件；其中一個 callback 將物件隱藏、移出地圖或呼叫 `Destroy()` 後，該物件不再參與該幀後續碰撞。

## 打地鼠範例

以下程式延續前幾章的 8×8 Engine、地鼠與 Label。地鼠每隔一秒移到隨機格；滑鼠點到目前格子時增加分數並立即換位置。剩餘時間歸零後，物件會隱藏並停止計分。範例使用預設紅色方塊，因此不需要素材包。

```cpp
#include "GridPlusPlus.h"

#include <algorithm>
#include <string>

using gridpp::GridEngine;
using gridpp::GridObject;

int score = 0;
double next_move = 0.0;
double end_time = 0.0;
gridpp::Label* score_label = nullptr;
gridpp::Label* time_label = nullptr;

void MoveMole(GridObject* self) {
    GridEngine* game = self->engine();
    const double remaining = std::max(0.0, end_time - GetTime());
    time_label->set_text("Time: " + std::to_string(static_cast<int>(remaining)));

    if (remaining <= 0.0) {
        self->set_visible(false);
        return;
    }

    const Vector2 mouse = GetMousePosition();
    const int mouse_x = static_cast<int>(mouse.x) / game->grid_size();
    const int mouse_y = static_cast<int>(mouse.y) / game->grid_size();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        mouse_x == self->x() && mouse_y == self->y()) {
        ++score;
        score_label->set_text("Score: " + std::to_string(score));
        next_move = 0.0;
    }

    if (GetTime() >= next_move) {
        self->set_x(GetRandomValue(0, game->cols() - 1));
        self->set_y(GetRandomValue(0, game->rows() - 1));
        next_move = GetTime() + 1.0;
    }
}

int main() {
    GridEngine game(8, 8, 64);
    game.set_show_grid(true);

    score_label = new gridpp::Label("Score: 0", 12, 12, 24, BLACK);
    time_label = new gridpp::Label("Time: 30", 12, 44, 24, BLACK);
    game.AddOverlay(score_label);
    game.AddOverlay(time_label);

    end_time = GetTime() + 30.0;
    game.Spawn("mole", 0, 0, MoveMole);
    game.Run();
}
```

`score`、`next_move` 和 `end_time` 是整個遊戲共用的狀態。`score_label` 與 `time_label` 是 Engine 擁有之 Overlay 的借用指標。callback 每幀先更新剩餘時間；時間結束時隱藏地鼠，使它不再繪製或接受點擊，但 callback 本身仍會繼續執行並立即 return。

遊戲只有一隻地鼠時，共用的 `next_move` 足以表示規則。若同時生成多隻地鼠，每隻地鼠都需要自己的下一次移動時間，共用變數便無法分辨狀態屬於哪一個物件。

## 適用範圍

Callback 適合固定牆壁、豆子、單一玩家或只有一份共用狀態的遊戲。若每個物件需要自己的計時器、生命值、目標或移動方向，應衍生 `GridObject`，把這些資料放入成員變數。不要為了模擬 instance 狀態而建立多組全域陣列；自訂類別能直接保持資料與行為的對應關係。
