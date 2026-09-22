# 遊戲狀態與 Overlay

整局共用的階段適合放在全域 enum：

```cpp
enum class GameState { kStart, kPlaying, kWon, kLost };
GameState game_state = GameState::kStart;
```

分數文字的 Update 依階段更新內容或隱藏：

```cpp
void UpdateScore(GameEngine, Overlay self) {
    if (game_state != GameState::kPlaying) {
        self.hide();
        return;
    }
    self.setText("Pellets: " + std::to_string(pellets_left));
    self.show();
}
```

按鈕由 Click 函式改變階段，再用 Update 決定是否顯示。Restart 的 Click 呼叫 `BuildLevel(game)`；
該函式先 `clearObjects()`，再加入新迷宮、豆子、鬼與玩家。舊 Handler 自動失效，新元素在幀末 Init，
下一幀開始運作。
