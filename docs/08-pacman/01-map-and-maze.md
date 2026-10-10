# 地圖與迷宮

教學第 3 步已經用 `Maze` 蓋過牆：地圖寫成字串陣列，再用 `isWall()` 擋住小精靈。本節說明 `Maze` 本身的設計，以及完整版如何改從檔案讀取地圖。

迷宮是另一層網格資料，每一格記錄「這個位置能不能通過」。若將每一面牆都建立成獨立 GridObject，小型地圖仍可運作，但程式會產生大量沒有行為的物件，移動時還要等待碰撞發生才知道玩家已經走進牆內。

`Maze` 把整張牆面配置保存為一個固定大小的二維網格。遊戲可以在移動前查詢目標格是否為牆，先決定是否允許移動；繪製時則由同一份資料畫出所有牆面。牆面判斷與畫面因此使用一致的地圖來源。

和其他內容一樣，迷宮由 Engine 建立，程式取得的是操作它的 Handler。下列程式是最小使用示意：

```cpp title="最小使用示意"
#include "GridPlusPlus.h"

int main() {
    gridpp::GameEngine game(10, 10, 48);
    gridpp::Maze maze = game.addMaze(10, 10);
    maze.setWallImage("wall");
    maze.setWall(4, 4);
    game.run();
    return 0;
}
```

`addMaze()` 的兩個參數是迷宮的欄數與列數。迷宮從網格左上角 `(0, 0)` 開始鋪設，尺寸可以與 Engine 相同，也可以只覆蓋其中一部分；寬度與高度必須介於 1～64，無效尺寸會丟出 `std::invalid_argument`。`width()` 和 `height()` 回傳建立時的尺寸。

## 設定與查詢牆面

`setWall(x, y)` 將一格設為牆，`setWall(x, y, false)` 則恢復為通道。設定迷宮外座標是程式錯誤，因此函式會丟出 `std::out_of_range`。

```cpp title="示意：建立上下邊界"
for (int x = 0; x < maze.width(); ++x) {
    maze.setWall(x, 0);
    maze.setWall(x, maze.height() - 1);
}
```

`isWall(x, y)` 查詢一格是否為牆。查詢迷宮外座標會回傳 `true`，不會丟出例外；移動程式因此可直接檢查下一格，不必另外撰寫四個邊界條件。

```cpp title="玩家更新函式示意：移動前查詢牆面"
void MovePlayer(GameEngine game, GridObject self) {
    if (game.keyPressed(KEY_RIGHT) && !maze.isWall(self.x() + 1, self.y())) {
        self.move(1, 0);
    }
}
```

為了讓每個角色的函式都能查詢牆面，範例把 `maze` 宣告為全域 Handler。迷宮不是只佔一格的物件，也不會產生碰撞事件：玩家是否撞牆應透過 `isWall()` 在移動前判斷，而不是等到走進牆裡再由 Collide 處理。

迷宮也可以在建立時接收 Init 與 Update 函式，形式是 `void Function(GameEngine game, Maze self)`，例如讓某些牆每隔幾秒開關一次；Pac-Man 範例的牆面固定不變，所以沒有使用。

## 牆面素材模式

`setWallImage(name)` 讓所有牆使用同一素材。`setWallImages(...)` 接受孤立、端點、直線、轉角、T 形與十字六種素材，並根據上下左右連通狀態自動選擇圖案和旋轉方向；迷宮邊界之外也視為牆，所以外框會自然接起來。

```cpp title="main.cpp 節錄：BuildLevel() 設定自動拼接素材"
maze.setWallImages("wall_iso", "wall_end", "wall_straight", "wall_corner", "wall_tee", "wall_cross");
```

兩個函式同時負責切換模式，最後呼叫者生效。這使關卡可以在單一牆面素材與自動拼接素材之間切換，不需要重建迷宮。若兩者都沒有呼叫，牆面會以紅色替代方塊顯示，和找不到素材的物件相同。

