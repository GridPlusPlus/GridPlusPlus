# 解析完整的 Pac-Man 範例

第 2 章從零做出的 Pac-Man 刻意保持精簡。本章換個角度，閱讀一份功能更完整的版本，看看同樣的想法如何擴充成一個有開始畫面、暫停和多隻鬼的遊戲：

| | 第 2 章教學版 | 本章完整版 |
|---|---|---|
| 地圖 | 字串陣列寫在程式裡 | 從 `map.txt` 讀取，並檢查格式；符號同樣是 `#`、`.`、`P`、`G` |
| 小精靈 | 按一次走一格 | 持續前進，並記住下一個想轉的方向 |
| 鬼 | 一隻，方向和計時放在全域變數 | 四隻，各自用 `set()`／`get()` 保存方向和計時，其中一隻隨機移動 |
| 辨認碰到誰 | `other.image()` | 每個物件用 `set("type", ...)` 記下種類 |
| 遊戲階段 | `game_over` 一個布林值 | 開始、遊戲中、勝利、失敗四個階段，外加暫停 |
| 介面 | 分數與結束文字 | 加上 Start、Pause、Restart 按鈕 |

其餘寫法都和教學版相同：用 `game.time()` 安排下一次移動、方向編號 0 右 1 上 2 左 3 下、鬼「選最近、不回頭」，以及移動前後各檢查一次是否抓到小精靈，讀到時可以對照。

本章依照資料相依順序導讀 [`examples/pacman/main.cpp`](https://github.com/GridPlusPlus/GridPlusPlus/blob/main/examples/pacman/main.cpp)。各頁節錄不能單獨貼上執行；標示「`main.cpp` 節錄」的程式碼區塊，都要放回這份檔案的既有位置理解。

整份程式只使用 `GameEngine`、`GridObject`、`Maze`、`Overlay`、普通函式與少量全域變數，沒有自訂 class、繼承、指標或 `new`。

完整版放在 Grid++ 主專案的 `examples/` 資料夾裡，不在你的 Pac-Man 專案中。先把主專案下載到電腦（不需要帳號）：

```bash
cd ~
git clone https://github.com/GridPlusPlus/GridPlusPlus.git
cd GridPlusPlus/examples/pacman
```

先在 `examples/pacman/` 編譯並玩一次完成品，確認方向鍵、碰撞與按鈕的實際結果，再依序閱讀地圖、小精靈、豆子、鬼與遊戲狀態。完整範例由下列四個檔案組成：

| 檔案 | 用途 |
|---|---|
| `main.cpp` | 遊戲規則函式、關卡建立、Overlay 與 `main()` |
| `LevelMap.h` | 讀取並驗證文字地圖的輔助工具 |
| `map.txt` | 19 欄 × 21 列的關卡地圖 |
| `pacman.db` | 小精靈、鬼、豆子與牆面素材，和教學版相同 |

在 `examples/pacman/` 中編譯並執行 WSL／Linux 版本：

```bash
g++ -std=c++17 main.cpp -I../.. -o game \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```

`-I../..` 告訴編譯器 Grid++ 標頭檔位於上兩層的專案根目錄。其他平台的連結參數見[執行第一個程式](../01-getting-started/04-first-run.md)。程式使用相對路徑讀取 `map.txt` 與 `pacman.db`，所以必須從 `examples/pacman/` 執行。

<figure markdown="span">
  ![Pac-Man 完成畫面：藍色迷宮中包含豆子、四隻不同顏色的鬼與黃色小精靈](../images/preview.png)
  <figcaption>完成品應正確載入迷宮與素材；若看見紅色替代方塊，表示素材包或執行目錄有誤。</figcaption>
</figure>

## 先看懂共享資料

後續函式會共同使用 `maze`、`pacman` 與遊戲階段，因此閱讀角色之前必須先知道這些名稱的來源。它們不是每個物件各自擁有的狀態，而是整個關卡只有一份的共享資料；其中遊戲階段使用具名的 `GameState`，避免以 `0`、`1`、`2`、`3` 猜測數字意義。

```cpp title="main.cpp 節錄：方向與速度常數之後的共享狀態"
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
GridObject pacman;
LevelMap level;
```

`game_state` 與 `paused` 決定角色是否更新，`pellets_left` 讓最後一顆豆子觸發勝利；`maze` 供角色查詢牆面，`pacman` 則讓鬼取得追逐目標。`level` 保存已驗證的地圖，讓重新開始按鈕重建同一關。`maze` 與 `pacman` 都是 Handler，一開始不指向任何內容，要等建立關卡時才被指定；重新開始時舊的迷宮與小精靈會被清除，兩個 Handler 隨即重新指向新建立的內容。

這些宣告與所有函式都被包在 `namespace { ... }` 中。這種沒有名字的命名空間讓裡面的名稱只在 `main.cpp` 中可見，避免和其他檔案衝突；閱讀時可以先把它當成一般的全域範圍。

這種全域設計讓小型案例維持在一個檔案中，代價是多個函式都依賴共享變數，因此不能被誤認為大型遊戲的推薦架構。接下來依照相依關係閱讀：先看地圖如何建立 `maze`，再看小精靈、豆子與鬼如何使用共享資料，最後檢查 Overlay 與按鈕如何呈現 `GameState`。

## 完成品驗收

按下 Start 後，方向鍵應改變小精靈接下來想走的方向，小精靈會沿通道持續前進；吃完所有豆子進入勝利畫面，碰到鬼進入失敗畫面，而 Pause 與 Restart 分別控制暫停和重建關卡。閱讀每一節後，都應回到已編譯的完成品操作對應功能；本章的驗收依據是可觀察行為，不是節錄本身看起來合理。

[地圖與迷宮](01-map-and-maze.md){ .md-button .md-button--primary }
