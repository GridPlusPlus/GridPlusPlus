# 組裝 Pacman

打地鼠把主要規則集中在單一 `UpdateFn` 中，而 Pacman 同時需要可查詢的迷宮、持續移動的玩家、多隻各自保存方向與計時器的鬼、分布於通道上的豆子，以及協調開始、暫停、勝利和失敗的畫面，因此它適合用來檢查前面學過的 Engine、物件、Overlay、碰撞、成員狀態與素材能否真正組成一個完整遊戲。本章不是要求讀者在每一頁另寫一個彼此獨立的小程式，而是依照資料相依順序解析並組裝 repository 中同一份可執行範例；每完成一節，都應回到 `examples/pacman/main.cpp` 對照新增的責任最後被放在哪裡。

完整程式由下列四個檔案組成，其中 `main.cpp` 是各節程式片段最後共同回到的位置，而另外三個檔案分別提供讀圖工具、關卡資料與美術資源：

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

## 先看懂全局資料如何連接物件

在開始撰寫迷宮與角色之前，必須先知道後續各類別會共同依賴哪些資料，否則讀者會在玩家程式中突然遇到尚未解釋的 `g_maze`，又在鬼的程式中看到來源不明的 `g_player`。這些變數不是每個物件各自擁有的狀態，而是整個關卡只有一份的共享關係，因此範例將它們集中放在 `main.cpp` 前段：

```cpp
int g_state = 0;
int g_pellets_left = 0;
bool g_paused = false;
GridMaze* g_maze = nullptr;
GridObject* g_player = nullptr;
GridEngine* g_game = nullptr;
LevelMap g_level;
```

`g_state` 與 `g_paused` 決定角色是否應該移動，`g_pellets_left` 讓最後一顆豆子觸發勝利，`g_maze` 供玩家和鬼查詢下一格是不是牆，`g_player` 讓鬼知道追逐目標，而 `g_game` 與 `g_level` 則讓重新開始按鈕可以重建同一關。三個指標都只是指向 Engine 已擁有物件的借用關係，`BuildLevel()` 清除舊關卡時必須先重設，再在生成新迷宮與玩家後重新指定；第 8 章會進一步說明這些指標何時失效。

這個全局設計刻意保持範例集中，讓初學者能在一個檔案中追蹤完整流程；當遊戲繼續成長時，可以把共享狀態收進 `Game` 類別，但那是程式組織方式的下一步，不影響本章要學的物件分工。接下來的組裝順序也依照相依關係展開：先讀取地圖並建立 `g_maze`，再建立依賴迷宮的玩家、豆子與鬼，最後才加入讀取整局狀態的 Overlay 與按鈕。

## 完成後應看見什麼

方向鍵會改變玩家接下來想走的方向，玩家持續沿可通行方向前進，吃完所有豆子便進入勝利狀態，碰到任何鬼則進入失敗狀態，而 Start、Pause 與 Restart 按鈕分別控制遊戲開始、暫停和重新建立關卡。若其中一項結果沒有出現，應依章節順序確認地圖是否成功載入、共享指標是否在生成物件後指定、角色更新是否被狀態允許，以及碰撞是否使用正確 tag，而不是只確認程式能編譯便視為完成。
