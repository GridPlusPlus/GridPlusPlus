# 地圖與迷宮

地圖檔先寫列數與欄數，後方每格使用 `0` 豆子、`1` 牆、`2` 玩家、`3` 鬼。讀取並完整驗證後，
用相同尺寸建立 `GameEngine` 與迷宮：

```cpp
GameEngine game(level.cols, level.rows, 32);
Maze maze = game.addMaze(level.cols, level.rows);
maze.setWallImages(
    "wall_iso", "wall_end", "wall_straight",
    "wall_corner", "wall_tee", "wall_cross"
);
```

逐格設定牆：

```cpp
if (level.tiles[y][x] == 1) maze.setWall(x, y);
```

`isWall(x, y)` 對迷宮外座標回傳 true，所以角色移動前只需檢查目標格。`setWallImage()` 讓所有牆
使用同一素材；`setWallImages()` 則使用六種基本素材自動依鄰居旋轉與拼接。
