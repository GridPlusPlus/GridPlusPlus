# 用 Init、Update 與 Collide 描述行為

第 4 章的打地鼠程式把 `UpdateMole` 交給 `addObject()` 之後，便能在沒有自行撰寫主迴圈的情況下持續處理滑鼠輸入與地鼠移動，原因是 Engine 已經負責反覆更新畫面，並且會在每一幀的固定階段呼叫這個函式。從遊戲作者的角度來看，現在最重要的不是先記住這種機制的術語，而是理解三個清楚的角色：

| 函式 | 何時被呼叫 | 常見用途 |
| --- | --- | --- |
| Init | 物件開始運作前一次 | 設定依賴遊戲時間的初始值、重設一局 |
| Update | 每一幀一次 | 讀取輸入、計時、移動 |
| Collide | 和另一個物件位於同一格時 | 收集道具、受傷、勝負判斷 |

## 函式必須長成什麼樣子

Engine 呼叫這些函式時，會傳入固定的參數，因此函式的參數型別與順序必須符合 Engine 的規定。Init 與 Update 使用相同形式，Collide 則多一個參數：

```cpp
// Init 與 Update
void FunctionName(gridpp::GameEngine game, gridpp::GridObject self);

// Collide
void FunctionName(gridpp::GameEngine game, gridpp::GridObject self, gridpp::GridObject other);
```

`game` 代表物件所在的遊戲，可以用來查詢世界大小、讀取輸入或建立新內容；`self` 是這次輪到的物件；`other` 則是碰撞時和它同格的另一個物件。函式名稱與參數名稱都可以自由決定，但回傳型別必須是 `void`，參數的型別與順序也不能改變；如果不相符，編譯器會在 `addObject()` 那一行報錯。

如果函式用不到某個參數，可以只寫型別、省略名稱。例如豆子的碰撞函式不需要查詢遊戲世界，就能寫成：

```cpp
void EatPellet(gridpp::GameEngine, gridpp::GridObject self, gridpp::GridObject other) {
    // 函式內不會用到 game，所以省略它的名稱。
}
```

## 讓物件每一幀更新

一個 Update 函式會收到 `self`，因此同一個函式不必把物件寫死在全域變數中，也能查詢它的位置，或透過 `game` 取得所在的遊戲世界。以下程式讓玩家用方向鍵一次移動一格，並用 `cols()` 與 `rows()` 防止它走出網格；函式寫在 `main()` 前面，`addObject()` 則放在 `main()` 中建立 Engine 之後、呼叫 `run()` 之前。

```cpp title="main.cpp（省略第 1 章已出現的 include 與 using）"
void MovePlayer(GameEngine game, GridObject self) {
    if (game.keyPressed(KEY_RIGHT) && self.x() < game.cols() - 1) {
        self.move(1, 0);
    } else if (game.keyPressed(KEY_LEFT) && self.x() > 0) {
        self.move(-1, 0);
    } else if (game.keyPressed(KEY_DOWN) && self.y() < game.rows() - 1) {
        self.move(0, 1);
    } else if (game.keyPressed(KEY_UP) && self.y() > 0) {
        self.move(0, -1);
    }
}

// 放在 main() 內：
GridObject player = game.addObject("player", nullptr, MovePlayer);
player.setPosition(1, 1);
```

`self` 與 `game` 都是 Handler，物件與遊戲本身仍由 Engine 管理。物件需要永久離開世界時，呼叫 `self.remove()` 即可；移除後就不應再透過 `self` 操作它，詳細的安全時機集中在第 9 章說明。

隱藏的物件也會繼續執行 Update。打地鼠正是利用這一點：地鼠在倒數結束後被隱藏，Update 卻仍能等待玩家按下 R。

## 用 Init 準備開始

Update 每幀都會執行，不適合放「只做一次」的準備工作；而 `main()` 在呼叫 `run()` 之前還沒有進入遊戲，某些值無法在那時決定，例如「這一局從什麼時刻開始」。Init 填補了這兩者之間的空隙：

