# 遊戲狀態與 Overlay

本章首頁已先建立共享資料，現在把 `game_state` 與 `paused` 接到角色、訊息和按鈕上，使同一批遊戲物件不必在開始、勝利或失敗時反覆移除與重建。Pac-Man 有開始、遊戲中、勝利與失敗四個主要階段，遊戲中還能暫停；角色每幀讀取狀態決定是否行動，Overlay 與按鈕則把同一狀態轉換成玩家看得見、也能操作的介面。

| `game_state` | 階段 | 畫面與行為 |
|---|---|---|
| `GameState::kStart` | 開始 | 顯示標題與 Start，角色不移動 |
| `GameState::kPlaying` | 遊戲中 | 顯示剩餘豆子與 Pause，角色移動 |
| `GameState::kWon` | 勝利 | 顯示 YOU WIN! 與 Restart |
| `GameState::kLost` | 失敗 | 顯示 GAME OVER 與 Restart |

`MovePlayer` 與 `MoveGhost` 都在狀態不是 `kPlaying` 或遊戲暫停時直接返回。遊戲物件仍存在並繪製，背景保持在目前關卡，Overlay 再於其上顯示狀態訊息。

## 讓文字跟著狀態改變

範例建立兩行文字：左上角的剩餘豆子數，以及畫面中央的大標題。兩者都在建立時交出一個 Update 函式，每幀依照 `game_state` 決定要顯示什麼：

```cpp title="main.cpp 節錄：UpdateScore() 與 UpdateStatus()"
void UpdateScore(GameEngine, Overlay self) {
    if (game_state != GameState::kPlaying) {
        self.hide();
        return;
    }
    self.setText("Pellets: " + std::to_string(pellets_left));
    self.show();
}

void UpdateStatus(GameEngine, Overlay self) {
    if (game_state == GameState::kPlaying) {
        self.hide();
        return;
    }

    if (game_state == GameState::kStart)
        self.setText("PAC-MAN");
    else if (game_state == GameState::kWon)
        self.setText("YOU WIN!");
    else
        self.setText("GAME OVER");
    self.show();
}
```

Overlay 的 Update 函式形式是 `void Function(GameEngine game, Overlay self)`，和 GridObject 的版本相同，只是 `self` 的型別換成 `Overlay`。隱藏的 Overlay 仍會繼續執行 Update，所以 `UpdateStatus` 在遊戲中隱藏自己之後，仍能在勝負揭曉的那一幀把自己重新顯示出來。

這兩個函式展現了第 2 章提到的分工：`pellets_left` 與 `game_state` 由遊戲規則修改，Overlay 只負責把它們轉成文字。文字不需要知道豆子何時被吃掉，只要每幀讀取最新的值即可。

## 按鈕的 Click 與 Update

按鈕由 `addButton()` 建立，參數依序是文字、像素 x、像素 y、寬度、高度與 Click 函式。玩家用滑鼠左鍵點擊可見的按鈕時，Engine 呼叫它的 Click 函式；Click 函式的形式和 Overlay 的 Update 相同。

```cpp title="main.cpp 節錄：三個 Click 函式"
void StartGame(GameEngine, Overlay) { game_state = GameState::kPlaying; }

void RestartGame(GameEngine game, Overlay) {
    BuildLevel(game);
    game_state = GameState::kPlaying;
}

void TogglePause(GameEngine, Overlay) { paused = !paused; }
```

每個按鈕只應在適當的階段出現，因此範例在建立按鈕後，再用 `setUpdateFunction()` 交給它一個決定顯示與否的 Update 函式。隱藏的按鈕不會被畫出來，也不接受點擊：

```cpp title="main.cpp 節錄：依階段顯示按鈕"
void UpdateStartButton(GameEngine, Overlay self) {
    if (game_state == GameState::kStart)
        self.show();
    else
        self.hide();
}

void UpdateRestartButton(GameEngine, Overlay self) {
    if (game_state == GameState::kWon || game_state == GameState::kLost)
        self.show();
    else
        self.hide();
}

void UpdatePauseButton(GameEngine, Overlay self) {
    if (game_state != GameState::kPlaying) {
        self.hide();
        return;
    }
    self.setText(paused ? "Resume" : "Pause");
    self.show();
}
```

