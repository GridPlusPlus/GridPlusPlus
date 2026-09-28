# 解析完整的 Pac-Man 範例

打地鼠讓讀者從空白檔案逐步完成遊戲，Pac-Man 的任務不同：它是一份已完成的案例，用來觀察迷宮、玩家、鬼、豆子、共享狀態與 Overlay 如何共同運作。本章依照資料相依順序導讀 [`examples/pacman/main.cpp`](https://github.com/GridPlusPlus/GridPlusPlus/blob/main/examples/pacman/main.cpp)。各頁節錄不能單獨貼上執行；標示「`main.cpp` 節錄」的程式碼區塊，都要放回這份檔案的既有位置理解。

整份程式只使用 `GameEngine`、`GridObject`、`Maze`、`Overlay`、普通函式與少量全域變數，沒有自訂 class、繼承、指標或 `new`。如果想先看一個更小的版本，[`examples/pacman_easy/`](https://github.com/GridPlusPlus/GridPlusPlus/tree/main/examples/pacman_easy) 只有一名玩家和一隻鬼，地圖直接寫在程式裡，勝負結果則印在終端機，適合在閱讀本章之前先玩一次。

先在 `examples/pacman/` 編譯並玩一次完成品，確認方向鍵、碰撞與按鈕的實際結果，再依序閱讀地圖、玩家、豆子、鬼與遊戲狀態。完整範例由下列四個檔案組成：

| 檔案 | 用途 |
|---|---|
| `main.cpp` | 遊戲規則函式、關卡建立、Overlay 與 `main()` |
| `LevelMap.h` | 讀取並驗證文字地圖的輔助工具 |
| `map.txt` | 21×19 關卡資料 |
| `pacman.db` | 玩家、鬼、豆子與牆面素材 |

在 `examples/pacman/` 中編譯並執行 WSL／Linux 版本：

```bash
g++ -std=c++17 main.cpp -I../.. -o game \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```

`-I../..` 告訴編譯器 Grid++ 標頭檔位於上兩層的專案根目錄。其他平台的連結參數見[第一個 Grid++ 程式](../01-getting-started/02-hello-gridpp.md)。程式使用相對路徑讀取 `map.txt` 與 `pacman.db`，所以必須從 `examples/pacman/` 執行。

<figure markdown="span">
  ![Pac-Man 完成畫面：藍色迷宮中包含豆子、四隻不同顏色的鬼與黃色玩家](../images/preview.png)
  <figcaption>完成品應正確載入迷宮與素材；若看見紅色替代方塊，表示素材包或執行目錄有誤。</figcaption>
</figure>

## 先看懂共享資料

後續函式會共同使用 `maze`、`player` 與遊戲階段，因此閱讀角色之前必須先知道這些名稱的來源。它們不是每個物件各自擁有的狀態，而是整個關卡只有一份的共享資料；其中遊戲階段使用具名的 `GameState`，避免以 `0`、`1`、`2`、`3` 猜測數字意義。

```cpp title="main.cpp 節錄：方向常數之後的共享狀態"
enum class GameState {
    kStart,
    kPlaying,
    kWon,
    kLost,
};

GameState game_state = GameState::kStart;
int pellets_left = 0;
bool paused = false;
Maze maze;
GridObject player;
LevelMap level;
```

`game_state` 與 `paused` 決定角色是否更新，`pellets_left` 讓最後一顆豆子觸發勝利；`maze` 供角色查詢牆面，`player` 則讓鬼取得追逐目標。`level` 保存已驗證的地圖，讓重新開始按鈕重建同一關。`maze` 與 `player` 都是 Handler，一開始不指向任何內容，要等建立關卡時才被指定；重新開始時舊的迷宮與玩家會被清除，兩個 Handler 隨即重新指向新建立的內容。

這些宣告與所有函式都被包在 `namespace { ... }` 中。這種沒有名字的命名空間讓裡面的名稱只在 `main.cpp` 中可見，避免和其他檔案衝突；閱讀時可以先把它當成一般的全域範圍。

這種全域設計讓小型案例維持在一個檔案中，代價是多個函式都依賴共享變數，因此不能被誤認為大型遊戲的推薦架構。接下來依照相依關係閱讀：先看地圖如何建立 `maze`，再看玩家、豆子與鬼如何使用共享資料，最後檢查 Overlay 與按鈕如何呈現 `GameState`。

## 完成品驗收

按下 Start 後，方向鍵應改變玩家接下來想走的方向，玩家會沿通道持續前進；吃完所有豆子進入勝利畫面，碰到鬼進入失敗畫面，而 Pause 與 Restart 分別控制暫停和重建關卡。閱讀每一節後，都應回到已編譯的完成品操作對應功能；本章的驗收依據是可觀察行為，不是節錄本身看起來合理。

[地圖與迷宮](01-map-and-maze.md){ .md-button .md-button--primary }
