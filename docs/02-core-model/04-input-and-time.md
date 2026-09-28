# 輸入、時間與亂數

遊戲規則透過 `GameEngine` 讀取常用輸入：

```cpp
if (game.keyPressed(KEY_SPACE)) { /* 只在按下當幀成立 */ }
if (game.keyDown(KEY_RIGHT)) { /* 按住期間持續成立 */ }

if (game.mousePressed(MOUSE_BUTTON_LEFT)) {
    int pixel_x = game.mouseX();
    int pixel_y = game.mouseY();
}
```

`game.time()` 回傳程式開始後的秒數，適合控制冷卻時間；`game.random(min, max)` 回傳包含兩端的
隨機整數。需要把滑鼠像素轉為格子時，用 `game.mouseX() / game.gridSize()`。

按鍵與顏色常數仍由 raylib 提供，例如 `KEY_RIGHT`、`MOUSE_BUTTON_LEFT`、`RED` 與 `BLACK`；
學生不需要直接呼叫 raylib 的輸入函式。