Start 與 Restart 放在畫面上同一個位置，卻不會同時出現：開始階段只有 Start 可見，勝負揭曉後只有 Restart 可見。Pause 按鈕則在暫停時把文字改成 `Resume`，提示玩家再按一次可以繼續。`paused ? "Resume" : "Pause"` 是條件運算子，意思是「`paused` 為真時使用 `"Resume"`，否則使用 `"Pause"`」。

按下 Pause 並沒有停止 Engine 的主迴圈；`MovePlayer` 與 `MoveGhost` 仍然每幀被呼叫，只是看見 `paused` 為真後立即返回。

## 重新開始一局

`RestartGame` 呼叫 `BuildLevel()` 重建關卡，再把狀態切回 `kPlaying`。`BuildLevel()` 開頭的 `clearObjects()` 會移除上一局的迷宮、豆子、鬼與玩家，接著從已驗證的 `level` 建立新的一批；Overlay 與已載入素材不受 `clearObjects()` 影響，所以文字和按鈕可以繼續使用。

清除之後，全域的 `maze` 與 `player` 會立即被重新指定為新建立的迷宮與玩家。新內容在這一幀結束時執行 Init，並從下一幀開始更新、碰撞與繪製；這項時序規則在第 8 章有完整說明。

## 檢查 main() 的組裝順序

`main()` 依資料相依順序組裝程式：讀取地圖、建立 Engine、載入素材、建立關卡、加入 Overlay，最後開始主迴圈。

```cpp title="main.cpp 節錄：main() 的正常流程"
level = LoadLevelMap("map.txt");

GameEngine game(level.cols, level.rows, 32);
game.loadAssets("pacman.db");
game.setBackgroundColor(BLACK);
BuildLevel(game);
game_state = GameState::kStart;

constexpr int kButtonWidth = 120;
constexpr int kButtonHeight = 40;
const int button_x = level.cols * 32 / 2 - kButtonWidth / 2;
const int button_y = level.rows * 32 / 2;

game.addTextOverlay("Pellets: 0", 8, 8, 20, YELLOW, nullptr, UpdateScore);
game.addTextOverlay("PAC-MAN", button_x, button_y - 70, 40, YELLOW, nullptr, UpdateStatus);

Overlay start = game.addButton("Start", button_x, button_y, kButtonWidth, kButtonHeight, StartGame);
start.setUpdateFunction(UpdateStartButton);
Overlay restart = game.addButton("Restart", button_x, button_y, kButtonWidth, kButtonHeight, RestartGame);
restart.setUpdateFunction(UpdateRestartButton);
Overlay pause = game.addButton("Pause", level.cols * 32 - 88, 6, 82, 24, TogglePause);
pause.setUpdateFunction(UpdatePauseButton);

game.run();
```

`addTextOverlay()` 的最後兩個參數是 Init 與 Update 函式，這裡只需要 Update，所以 Init 傳入 `nullptr`。按鈕的座標由視窗大小算出：`level.cols * 32` 是視窗寬度，減去按鈕寬度的一半後，按鈕就會水平置中。

這個順序也決定 Overlay 的繪製先後。兩行文字最早加入，三個按鈕在它們之後繪製；Overlay 不使用 layer，後加入者畫在較早加入者上方。

## 本節小結

- 全域遊戲狀態協調角色行動、勝負畫面與按鈕。
- Overlay 的 Update 依狀態修改文字或切換顯示；隱藏時仍會繼續 Update。
- 按鈕的 Click 函式改變狀態，Update 函式決定按鈕何時可見；隱藏的按鈕不接受點擊。
- `clearObjects()` 重建關卡時保留 Overlay 與已載入素材。

[生命週期與所有權](../08-lifecycle/index.md){ .md-button .md-button--primary }
