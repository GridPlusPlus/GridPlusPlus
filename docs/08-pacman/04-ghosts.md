# 多隻鬼的獨立狀態

教學第 5 步的鬼只有一隻，所以方向和計時可以放在全域變數。完整版有四隻鬼，使用相同的移動程式，但每隻都要記住自己的方向、下一次移動時間、顏色與策略。這正是第 6 章處理的個別狀態問題：所有鬼共用同一個 Update 函式 `MoveGhost`，每隻鬼的資料則用 `set()` 存在牠自己身上。

## 建立不同的鬼

地圖中的每個 `G` 建立一隻鬼。顏色依建立順序循環，第四隻使用隨機策略：

```cpp title="main.cpp 節錄：BuildLevel() 建立鬼的部分"
const Color ghost_colors[4] = {RED, PINK, SKYBLUE, ORANGE};
int ghost_count = 0;
// ...
for (int y = 0; y < level.rows; ++y) {
    for (int x = 0; x < level.cols; ++x) {
        const char tile = level.tiles[y][x];
        // ...（牆面、豆子與小精靈起點）
        if (tile == 'G') {
            GridObject ghost = game.addObject("ghost", nullptr, MoveGhost);
            ghost.setPosition(x, y);
            ghost.setColor(ghost_colors[ghost_count % 4]);
            ghost.set("type", "ghost");
            ghost.set("nextMove", 0.0);
            ghost.set("direction", 0);
            ghost.set("random", ghost_count == 3);
            ghost_count++;
        }
    }
}
```

`setColor()` 讓四隻鬼共用同一張白色素材，再以不同顏色繪製；教學版的紅色鬼也是這樣來的。`ghost_count == 3` 的結果是布林值，只有第四隻鬼會得到 `true`，因此 `"random"` 以 `bool` 保存；之後讀取時也必須使用 `bool` 變數。

## 計時與抓到檢查

`MoveGhost` 的開頭和教學第 5 步的 `ChasePacman` 幾乎相同，只是下一次移動時間改從自己身上讀取：

```cpp title="main.cpp 節錄：MoveGhost() 開頭"
void MoveGhost(GameEngine game, GridObject self) {
    if (game_state != GameState::kPlaying || paused || !pacman.exists()) return;

    double next_move = 0.0;
    self.get("nextMove", next_move);
    if (game.time() < next_move) return;  // 還沒輪到這隻鬼走
    self.set("nextMove", game.time() + kGhostStepTime);

    // 走之前先看一眼：小精靈是不是自己撞上來了？
    if (Caught(self)) {
        game_state = GameState::kLost;
        return;
    }

    int direction = 0;
    bool random_ghost = false;
    self.get("direction", direction);
    self.get("random", random_ghost);
    // 接下來選擇方向並移動。
}
```

`!pacman.exists()` 確認小精靈仍在遊戲中，後面才能安全地讀取它的座標。小精靈每 0.13 秒走一格，鬼每 0.2 秒走一格，所以鬼會稍慢一點；每隻鬼各自保存自己的 `"nextMove"`，不會互相影響。`Caught(self)` 和教學版完全相同，移動完成後也會再檢查一次，原因見下方〈為什麼要自己檢查是否抓到〉。

## 列出可以走的方向

鬼選路的規則和教學第 5 步一樣：不走進牆、不回頭，只有走進死路時才回頭。完整版先把所有可以走的方向收集進陣列 `choices`，這樣接下來不論是追擊還是隨機，都從同一份清單裡挑：

```cpp title="main.cpp 節錄：MoveGhost() 收集可以走的方向"
// 列出走得通、而且不用回頭的方向；走進死路時才回頭。
int back = (direction + 2) % 4;
int choices[4];
int choice_count = 0;
for (int d = 0; d < 4; ++d) {
    if (d == back) continue;
    if (maze.isWall(self.x() + kDX[d], self.y() + kDY[d])) continue;
    choices[choice_count] = d;
    choice_count++;
}
if (choice_count == 0) {
    if (maze.isWall(self.x() + kDX[back], self.y() + kDY[back])) return;  // 四面都是牆
    choices[0] = back;
    choice_count = 1;
}
```

`(direction + 2) % 4` 是反方向：右（0）的反方向是左（2），上（1）的反方向是下（3）。若四周全是牆，連回頭都走不通，鬼便保留原位。

## 追擊與隨機策略

第四隻鬼在可以走的方向中隨機選一條。其餘的鬼和教學版一樣，計算走過去之後離小精靈多遠（橫向距離加縱向距離），選最近的那條：

```cpp title="main.cpp 節錄：MoveGhost() 選擇並移動"
// 隨機的鬼亂選一條路，其他鬼選離小精靈最近的那條。
int picked = choices[0];
if (random_ghost) {
    picked = choices[game.random(0, choice_count - 1)];
} else {
    int best_distance = 9999;
    for (int i = 0; i < choice_count; ++i) {
        int x = self.x() + kDX[choices[i]];
        int y = self.y() + kDY[choices[i]];
        int distance = std::abs(x - pacman.x()) + std::abs(y - pacman.y());
        if (distance < best_distance) {
            best_distance = distance;
            picked = choices[i];
        }
    }
}

self.set("direction", picked);
self.move(kDX[picked], kDY[picked]);

// 走完再看一眼：是不是撲到小精靈身上了？
if (Caught(self)) game_state = GameState::kLost;
```

鬼只透過全域的 `pacman` Handler 讀取小精靈的座標，不會修改它。選好方向後，新的方向存回自己身上，下一次移動時就能據此避免回頭。

這個設計說明「多隻物件各有狀態」不等於必須為每一種角色寫一個 class：函式負責規則，物件自己保存的資料負責區分每隻鬼。

## 為什麼要自己檢查是否抓到

小精靈的 `PacmanHit` 已經會在和鬼同格時結束遊戲，但 Engine 只在所有物件都更新完之後，才檢查哪些物件位於同一格。若小精靈在 `(3, 1)` 往右走、鬼在 `(4, 1)` 往左走，而且兩者剛好在同一幀移動，更新結束時小精靈在 `(4, 1)`、鬼在 `(3, 1)`：兩者互換了位置，卻從未落在同一格，碰撞檢查就看不到這次相遇。教學第 5 步的「咦？鬼穿過去了？」說的就是這件事。

鬼在移動前後各比較一次座標，就能補上這個情況，而且不必在意誰先更新：

- 小精靈先更新、走進鬼的格子時，鬼移動前就會發現兩者同格。
- 鬼先更新、走進小精靈的格子時，鬼移動後就會發現兩者同格；遊戲已經結束，小精靈這一幀便不會再移動。

## 本節小結

- 所有鬼共用 `MoveGhost`，方向、下一次移動時間與策略存在各自身上。
- `isWall()` 在移動前排除牆面和地圖外座標。
- 鬼不回頭，只有走進死路時才反向移動。
- `setColor()` 讓多個物件以不同顏色共用同一素材。
- 互換位置不會觸發碰撞，所以鬼移動前後都自己檢查一次是否抓到小精靈。

[遊戲狀態與 Overlay](05-game-states.md){ .md-button .md-button--primary }
