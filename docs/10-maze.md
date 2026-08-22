# 迷宮與 GridMaze

迷宮是另一層網格資料，每一格記錄「這個位置能不能通過」。若將每一面牆都建立成獨立 GridObject，小型地圖仍可運作，但程式會產生大量沒有更新行為的物件，移動時還要等待碰撞發生才知道玩家已經走進牆內。

`GridMaze` 把整張牆面配置保存為一個固定大小的二維網格。遊戲可以在移動前查詢目標格是否為牆，先決定是否允許移動；繪製時則由同一份資料畫出所有牆面。牆面判斷與畫面因此使用一致的地圖來源。

GridMaze 本身繼承 GridObject，是為了進入 Engine 的繪製與所有權流程，但它代表整座迷宮，不代表其中某一格牆。它是選用模組，使用時需額外引入 `GridMaze.h`。

```cpp
#include "GridPlusPlus.h"
#include "GridMaze.h"

gridpp::GridEngine game(10, 10, 48);
auto* maze = new gridpp::GridMaze(10, 10);
maze->SetWallAsset("wall");
game.Spawn(maze);
```

迷宮尺寸可以與引擎相同，也可以只覆蓋其中一部分。寬度與高度必須介於 1～64；無效尺寸會丟出 `std::invalid_argument`。`width()` 和 `height()` 回傳建立時的尺寸。

## 設定與查詢牆面

`SetWall(x, y, true)` 將一格設為牆，傳入 `false` 則恢復為通道。設定迷宮外座標是程式錯誤，因此函式會丟出 `std::out_of_range`，訊息包含座標與迷宮尺寸。

```cpp
for (int x = 0; x < maze->width(); ++x) {
    maze->SetWall(x, 0, true);
    maze->SetWall(x, maze->height() - 1, true);
}
```

`IsWall(x, y)` 查詢一格是否為牆。查詢迷宮外座標會回傳 `true`，不會丟出例外。移動程式因此可直接檢查下一格，不必另外撰寫四個邊界條件。

```cpp
void MovePlayer(GridObject* self) {
    if (IsKeyPressed(KEY_RIGHT) && !maze->IsWall(self->x() + 1, self->y())) {
        self->Move(1, 0);
    }
}
```

`GridMaze` 本身的位置是 `(-1, -1)`，因為它不是只佔一格的碰撞物件。它覆寫 `Render()`，逐格繪製內部牆面。玩家是否撞牆應透過 `IsWall()` 判斷；迷宮物件不會為每一格牆產生 GridObject 碰撞 callback。

## 牆面素材模式

`SetWallAsset(name)` 讓所有牆使用同一素材。`SetWallTiles(...)` 接受孤立、端點、直線、轉角、T 形與十字六種素材，並根據上下左右連通狀態自動選擇圖案和方向。

```cpp
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

## 載入文字地圖

GridMaze 不直接解析檔案。遊戲應先把整份地圖讀入並驗證，再建立或清除目前關卡。驗證至少應包含檔案是否存在、尺寸是否在 1～64、格子數是否正確、tile 值是否合法，以及必要角色的數量。

只有在驗證成功後才呼叫 `ClearObjects()` 和 `SetWall()`。這個順序可避免讀到半份或錯誤地圖時，先清除仍可使用的舊關卡。Pacman 範例的 `LevelMap` 解析器展示了完整做法。
