# 小精靈移動

教學版的小精靈按一次方向鍵走一格；真正的 Pac-Man 則會持續沿目前方向移動。玩家可以在抵達路口前先按下方向鍵，程式記住這個輸入，等那個方向不再是牆時才轉彎。這需要保存三項資料：目前方向、玩家希望前往的方向，以及下一次可以移動的時間。

這三項資料都屬於小精靈本身，因此依照第 6 章的原則存在小精靈物件上，而不是放在全域變數。四個方向和教學第 5 步一樣，以 0～3 表示，並用兩個陣列取得 x、y 位移：

```cpp title="main.cpp 節錄：方向與速度常數"
constexpr int kDX[4] = {1, 0, -1, 0};  // 方向 0 右、1 上、2 左、3 下
constexpr int kDY[4] = {0, -1, 0, 1};
constexpr double kPacmanStepTime = 0.13;  // 小精靈每 0.13 秒走一格
constexpr double kGhostStepTime = 0.2;    // 鬼每 0.2 秒走一格
```

方向 `d` 的下一格就是 `(x + kDX[d], y + kDY[d])`。例如方向 1（上）的位移是 `(0, -1)`，因為 y 向下增加，往上走時 y 要減一。兩個速度常數集中寫在檔案開頭，想調整難度時只要改這裡。

## 建立小精靈

`BuildLevel()` 在逐格掃描地圖時，只先記住 `P` 的位置；掃描完成後才建立小精靈，並存入初始狀態與種類：

```cpp title="main.cpp 節錄：BuildLevel() 最後建立小精靈"
pacman = game.addObject("pacman", nullptr, MovePacman, PacmanHit);
pacman.setPosition(pacman_x, pacman_y);
pacman.set("type", "pacman");
pacman.set("nextMove", 0.0);
pacman.set("direction", -1);
pacman.set("wantedDirection", -1);
```

方向的初始值 `-1` 表示「還沒有方向」，因此開始時小精靈會停在原地，等第一次按鍵。`nextMove` 是小數，所以初始值要寫成 `0.0`（原因見第 6.1 節）。`LevelMap` 已保證恰好有一個 `P`，因此有效地圖一定會建立 `pacman`。把小精靈留到最後建立，也讓 layer 相同時的小精靈顯示在豆子和鬼上方。建立結果存進全域的 `pacman`，讓鬼可以查詢小精靈的位置。

## 每幀讀取輸入，定時移動

`MovePacman` 是小精靈的 Update 函式：

```cpp title="main.cpp 節錄：MovePacman()"
void MovePacman(GameEngine game, GridObject self) {
    if (game_state != GameState::kPlaying || paused) return;

    // 每一幀都記下最後按的方向，等那個方向走得過去時再轉彎。
    int wanted_direction = -1;
    self.get("wantedDirection", wanted_direction);
    if (game.keyDown(KEY_RIGHT)) wanted_direction = 0;
    if (game.keyDown(KEY_UP)) wanted_direction = 1;
    if (game.keyDown(KEY_LEFT)) wanted_direction = 2;
    if (game.keyDown(KEY_DOWN)) wanted_direction = 3;
    self.set("wantedDirection", wanted_direction);

    double next_move = 0.0;
    self.get("nextMove", next_move);
    if (game.time() < next_move) return;  // 還沒輪到小精靈走
    self.set("nextMove", game.time() + kPacmanStepTime);

    int direction = -1;
    self.get("direction", direction);
    if (wanted_direction >= 0 && !maze.isWall(self.x() + kDX[wanted_direction], self.y() + kDY[wanted_direction])) {
        direction = wanted_direction;
        self.set("direction", direction);
    }
    if (direction >= 0 && !maze.isWall(self.x() + kDX[direction], self.y() + kDY[direction])) {
        self.move(kDX[direction], kDY[direction]);
        self.setDirection(direction);
    }
}
```

函式可以分成四段閱讀：

1. **是否在遊戲中**：開始畫面、勝負畫面或暫停時直接返回，小精靈停在原地。
2. **讀取輸入**：每幀都讀取方向鍵，讓短暫的按鍵也能更新 `wantedDirection`，並立刻存回物件。這裡使用 `keyDown()` 而不是教學版的 `keyPressed()`，是因為玩家通常會按住方向鍵，等待轉彎的時機。
3. **計時**：和教學第 5 步的鬼一樣，用 `game.time()` 和「下一次可以移動的時間」比較；時間還沒到就返回，到了就把下一次排在 `kPacmanStepTime` 秒後。差別只在這個時間存在小精靈身上，而不是全域變數。
4. **轉向與移動**：先嘗試採用希望的方向，若那個方向不是牆就把它設為目前方向；接著檢查目前方向能否前進，可以的話才移動。

因為 `isWall()` 對地圖外座標回傳 `true`，第 4 段的兩個檢查也一併處理了地圖邊界。以時間而不是幀數來計時，讓小精靈在不同電腦上都以相同的速度前進（見第 3.4 節）。

`setDirection(direction)` 只旋轉素材，不會移動物件。方向編號和素材旋轉規則使用相同的右、上、左、下順序，因此一張朝右的 Pac-Man 圖片可以顯示四個方向。

## 先取出、修改，再存回

`MovePacman` 反覆出現同一種模式：用 `get()` 把狀態讀進區域變數，在函式中修改區域變數，再用 `set()` 存回物件。區域變數在函式結束時就會消失，如果只修改 `wanted_direction` 而忘記 `self.set("wantedDirection", ...)`，下一幀讀到的仍是舊值，提前按下的轉彎就會被忘記。遇到「狀態好像沒有被保存」的問題時，第一步就是檢查每個修改後的值是否都已存回。

## 本節小結

- `wantedDirection` 保存尚未能執行的轉向輸入，`direction` 保存目前移動方向，`nextMove` 保存下一次可以移動的時間。
- 三項狀態都存在小精靈物件上，每幀先 `get()`、修改後再 `set()`。
- 和教學版一樣，用 `game.time()` 安排移動，並在移動前用 `maze.isWall()` 查詢目標格。
- `setDirection()` 旋轉素材，`move()` 修改網格座標。

[豆子與勝利條件](03-pellets.md){ .md-button .md-button--primary }
