# 多隻鬼的獨立狀態

教學第 5 步的鬼只有一隻，所以方向和計時器可以放在全域變數。完整版有四隻鬼，使用相同的移動程式，但每隻都要記住自己的方向、移動計時器、顏色與策略。這正是第 6 章處理的個別狀態問題：所有鬼共用同一個 Update 函式 `MoveGhost`，每隻鬼的資料則用 `set()` 存在牠自己身上。

## 建立不同的鬼

地圖中的每個 `3` tile 建立一隻鬼。顏色依建立順序循環，第四隻使用隨機策略：

```cpp title="main.cpp 節錄：BuildLevel() 建立鬼的部分"
const Color ghost_colors[4] = {RED, PINK, SKYBLUE, ORANGE};
int ghost_count = 0;
// ...
for (int y = 0; y < level.rows; ++y) {
    for (int x = 0; x < level.cols; ++x) {
        const int tile = level.tiles[y][x];
        // ...（牆面、豆子與玩家起點）
        if (tile == 3) {
            GridObject ghost = game.addObject("ghost", nullptr, MoveGhost);
            ghost.setPosition(x, y);
            ghost.setColor(ghost_colors[ghost_count % 4]);
            ghost.set("type", "ghost");
            ghost.set("timer", 0);
            ghost.set("direction", 0);
            ghost.set("random", ghost_count == 3);
            ++ghost_count;
        }
    }
}
```

`setColor()` 讓四隻鬼共用同一張白色素材，再以不同顏色繪製。`ghost_count == 3` 的結果是布林值，只有第四隻鬼會得到 `true`，因此 `"random"` 以 `bool` 保存；之後讀取時也必須使用 `bool` 變數。

## 計時與前置檢查

`MoveGhost` 開頭先確認遊戲正在進行，並處理自己的計時器：

```cpp title="main.cpp 節錄：MoveGhost() 開頭"
void MoveGhost(GameEngine game, GridObject self) {
    if (game_state != GameState::kPlaying || paused || !player.exists()) return;

    int timer = 0;
    self.get("timer", timer);
    ++timer;
    self.set("timer", timer);
    if (timer < 12) return;
    self.set("timer", 0);

    // 鬼和玩家同一幀互換位置時，碰撞檢查看不到兩者同格，所以移動前後各檢查一次。
    if (Caught(self)) {
        game_state = GameState::kLost;
        return;
    }

    int current_direction = 0;
    bool random_ghost = false;
    self.get("direction", current_direction);
    self.get("random", random_ghost);
    // 接下來選擇方向並移動。
}
```

`!player.exists()` 確認玩家仍在遊戲中，後面才能安全地讀取玩家座標。`Caught(self)` 比較鬼與玩家的座標，同格就判定玩家輸了；移動完成後也會再檢查一次，原因見下方〈為什麼要自己檢查是否抓到〉。玩家每 8 幀移動，鬼每 12 幀移動，因此在相同更新率下鬼會稍慢；每隻鬼各自累加自己的 `"timer"`，也不會重設其他鬼的計時器。和玩家一樣，這是依賴幀率的簡化計時。

## 收集合法方向

鬼的移動選擇可以分成三步理解：第一步列出所有不是牆的相鄰方向，第二步在仍有其他選擇時排除立刻回頭的方向，第三步才從剩餘候選中隨機挑選，或比較哪一格更接近玩家。下列程式完成前兩步，其中 `any` 保存所有非牆方向，`options` 再排除目前方向的反向；只有走到死路、`options` 為空時，才退回 `any` 允許鬼轉身。

```cpp title="main.cpp 節錄：MoveGhost() 收集候選方向"
int options[4];
int option_count = 0;
int any[4];
int any_count = 0;
for (int direction = 0; direction < 4; ++direction) {
    if (maze.isWall(self.x() + kDirectionX[direction], self.y() + kDirectionY[direction])) continue;
    any[any_count++] = direction;
    if (direction != (current_direction ^ 2)) options[option_count++] = direction;
}

int* choices = option_count > 0 ? options : any;
const int choice_count = option_count > 0 ? option_count : any_count;
if (choice_count == 0) return;
```

方向編號 0、1、2、3 對應右、上、左、下，二進位表示分別是 `00`、`01`、`10`、`11`。和 `2`（二進位 `10`）做 XOR 會得到 `0 ↔ 2`、`1 ↔ 3`，所以 `current_direction ^ 2` 正好取得反向方向；若四周全是牆，`choice_count` 為 0，鬼便保留原位。

`int* choices` 是這份範例中唯一出現的指標，它只是讓 `choices` 代表 `options` 或 `any` 其中一個陣列，後面就能用同一段程式讀取；如果還不熟悉指標，可以把它讀成「選出的那一個陣列」。

## 隨機與追蹤策略

第四隻鬼在合法方向中隨機選擇。其餘鬼計算每個候選格到玩家的曼哈頓距離（橫向距離加縱向距離），選擇距離最小的方向：

```cpp title="main.cpp 節錄：MoveGhost() 選擇並移動"
int picked = choices[0];
if (random_ghost) {
    picked = choices[game.random(0, choice_count - 1)];
} else {
    int best_distance = 1 << 30;
    for (int i = 0; i < choice_count; ++i) {
        const int direction = choices[i];
        const int distance = ManhattanDistance(self.x() + kDirectionX[direction], self.y() + kDirectionY[direction],
                                               player.x(), player.y());
        if (distance < best_distance) {
            best_distance = distance;
            picked = direction;
        }
    }
}

self.set("direction", picked);
self.move(kDirectionX[picked], kDirectionY[picked]);
if (Caught(self)) game_state = GameState::kLost;
```

`1 << 30` 是一個很大的整數，當作「目前最短距離」的起始值，讓第一個候選一定會比它小。鬼只透過全域的 `player` Handler 讀取玩家座標，不會修改玩家；選好方向後，新的方向存回自己身上，下一次移動時就能據此避免回頭。

這個設計說明「多隻物件各有狀態」不等於必須為每一種角色寫一個 class：函式負責規則，物件自己保存的資料負責區分每隻鬼。

## 為什麼要自己檢查是否抓到

玩家的 `HitPlayer` 已經會在和鬼同格時結束遊戲，但 Engine 只在所有物件都更新完之後，才檢查哪些物件位於同一格。若玩家在 `(3, 1)` 往右走、鬼在 `(4, 1)` 往左走，而且兩者剛好在同一幀移動，更新結束時玩家在 `(4, 1)`、鬼在 `(3, 1)`：兩者互換了位置，卻從未落在同一格，碰撞檢查就看不到這次相遇。

鬼在移動前後各比較一次座標，就能補上這個情況，而且不必在意誰先更新：

- 玩家先更新、走進鬼的格子時，鬼移動前就會發現兩者同格。
- 鬼先更新、走進玩家的格子時，鬼移動後就會發現兩者同格；遊戲已經結束，玩家這一幀便不會再移動。

## 本節小結

- 所有鬼共用 `MoveGhost`，方向、計時器與策略存在各自身上。
- `isWall()` 在移動前排除牆面和地圖外座標。
- 鬼優先避免回頭，死路時允許反向移動。
- `setColor()` 讓多個物件以不同顏色共用同一素材。
- 互換位置不會觸發碰撞，所以鬼移動前後都自己檢查一次是否抓到玩家。

[遊戲狀態與 Overlay](05-game-states.md){ .md-button .md-button--primary }
