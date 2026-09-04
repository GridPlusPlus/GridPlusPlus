# 地圖與迷宮

迷宮是另一層網格資料，每一格記錄「這個位置能不能通過」。若將每一面牆都建立成獨立 GridObject，小型地圖仍可運作，但程式會產生大量沒有更新行為的物件，移動時還要等待碰撞發生才知道玩家已經走進牆內。

`GridMaze` 把整張牆面配置保存為一個固定大小的二維網格。遊戲可以在移動前查詢目標格是否為牆，先決定是否允許移動；繪製時則由同一份資料畫出所有牆面。牆面判斷與畫面因此使用一致的地圖來源。

GridMaze 本身繼承 GridObject，是為了進入 Engine 的繪製與所有權流程，但它代表整座迷宮，不代表其中某一格牆。它是選用模組，使用時需額外引入 `GridMaze.h`。

下列程式是最小使用示意，物件建立與方法呼叫應放在 `main()` 或其他函式內，不可直接放在全域範圍：

```cpp title="最小使用示意"
#include "GridPlusPlus.h"
#include "GridMaze.h"

int main() {
    gridpp::GridEngine game(10, 10, 48);
    auto* maze = new gridpp::GridMaze(10, 10);
    maze->SetWallAsset("wall");
    game.Spawn(maze);
    game.Run();
}
```

迷宮尺寸可以與引擎相同，也可以只覆蓋其中一部分。寬度與高度必須介於 1～64；無效尺寸會丟出 `std::invalid_argument`。`width()` 和 `height()` 回傳建立時的尺寸。

## 設定與查詢牆面

`SetWall(x, y, true)` 將一格設為牆，傳入 `false` 則恢復為通道。設定迷宮外座標是程式錯誤，因此函式會丟出 `std::out_of_range`，訊息包含座標與迷宮尺寸。

```cpp title="BuildLevel() 節錄：建立上下邊界"
for (int x = 0; x < maze->width(); ++x) {
    maze->SetWall(x, 0, true);
    maze->SetWall(x, maze->height() - 1, true);
}
```

`IsWall(x, y)` 查詢一格是否為牆。查詢迷宮外座標會回傳 `true`，不會丟出例外。移動程式因此可直接檢查下一格，不必另外撰寫四個邊界條件。

```cpp title="玩家更新函式節錄：移動前查詢牆面"
void MovePlayer(GridObject* self) {
    if (IsKeyPressed(KEY_RIGHT) && !maze->IsWall(self->x() + 1, self->y())) {
        self->Move(1, 0);
    }
}
```

`GridMaze` 本身的位置是 `(-1, -1)`，因為它不是只佔一格的碰撞物件。它覆寫 `Render()`，逐格繪製內部牆面。玩家是否撞牆應透過 `IsWall()` 判斷；迷宮物件不會為每一格牆產生 GridObject 碰撞事件。

## 牆面素材模式

`SetWallAsset(name)` 讓所有牆使用同一素材。`SetWallTiles(...)` 接受孤立、端點、直線、轉角、T 形與十字六種素材，並根據上下左右連通狀態自動選擇圖案和方向。

```cpp title="BuildLevel() 節錄：設定自動拼接素材"
maze->SetWallTiles(
    "wall_iso",
    "wall_end",
    "wall_straight",
    "wall_corner",
    "wall_tee",
    "wall_cross"
);
```

兩個函式同時負責切換模式，最後呼叫者生效。這使關卡可以在單一牆面素材與自動拼接素材之間切換，不需要重建迷宮。

## Pacman 地圖格式

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

```cpp title="main() 節錄：先驗證地圖再建立 Engine"
using pacman_example::LevelMap;
using pacman_example::LoadLevelMap;

LevelMap level = LoadLevelMap("map.txt");
GridEngine game(level.cols, level.rows, 32);
```

這個呼叫順序先驗證檔案，再建立 raylib 視窗。錯誤地圖只會在終端顯示訊息，不會留下無法使用的遊戲視窗。`main()` 捕捉例外並回傳失敗：

```cpp title="完整 main() 結構示意"
int main() {
    try {
        g_level = LoadLevelMap("map.txt");
        GridEngine game(g_level.cols, g_level.rows, 32);
        // Load assets and build the level.
        game.Run();
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
```

## 從 LevelMap 建立 GridMaze

`BuildLevel()` 收到已驗證的 `LevelMap`，建立相同尺寸的 GridMaze，設定自動拼接牆面素材，再逐格填入牆與遊戲物件。

```cpp title="main.cpp 節錄：BuildLevel() 建立迷宮的部分"
GridMaze* maze = new GridMaze(level.cols, level.rows);
maze->SetWallTiles(
    "wall_iso", "wall_end", "wall_straight",
    "wall_corner", "wall_tee", "wall_cross"
);
game.Spawn(maze);

for (int y = 0; y < level.rows; ++y) {
    for (int x = 0; x < level.cols; ++x) {
        if (level.tiles[y][x] == 1) {
            maze->SetWall(x, y, true);
        }
    }
}
```

迷宮先生成，讓相同 z-index 的玩家、豆子與鬼在它之後繪製。重新開始時，程式重用啟動時已驗證的 `g_level`，不重新讀取磁碟。`BuildLevel()` 先呼叫 `ClearObjects()`，再把舊的全域借用指標設為 `nullptr` 並建立新關卡。

## 本節小結

- `LevelMap` 在建立 Engine 前完整驗證 `map.txt`。
- GridMaze 保存牆面網格，角色在移動前用 `IsWall()` 查詢下一格。
- `SetWallTiles()` 根據相鄰牆面選擇素材與方向。
- 重新開始會重用已驗證的地圖資料並重建 GridObject。

完整函式簽名與尺寸常數見 [GridMaze API](../api/classgridpp_1_1_grid_maze.md)。
