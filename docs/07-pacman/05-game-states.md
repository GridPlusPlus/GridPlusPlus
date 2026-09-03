# 遊戲狀態與 Overlay

本章首頁已先建立全局資料的分工，現在可以把 `g_state` 與 `g_paused` 實際接到角色、訊息和按鈕上，使同一批遊戲物件不必在開始、勝利或失敗時反覆刪除與重建。Pacman 有開始、遊戲中、勝利與失敗四個主要階段，而遊戲中還能暫停；角色每幀讀取狀態決定是否更新，Overlay 與按鈕則把同一狀態轉換成玩家看得見、也能操作的界面。

| `g_state` | 階段 | 畫面與行為 |
|---|---|---|
| `0` | 開始 | 顯示標題與 Start，角色不移動 |
| `1` | 遊戲中 | 顯示剩餘豆子與 Pause，角色更新 |
| `2` | 勝利 | 顯示 YOU WIN 與 Restart |
| `3` | 失敗 | 顯示 GAME OVER 與 Restart |

Pacman 與 Ghost 的 `OnUpdate()` 都在狀態不是 1 或暫停時直接 return。遊戲物件仍存在並繪製，背景畫面保持在目前關卡；Overlay 再於其上顯示狀態訊息。

## 狀態畫面

`ScoreOverlay` 覆寫 `Draw()`。遊戲進行時顯示剩餘豆子；其他階段先以半透明黑色壓暗整個視窗，再繪製置中的訊息。

```cpp
class ScoreOverlay : public Overlay {
public:
    void Draw() override {
        if (g_state == 1) {
            DrawText(TextFormat("Pellets: %d", g_pellets_left),
                     8, 8, 20, YELLOW);
            if (g_paused) DrawBigText("PAUSED", ORANGE);
            return;
        }

        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                      Fade(BLACK, 0.6f));
        if (g_state == 0)
            DrawBigText("PAC-MAN", YELLOW);
        else if (g_state == 2)
            DrawBigText("YOU WIN!", GREEN);
        else if (g_state == 3)
            DrawBigText("GAME OVER", RED);
    }

private:
    static void DrawBigText(const char* message, Color color) {
        constexpr int kFontSize = 40;
        const int text_width = MeasureText(message, kFontSize);
        DrawText(message,
                 GetScreenWidth() / 2 - text_width / 2,
                 GetScreenHeight() / 2 - 70,
                 kFontSize,
                 color);
    }
};
```

Overlay 使用視窗像素座標，也能直接呼叫 raylib。它在所有 GridObject 後繪製，因此壓暗矩形會覆蓋整個關卡，後面加入的按鈕又會繪製在 `ScoreOverlay` 上方。

## 依狀態啟用按鈕

Button 的預設 `OnUpdate()` 處理 hover 與點擊，`Draw()` 繪製按鈕。衍生按鈕先檢查遊戲狀態，只在適用階段呼叫基底實作。

```cpp
class StartButton : public Button {
public:
    StartButton(int x, int y, int width, int height)
        : Button("Start", x, y, width, height) {}

    void OnClick() override { g_state = 1; }

    void OnUpdate() override {
        if (g_state == 0) Button::OnUpdate();
    }

    void Draw() override {
        if (g_state == 0) Button::Draw();
    }
};
```

RestartButton 只在狀態 2 或 3 啟用。點擊時重建所有 GridObject，保留已加入的 Overlay 和素材。

```cpp
void OnClick() override {
    BuildLevel(*g_game, g_level);
    g_state = 1;
}
```

`BuildLevel()` 呼叫 `ClearObjects()`，重設 `g_pellets_left`、`g_paused`、`g_maze` 和 `g_player`，再從已驗證的 `g_level` 建立新 instance。`ClearObjects()` 不刪除 Overlay，也不卸載 `pacman.db`，所以按鈕與素材可以跨關卡重用。

## 組裝遊戲

`main()` 依資料相依順序組裝程式：讀取地圖、建立 Engine、載入素材、建立關卡、加入 Overlay，最後開始主迴圈。

```cpp
g_level = LoadLevelMap("map.txt");

GridEngine game(g_level.cols, g_level.rows, 32);
g_game = &game;
game.LoadAssets("pacman.db");
game.set_background_color(BLACK);

BuildLevel(game, g_level);
g_state = 0;

game.AddOverlay(new ScoreOverlay());
game.AddOverlay(new StartButton(button_x, button_y,
                                kButtonWidth, kButtonHeight));
game.AddOverlay(new RestartButton(button_x, button_y,
                                  kButtonWidth, kButtonHeight));
game.AddOverlay(new PauseButton(g_level.cols * 32 - 88, 6, 82, 24));

game.Run();
```

這個順序也決定 Overlay 的繪製先後。`ScoreOverlay` 最早加入，Start、Restart 和 Pause 在它之後繪製。Overlay 不使用 GridObject 的 z-index。

## Summary

- 全局遊戲狀態協調角色更新、勝負畫面與按鈕。
- Overlay 以像素座標繪製在全部 GridObject 上方。
- Button 可依遊戲階段選擇是否呼叫基底更新與繪製。
- `ClearObjects()` 重建關卡時保留 Overlay 與已載入素材。
