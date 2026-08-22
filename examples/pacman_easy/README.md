# Pac-Man 入門版

用 [Grid++](../../docs/index.md) 寫成的 Pac-Man「入門版」，示範
[`GridObject.h`](../../GridObject.h) 的 `CallbackGridObject`。

**刻意寫得像 C**：整支程式沒有自訂 `class`、沒有繼承，只用「全域變數 + 一般函式」。
角色的行為靠 `CallbackGridObject`——先寫好普通函式，初始化時把函式名字傳進去即可。
概念說明見 [CallbackGridObject](../../docs/04-object-behavior/01-callbacks.md)；
想看用繼承寫的完整版，見隔壁的 [`examples/pacman/`](../pacman/)。

> 玩法：**方向鍵**移動，吃光所有豆子獲勝，碰到鬼魂失敗。（結束後畫面會定格，重玩就重跑程式）

## 資料夾內容

| 檔案 | 說明 |
|---|---|
| `main.cpp` | 範例原始碼（地圖直接寫死在檔案裡，不讀外部檔案）。 |
| `pacman.db` | 素材包，沿用 `examples/pacman` 的那份。 |

## 編譯與執行

raylib 的安裝方式見[安裝 raylib](../../docs/01-getting-started/01-installation.md)。Windows 建議使用 WSL，並依照 Linux 指令操作；
需要原生 `.exe` 時才使用 MinGW-w64。`-I../..` 讓編譯器找到專案根目錄的 headers。

=== "WSL / Linux"

    ```bash
    g++ -std=c++17 main.cpp -I../.. -o game \
        -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    ./game
    ```

=== "Windows / MinGW-w64"

    ```bash
    g++ -std=c++17 main.cpp -I../.. -o game.exe \
        -I../../raylib/include -L../../raylib/lib -lraylib -lgdi32 -lwinmm
    ./game.exe
    ```

=== "macOS"

    ```bash
    g++ -std=c++17 main.cpp -I../.. -o game $(pkg-config --cflags --libs raylib)
    ./game
    ```

!!! note "執行前確認"
    程式以相對路徑讀取 `pacman.db`，所以要在 `examples/pacman_easy/` 資料夾內執行。

## 程式怎麼運作

**地圖**是一個寫死的二維陣列 `kMap[kHeight][kWidth]`（`1`=牆 `0`=豆子 `2`=玩家 `3`=鬼魂），
`main()` 用兩層 for 迴圈逐格擺好牆、豆子、玩家、鬼。迷宮用最簡單的畫法：
`GridMaze` + `SetWallAsset("wall_cross")`，所有牆都用同一張實心方塊。

**角色的行為都是普通函式**，在 `new CallbackGridObject(...)` 時傳進去：

- `MovePlayer(self)`：方向鍵按一下走一格，用 `IsWall()` 檢查前面不是牆才走。
- `MoveGhost(self)`：每 15 幀走一格（比玩家慢），往玩家的方向靠近、會避開牆。
- `EatPellet(self, other)`：被玩家吃到就 `set_visible(false)`，剩餘 -1。
- `HitPlayer(self, other)`：撞到 tag 為 `ghost` 的東西就失敗。

**狀態全部放全域變數**：`g_state`（遊戲中/贏/輸）、`g_pellets`（剩幾顆豆子）、
`g_ghost_timer`（鬼的計時器）、`g_player`（讓鬼知道玩家在哪）、`g_maze`（查牆用）。
因為**只有一個玩家、一隻鬼**，這些狀態放全域就夠——這正是 `CallbackGridObject` 能用的前提。

!!! tip "兩個關鍵小地方"
    - **`self` 參數**：普通函式沒有 `this`，所以引擎把物件本身當 `self` 傳進來，
      要操作它就寫 `self->Move(...)`。
    - **豆子不用記「吃了沒」**：直接隱藏被吃的豆子，就不需要每顆豆子各自的旗標——
      剛好繞過 `CallbackGridObject`「沒有每實例狀態」的限制。

## 接下來

想讓「多隻鬼各自記住自己的計時器與方向」時，全域變數就不夠用了——那就是需要**類別**的時機。
把這些函式搬進繼承 `GridObject` 的子類別（用成員變數記狀態），就成了
[`examples/pacman/`](../pacman/) 的完整版。物件狀態與繼承的說明見[自訂 GridObject](../../docs/05-object-state/01-custom-grid-object.md)。
