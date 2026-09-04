# 解析完整的 Pacman 範例

打地鼠讓讀者從空白檔案逐步完成遊戲，Pacman 的任務不同：它是一份已完成的案例，用來觀察迷宮、玩家、鬼、豆子、共享狀態與 Overlay 如何共同運作。本章依照資料相依順序導讀 [`examples/pacman/main.cpp`](https://github.com/GridPlusPlus/GridPlusPlus/blob/main/examples/pacman/main.cpp)。各頁節錄不能單獨貼上執行；標示「`main.cpp` 節錄」的程式碼區塊，都要放回這份檔案的既有位置理解。

先在 `examples/pacman/` 編譯並玩一次完成品，確認方向鍵、碰撞與按鈕的實際結果，再依序閱讀地圖、玩家、豆子、鬼與遊戲狀態。完整範例由下列四個檔案組成：

| 檔案 | 用途 |
|---|---|
| `main.cpp` | 遊戲物件、關卡建立、Overlay 與 `main()` |
| `LevelMap.h` | 讀取並驗證文字地圖的輔助工具 |
| `map.txt` | 21×19 關卡資料 |
| `pacman.db` | 玩家、鬼、豆子與牆面素材 |

在 `examples/pacman/` 中編譯並執行 WSL／Linux 版本：

```bash
g++ -std=c++17 main.cpp -I../.. -o game \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```

其他平台的連結參數見[安裝 raylib](../01-getting-started/01-installation.md)。程式使用相對路徑讀取 `map.txt` 與 `pacman.db`，所以必須從 `examples/pacman/` 執行。

<figure markdown="span">
  ![Pacman 完成畫面：黑色迷宮中包含豆子、四隻不同顏色的鬼與黃色玩家](../images/preview.png)
  <figcaption>完成品應正確載入迷宮與素材；若看見紅色替代方塊，表示素材包或執行目錄有誤。</figcaption>
</figure>

## 先看懂共享資料

後續類別會共同使用 `g_maze`、`g_player` 與遊戲階段，因此閱讀角色之前必須先知道這些名稱的來源。它們不是每個物件各自擁有的狀態，而是整個關卡只有一份的共享資料；其中遊戲階段使用具名的 `GameState`，避免以 `0`、`1`、`2`、`3` 猜測數字意義。

```cpp title="main.cpp 節錄：方向常數之後的共享狀態"
enum class GameState {
    kStart,
    kPlaying,
    kWon,
    kLost,
};

GameState g_state = GameState::kStart;
int g_pellets_left = 0;
bool g_paused = false;
GridMaze* g_maze = nullptr;
GridObject* g_player = nullptr;
GridEngine* g_game = nullptr;
LevelMap g_level;
```

`g_state` 與 `g_paused` 決定角色是否更新，`g_pellets_left` 讓最後一顆豆子觸發勝利；`g_maze` 供角色查詢牆面，`g_player` 則讓鬼取得追逐目標。最後，`g_game` 與 `g_level` 讓重新開始按鈕重建同一關。三個指標都是 Engine 所擁有物件的借用指標；`BuildLevel()` 清除舊關卡時必須先重設，再指向新生成的迷宮與玩家。

這種全域設計讓小型案例維持在一個檔案中，代價是多個類別都依賴 `g_` 變數，因此不能被誤認為大型遊戲的推薦架構。接下來依照相依關係閱讀：先看地圖如何建立 `g_maze`，再看玩家、豆子與鬼如何使用共享資料，最後檢查 Overlay 與按鈕如何呈現 `GameState`。

## 完成品驗收

方向鍵應改變玩家接下來想走的方向，玩家會沿通道持續前進；吃完所有豆子進入勝利畫面，碰到鬼進入失敗畫面，而 Start、Pause 與 Restart 分別控制開始、暫停和重建關卡。閱讀每一節後，都應回到已編譯的完成品操作對應功能；本章的驗收依據是可觀察行為，不是節錄本身看起來合理。
