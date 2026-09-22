# 玩家移動

玩家把計時器、目前方向與希望方向放在自己的狀態區：

```cpp
player.set("timer", 0);
player.set("direction", -1);
player.set("wantedDirection", -1);
```

Update 每幀先記住按鍵方向，到了移動時機再檢查牆：

```cpp
if (game.keyDown(KEY_RIGHT)) wanted_direction = 0;
if (game.keyDown(KEY_UP)) wanted_direction = 1;
if (game.keyDown(KEY_LEFT)) wanted_direction = 2;
if (game.keyDown(KEY_DOWN)) wanted_direction = 3;

if (!maze.isWall(self.x() + dx[direction], self.y() + dy[direction])) {
    self.move(dx[direction], dy[direction]);
    self.setDirection(direction);
}
```

把希望方向先保存起來，玩家可以在到達路口前先按鍵，轉彎手感會比只在移動當幀讀鍵自然。
