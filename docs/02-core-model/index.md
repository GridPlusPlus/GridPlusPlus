# 理解核心模型

Grid++ 把內容分成三類：

- 網格物件使用 `ObjectHandler`，位置單位是格子，會更新、繪製及同格碰撞。
- 迷宮使用 `MazeHandler`，負責保存與查詢牆面。
- 畫面介面使用 `OverlayHandler`，位置單位是像素，不參與網格碰撞。

`Game` 保存所有實體並安排每一幀。學生拿到的 Handler 是安全的存取憑證，不是指標；複製它仍然
操作同一個實體，呼叫 `deepCopy()` 才會建立另一個實體。
```cpp
gridpp::ObjectHandler player = game.addObject("player");
player.setPosition(2, 1);

gridpp::ObjectHandler same_player = player;
same_player.move(1, 0);  // player 現在也位於 (3, 1)
```
