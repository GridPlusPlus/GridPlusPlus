# 迷宮

`GridMaze` 是核心引擎之外的**選用模組**。需要一片牆壁/迷宮時，才額外引入：

```cpp
#include "GridMaze.h"   // 會自動帶進核心的 GridPlusPlus.h
```

它本身就是一個 `GridObject`，只是覆寫了 `Render()`，會**一次把整片牆畫出來**。

## 建立迷宮

先給尺寸建立一個空迷宮，再用 `SetWall` 一格一格設定牆壁。
「牆的位置從哪來」（檔案、亂數…）交給你的程式決定，迷宮本身保持單純。

```cpp
GridMaze* maze = new GridMaze(cols, rows);   // 一開始全是空地
maze->SetWall(3, 4, true);                   // 把 (3,4) 設成牆
// ...依你的地圖把每一格設好...
maze->SetWallAsset("wall");                  // 所有牆用同一張圖
game.Spawn(maze);
```

!!! note "地圖大小上限"
    `GridMaze` 內部用固定大小的陣列存牆壁，上限為 **64 × 64** 格（在 32 像素的格子下
    已遠超螢幕放得下的範圍）。尺寸必須介於 1 × 1 和 64 × 64；非法尺寸會丟出清楚的英文例外。

    `SetWall` 的座標若超出迷宮也會丟出例外；用來判斷移動的 `IsWall` 則仍把界外視為牆。

## 牆壁碰撞

迷宮不靠引擎的「同格碰撞」，而是提供查詢方法，讓角色**移動前先問**：

```cpp
bool IsWall(int x, int y) const;   // 界外也視為牆
```

```cpp
void OnUpdate() override {
    if (IsKeyPressed(KEY_RIGHT) && !maze->IsWall(x() + 1, y()))
        Move(1, 0);
}
```

## 自動拼接牆壁外觀

只用一張牆圖時，牆看起來是一塊塊獨立方格。若想讓牆**自然連成平滑線條**
（像經典 Pac-Man 的圓角牆），用 `SetWallTiles` 提供 **6 種基本形狀**的素材，
其餘方向引擎會**旋轉素材**自動湊出：

```cpp
void SetWallTiles(iso, end, straight, corner, tee, cross);
```

| 形狀 | 連到的方向 | 畫法（畫一個朝向即可） |
|---|---|---|
| `iso` | 無 | 孤立的牆 |
| `end` | 1 邊 | 端點（畫成朝上） |
| `straight` | 對向 2 邊 | 直線（畫成上下） |
| `corner` | 相鄰 2 邊 | 轉角（畫成上右） |
| `tee` | 3 邊 | T 形（畫成上右下） |
| `cross` | 4 邊 | 十字 |

一格牆和上下左右鄰居的連通情況共有 16 種，但它們其實都是這 6 種形狀的旋轉。
所以你只要每種形狀畫**一個朝向**，引擎會依連通情況挑形狀、再用素材旋轉功能轉到對的方向：

```cpp
maze->SetWallTiles("wall_iso", "wall_end", "wall_straight",
                   "wall_corner", "wall_tee", "wall_cross");
```

範例素材包 `examples/pacman/pacman.db` 已收錄這 6 張基本形狀。

!!! tip "切換回簡單版"
    改用一行 `maze->SetWallAsset("wall");` 就回到「所有牆同一張圖」（不旋轉、不分形狀）。

## API 一覽

| 方法 | 說明 |
|---|---|
| `GridMaze(cols, rows)` | 建立指定大小的空迷宮 |
| `SetWall(x, y, wall)` | 設定某格是不是牆 |
| `SetWallAsset(name)` | 所有牆用同一張圖 |
| `SetWallTiles(iso,end,straight,corner,tee,cross)` | 6 種基本形狀，靠旋轉自動拼接 |
| `IsWall(x, y)` | 那一格是不是牆（界外當牆） |
| `width()` / `height()` | 迷宮的欄數 / 列數 |