```cpp title="main.cpp 節錄：Init 設定依賴時間的初始值"
double end_time = 0.0;

void InitTimer(GameEngine game, GridObject self) {
    end_time = game.time() + 30.0;
}

// 放在 main() 內：
game.addObject("timer", InitTimer, UpdateTimer);
```

呼叫 `game.run()` 時，Engine 會先為所有已建立的物件依建立順序各執行一次 Init，全部完成後才開始第一幀。遊戲進行中才建立的物件，則會在該幀結束時執行 Init，並從下一幀開始 Update。無論是哪一種，每個物件的 Init 都只會執行一次。

## 看懂 addObject 的三個行為參數

`addObject()` 在素材名稱之後依序接收 Init、Update 和 Collide 三個函式。沒有某項行為時傳入 `nullptr`；排在最後、不需要的參數則可以直接省略。下列第一行只用來標示參數順序，不是一段可以直接編譯的程式。

```cpp title="addObject 參數順序"
game.addObject(image, init, update, collide);
```

下一節才會定義 `CollectPellet` 並完成玩家吃豆子的程式；目前先比較三種常見組合：牆面沒有任何行為，玩家只需要 `MovePlayer`，豆子則不更新位置，只在碰撞時執行 `CollectPellet`。

```cpp title="main() 內的建立方式（CollectPellet 將於下一節定義）"
GridObject wall = game.addObject("wall");
GridObject player = game.addObject("player", nullptr, MovePlayer);
GridObject pellet = game.addObject("pellet", nullptr, nullptr, CollectPellet);
```

## 在建立之後替換行為

函式也可以在建立物件之後才交給它，或在遊戲進行中換成另一個函式。三個 setter 分別對應三種行為：

```cpp
player.setInitFunction(InitPlayer);
player.setUpdateFunction(MovePlayer);
player.setCollideFunction(HitPlayer);
```

例如玩家吃到加速道具後，可以呼叫 `self.setUpdateFunction(MovePlayerFast)`，下一次 Update 就會改用新函式；傳入 `nullptr` 則表示停止這項行為。需要注意的是，若物件已經執行過 Init，替換 Init 並不會讓新函式再執行一次。

??? info "選讀：回呼函式背後的函式指標"

    把函式名稱交給 `addObject()` 之所以可行，是因為 C++ 的**函式指標**：函式編譯後也位於記憶體中，而函式指標保存函式的位址，使程式可以先記住要執行哪個函式，等更新或碰撞發生時再透過該位址呼叫它。

    Grid++ 的 Update 參數型別展開後可以寫成：

    ```cpp
    void (*update)(gridpp::GameEngine, gridpp::GridObject);
    void (*collide)(gridpp::GameEngine, gridpp::GridObject, gridpp::GridObject);
    ```

    括號內的 `*update` 和 `*collide` 表示它們是指標，後面的括號則列出目標函式必須接受的參數。

    ```cpp
    void (*update)(gridpp::GameEngine, gridpp::GridObject) = UpdateMole;
    update(game, mole);  // 等同於 UpdateMole(game, mole);
    ```

    指派時可以直接寫函式名稱 `UpdateMole`；C++ 會在這個位置取得函式位址，因此通常不需要寫成 `&UpdateMole`。被傳入的函式必須使用相同的回傳型別與參數型別，編譯器會拒絕不相容的函式。

    這段語法是理解底層機制的補充，而不是使用 Grid++ 時必須書寫的形式；實際程式直接把符合參數規格的函式名稱交給 `addObject()` 或 `set...Function()` 即可。

## 本節小結

- Init 在物件開始運作前執行一次，Update 每幀執行，Collide 在同格時執行。
- 三種函式都收到 `game` 與 `self`；Collide 另外收到 `other`。
- 建立時依序傳入 Init、Update、Collide，`nullptr` 表示沒有這項行為。
- `set...Function()` 可以在建立之後替換行為。
- 函式式行為適合只需使用 GridObject 內建資料或整局共用狀態的物件。

[同格互動](02-collisions.md){ .md-button .md-button--primary }
