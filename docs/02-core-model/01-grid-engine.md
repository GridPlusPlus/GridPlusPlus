# GameEngine

`GameEngine` 是程式的單一入口。建構子參數依序為欄數、列數與每格像素大小：

```cpp
gridpp::GameEngine game(8, 8, 64);
game.setBackgroundColor(BEIGE);
game.showGrid(true);
```

常用建立函式：

```cpp
auto player = game.addObject("player");
auto score = game.addTextOverlay("Score: 0", 12, 12, 24);
auto maze = game.addMaze(8, 8);
```

所有建立函式都由 `GameEngine` 管理實體，並回傳 Handler。`clearObjects()` 清除一般物件、圖形與迷宮；
`clearOverlays()` 清除圖片、文字與按鈕。最後呼叫 `game.run()` 啟動遊戲。

`GameEngine` 可以按值傳入 callback；複製後仍操作同一個遊戲，因此 callback 不需要參考或指標語法。
