# 玩家移動

Pac-Man 會持續沿目前方向移動。玩家可以在抵達路口前先按下方向鍵；程式記住這個輸入，等目標方向不再是牆時完成轉向。這需要保存目前方向、玩家希望前往的方向，以及控制移動速度的計時器。

這三項資料都屬於玩家本身，因此依照第 5 章的原則存在玩家物件上，而不是放在全域變數。四個方向以 0～3 表示，並使用兩個陣列取得 x、y 位移：

```cpp title="main.cpp 節錄：方向常數"
constexpr int kDirectionX[4] = {1, 0, -1, 0};
constexpr int kDirectionY[4] = {0, -1, 0, 1};
```

方向 `d` 的下一格就是 `(x + kDirectionX[d], y + kDirectionY[d])`。例如方向 1（上）的位移是 `(0, -1)`，因為 y 向下增加，往上走時 y 要減一。

## 建立玩家

`BuildLevel()` 在逐格掃描地圖時，只先記住玩家起點；掃描完成後才建立玩家，並存入三項初始狀態與種類：

```cpp title="main.cpp 節錄：BuildLevel() 最後建立玩家"
player = game.addObject("pacman", nullptr, MovePlayer, HitPlayer);
player.setPosition(player_x, player_y);
player.set("type", "pacman");
player.set("timer", 0);
player.set("direction", -1);
player.set("wantedDirection", -1);
```

方向的初始值 `-1` 表示「還沒有方向」，因此開始時玩家會停在原地，等第一次按鍵。`LevelMap` 已保證恰好有一個玩家，因此有效地圖一定會建立 `player`。把玩家留到最後建立，也讓 layer 相同時的玩家顯示在豆子和鬼上方。建立結果存進全域的 `player`，讓鬼可以查詢玩家位置。

## 每幀讀取輸入，定時移動

`MovePlayer` 是玩家的 Update 函式。它先取出三項狀態，更新希望的方向，再決定這一幀是否要移動：

```cpp title="main.cpp 節錄：MovePlayer()"
void MovePlayer(GameEngine game, GridObject self) {
    if (game_state != GameState::kPlaying || paused) return;

    int wanted_direction = -1;
    int direction = -1;
    int timer = 0;
    self.get("wantedDirection", wanted_direction);
    self.get("direction", direction);
    self.get("timer", timer);

    if (game.keyDown(KEY_RIGHT)) wanted_direction = 0;
    if (game.keyDown(KEY_UP)) wanted_direction = 1;
    if (game.keyDown(KEY_LEFT)) wanted_direction = 2;
    if (game.keyDown(KEY_DOWN)) wanted_direction = 3;
    self.set("wantedDirection", wanted_direction);

    ++timer;
    self.set("timer", timer);
    if (timer < 8) return;
    self.set("timer", 0);

    if (wanted_direction >= 0 &&
        !maze.isWall(self.x() + kDirectionX[wanted_direction], self.y() + kDirectionY[wanted_direction])) {
        direction = wanted_direction;
        self.set("direction", direction);
    }
    if (direction >= 0 && !maze.isWall(self.x() + kDirectionX[direction], self.y() + kDirectionY[direction])) {
        self.move(kDirectionX[direction], kDirectionY[direction]);
        self.setDirection(direction);
    }
}
```

函式可以分成四段閱讀：

1. **是否在遊戲中**：開始畫面、勝負畫面或暫停時直接返回，玩家停在原地。
2. **讀取輸入**：每幀都讀取方向鍵，讓短暫的按鍵也能更新 `wantedDirection`，並立刻存回物件。這裡使用 `keyDown()` 而不是 `keyPressed()`，是因為玩家通常會按住方向鍵等待轉彎時機。
3. **計時**：`timer` 每幀加一，累積到 8 幀才進入移動階段並歸零；其餘幀都在這裡返回。
4. **轉向與移動**：先嘗試採用希望的方向，若那個方向不是牆就把它設為目前方向；接著檢查目前方向能否前進，可以的話才移動。

因為 `isWall()` 對地圖外座標回傳 `true`，第 4 段的兩個檢查也一併處理了地圖邊界。這裡以幀數計時是為了讓範例保持簡單，因此實際速度會隨畫面更新率改變；如果遊戲需要在不同電腦上維持相同的每秒速度，便應像打地鼠一樣以 `game.time()` 比較下一次行動時刻。

`setDirection(direction)` 只旋轉素材，不會移動物件。方向編號和素材旋轉規則使用相同的右、上、左、下順序，因此一張朝右的 Pac-Man 圖片可以顯示四個方向。

## 先取出、修改，再存回

`MovePlayer` 反覆出現同一種模式：用 `get()` 把狀態讀進區域變數，在函式中修改區域變數，再用 `set()` 存回物件。區域變數在函式結束時就會消失，如果只修改 `timer` 而忘記 `self.set("timer", timer)`，下一幀讀到的仍是舊值，玩家便永遠不會移動。遇到「狀態好像沒有被保存」的問題時，第一步就是檢查每個修改後的值是否都已存回。

## 本節小結

- `wantedDirection` 保存尚未能執行的轉向輸入，`direction` 保存目前移動方向，`timer` 控制移動間隔。
- 三項狀態都存在玩家物件上，每幀先 `get()`、修改後再 `set()`。
- 角色在移動前使用 `maze.isWall()` 查詢目標格。
- `setDirection()` 旋轉素材，`move()` 修改網格座標。

[豆子與勝利條件](03-pellets.md){ .md-button .md-button--primary }
