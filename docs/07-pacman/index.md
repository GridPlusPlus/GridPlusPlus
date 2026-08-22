# 製作 Pacman

打地鼠以一個 callback 物件完成主要規則。Pacman 需要一座可查詢的迷宮、一名持續移動的玩家、多隻各自保存方向的鬼、數百顆豆子，以及開始、暫停、勝利和失敗畫面。這些需求會組合前面章節的 GridEngine、GridObject、Overlay、碰撞、instance 狀態與素材繪製。

本章使用 repository 中的 `examples/pacman/`。完整程式由 `main.cpp`、`LevelMap.h`、`map.txt` 和 `pacman.db` 組成；每一節抽出其中一個可辨識的功能，最終結果始終對應同一份 `main.cpp`。

| 檔案 | 用途 |
|---|---|
| `main.cpp` | 遊戲物件、關卡建立、Overlay 與 `main()` |
| `LevelMap.h` | 讀取並驗證文字地圖的提供工具 |
| `map.txt` | 21×19 關卡資料 |
| `pacman.db` | 玩家、鬼、豆子與牆面素材 |

在 `examples/pacman/` 中編譯並執行 WSL／Linux 版本：

```bash
g++ -std=c++17 main.cpp -I../.. -o game \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```

WSL、Linux 與 MinGW-w64 的連結參數見[安裝 raylib](../01-getting-started/01-installation.md)。程式使用相對路徑讀取 `map.txt` 與 `pacman.db`，因此應從 `examples/pacman/` 執行。

遊戲規則如下：方向鍵改變玩家方向；玩家持續沿目前方向移動，吃完所有豆子獲勝，碰到任何鬼時失敗。Start、Pause 和 Restart 按鈕控制遊戲狀態。
