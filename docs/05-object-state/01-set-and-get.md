# 用 set 與 get 保存個別狀態

章首頁已經確認問題來自資料歸屬，現在直接改寫地鼠。每個 GridObject 除了座標、素材與顯示狀態之外，還有一塊可以自由存放資料的空間：`set(key, value)` 以一個字串名稱（key）存入數值，`get(key, output)` 再依同一個名稱把數值讀回變數。這些資料跟著物件走，不同物件即使使用相同的 key，也互不干擾。

## 讓多隻地鼠保存自己的時間

以下的 `UpdateMole` 不再使用全域 `next_move`，而是把移動間隔 `"interval"` 與下一次移動時間 `"nextMove"` 存在每隻地鼠身上。函式開頭先把兩個值讀出來，判斷是否到了移動時間；移動後再把新的 `nextMove` 存回去。

```cpp title="main.cpp 節錄：每隻地鼠讀寫自己的計時器"
void UpdateMole(GameEngine game, GridObject self) {
    double interval = 1.0;
    double next_move = 0.0;
    self.get("interval", interval);
    self.get("nextMove", next_move);
    if (game.time() < next_move) return;

    self.setPosition(game.random(0, game.cols() - 1), game.random(0, game.rows() - 1));
    self.set("nextMove", game.time() + interval);
}
```

建立地鼠時，為每一隻存入不同的移動間隔，就能讓牠們以不同速度移動，而不必為相同行為寫出多個只因數值不同而換名字的函式：

```cpp title="main() 節錄：建立三隻速度不同的地鼠"
const double intervals[3] = {1.0, 0.8, 1.4};
for (int i = 0; i < 3; ++i) {
    GridObject mole = game.addObject("mole", nullptr, UpdateMole);
    mole.setPosition(i * 2 + 1, i * 2 + 1);
    mole.set("interval", intervals[i]);
    mole.set("nextMove", 0.0);
}
```

這項修改沒有改變 Engine 看待物件的方式，三隻地鼠仍然使用同一個 `UpdateMole`。真正改變的是資料的歸屬：`self` 每次指向目前輪到的那隻地鼠，所以 `self.set("nextMove", ...)` 只會修改那一隻的時間，不會再干擾另外兩隻。

把上面兩段放進第 3 章里程碑二的 `main.cpp`（取代原本的全域 `next_move` 與單隻地鼠）後編譯執行，三個紅色方塊應以各自的節奏換位。把某一隻的間隔改成 `0.2` 再執行，只有那一隻會明顯加快，這就證明三份計時器已經分開。

## 可以保存哪些型別

`set()` 會記住第一次存入時的型別，之後的 `set()` 與 `get()` 都必須使用同一種型別：

| 存入的值 | 讀取時使用的變數 |
| --- | --- |
| 整數，例如 `self.set("timer", 0)` | `int` 或 `long long` |
| 浮點數，例如 `self.set("speed", 1.5)` | `double` |
| 布林值，例如 `self.set("active", true)` | `bool` |
| 字串，例如 `self.set("name", "ghost")` | `std::string` |
| `std::vector<T>` | 相同型別的 `std::vector<T>` |

型別錯誤時，Grid++ 會丟出含有 key 名稱的錯誤訊息，而不是悄悄把資料轉成別的數值。最常見的錯誤是把整數與浮點數混在一起：`self.set("nextMove", 0)` 的 `0` 是整數，之後再用 `double` 讀取就會出錯。想保存的是小數時，初始值請寫成 `0.0`。

## 讀不到資料時

`get()` 會回傳一個整數，告訴我們有沒有找到資料：找到時回傳 `0` 並把數值寫入第二個參數；找不到時回傳 `1`，並把第二個參數設為該型別的預設值（數字為 0、布林值為 `false`、字串為空字串）。

```cpp
int timer = 0;
if (self.get("timer", timer) != 0) {
    // 第一次執行，物件還沒有 "timer"。
    self.set("timer", 0);
}
```

這種「0 代表成功」的慣例和許多 C 語言函式相同。多數情況下，只要在建立物件時先 `set()` 好初始值，就能直接使用 `get()` 而不必檢查回傳值；上面的地鼠便是如此，而第 4 章用 `"type"` 辨認碰撞對象時，也利用了「找不到就得到空字串」這項規則。

## 保存一串資料

需要保存多個同型別數值時，例如一條預先規劃好的巡邏路線，可以存入 `std::vector`：

```cpp
std::vector<int> path_x = {1, 2, 3, 3, 3};
self.set("pathX", path_x);

std::vector<int> saved;
self.get("pathX", saved);
```

vector 會被完整複製進物件，之後修改原本的 `path_x` 不會影響物件內的那一份；要更新時，先 `get()` 取出、修改後再 `set()` 存回去。使用 vector 時記得加上 `#include <vector>`。

## 判斷資料該放在哪裡

`set()`／`get()` 與全域變數並不互相取代，選擇標準是資料描述的對象：

| 情況 | 建議方式 |
| --- | --- |
| 整局只有一份，例如分數、遊戲階段、剩餘時間 | 全域變數 |
| 所有物件都要讀取的共享資料，例如迷宮 | 全域變數 |
| 每個物件各有一份，例如計時器、方向、生命值 | `self.set()`／`self.get()` |
| 物件的種類或角色，例如 `"player"`、`"ghost"` | `set("type", ...)` |

完整 Pac-Man 範例中的四隻鬼各自保存移動計時器、方向與策略，玩家則保存目前方向與下一個想轉向的方向，因此兩者都把這些資料存在自己身上；剩餘豆子數與遊戲階段屬於整局，則使用全域變數。同一個遊戲同時使用兩種方式是正常的，只要每個選擇都符合資料的實際歸屬。

!!! note "Overlay 也能保存資料"

    `Overlay` 擁有和 GridObject 相同的 `set()` 與 `get()`，規則完全一樣。例如一個會閃爍的提示文字，可以把自己的閃爍計時器存在身上。

## 本節小結

- `set(key, value)` 把資料存進物件，`get(key, output)` 依相同 key 讀回。
- 同一個 key 的型別固定；小數的初始值要寫成 `0.0`。
- `get()` 找到時回傳 0，找不到時回傳 1 並給出預設值。
- 屬於單一物件的資料放在物件上，屬於整局的資料使用全域變數。

[Init 與 Handler](02-init-and-handlers.md){ .md-button .md-button--primary }