## Pac-Man 地圖格式

`map.txt` 的第一行是列數和欄數，後面包含 `rows × cols` 個 tile。空白和換行都只作為分隔，因此每列可以排成地圖形狀，解析結果仍由第一行的尺寸決定。

```text
5 7
1 1 1 1 1 1 1
1 2 0 0 0 3 1
1 0 1 1 1 0 1
1 0 0 0 0 0 1
1 1 1 1 1 1 1
```

| Tile | 建立的內容 |
|---|---|
| `0` | 通道與一顆豆子 |
| `1` | 牆面 |
| `2` | 玩家起點 |
| `3` | 鬼的起點 |

`LevelMap.h` 提供 `LoadLevelMap(path)`。它會先讀取完整檔案，驗證尺寸介於 1～64、tile 數量正確、每個值位於 0～3、檔尾沒有多餘資料，並確認恰好一個玩家。驗證失敗時丟出以 `Map Error:` 開頭的英文例外。

```cpp title="main.cpp 節錄：先驗證地圖再建立 Engine"
level = LoadLevelMap("map.txt");

GameEngine game(level.cols, level.rows, 32);
```

這個呼叫順序先驗證檔案，再建立遊戲視窗。錯誤地圖只會在終端顯示訊息，不會留下無法使用的遊戲視窗。`main()` 以 `try`／`catch` 捕捉例外並回傳失敗：

```cpp title="main() 結構示意"
int main() {
    try {
        level = LoadLevelMap("map.txt");
        GameEngine game(level.cols, level.rows, 32);
        // 載入素材、建立關卡與 Overlay。
        game.run();
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
```

`try` 區塊中任何一步丟出例外，包括地圖錯誤、素材包無法開啟，或對失效 Handler 的操作，程式都會跳到 `catch` 印出訊息。這讓錯誤原因清楚地出現在終端機上，而不是讓程式直接中止。

## 從 LevelMap 建立迷宮

`BuildLevel()` 使用已驗證的全域 `level`，先清除舊內容，再建立相同尺寸的迷宮、設定自動拼接牆面素材，最後逐格填入牆與遊戲物件。以下節錄只保留和迷宮有關的部分：

```cpp title="main.cpp 節錄：BuildLevel() 建立迷宮的部分"
void BuildLevel(GameEngine game) {
    game.clearObjects();
    pellets_left = 0;
    paused = false;

    maze = game.addMaze(level.cols, level.rows);
    maze.setWallImages("wall_iso", "wall_end", "wall_straight", "wall_corner", "wall_tee", "wall_cross");

    // ...
    for (int y = 0; y < level.rows; ++y) {
        for (int x = 0; x < level.cols; ++x) {
            const int tile = level.tiles[y][x];
            if (tile == 1) {
                maze.setWall(x, y);
            } else if (tile == 0) {
                // 建立豆子（8.3 節）。
            } else if (tile == 2) {
                // 記住玩家起點（8.2 節）。
            } else if (tile == 3) {
                // 建立鬼（8.4 節）。
            }
        }
    }
    // ...
}
```

迷宮最先建立，讓 layer 同為 0 的豆子、鬼與玩家都畫在它之後。重新開始時，程式重用啟動時已驗證的 `level`，不重新讀取磁碟；`clearObjects()` 會移除上一局的迷宮與所有物件，舊的 `maze` Handler 隨之失效，接著被重新指定為新建立的迷宮。

## 本節小結

- `LevelMap` 在建立 Engine 前完整驗證 `map.txt`。
- `Maze` 保存牆面網格，角色在移動前用 `isWall()` 查詢下一格。
- `setWallImages()` 根據相鄰牆面選擇素材與方向。
- 重新開始會重用已驗證的地圖資料並重建迷宮與物件。

[玩家移動](02-player.md){ .md-button .md-button--primary }

完整函式簽名見 [Maze API](../api/classgridpp_1_1Maze.md)。
