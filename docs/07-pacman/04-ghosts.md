# 多隻鬼的獨立狀態

四隻鬼使用相同的移動程式，但每隻都要記住自己的方向、移動計時器、顏色與策略。`Ghost` 類別讓每次建立實例時同時建立一份 `timer_`、`direction_` 和 `random_`，正好延續第 5 章處理的個別狀態問題。以下第一段先保留方法骨架，後兩節再依序補上合法方向與選擇策略；三段合起來才是完整的 `OnUpdate()`。

```cpp title="main.cpp 節錄：Ghost 類別骨架"
class Ghost : public GridObject {
public:
    Ghost(int x, int y, Color color, bool is_random)
        : GridObject("ghost", x, y), random_(is_random) {
        set_tag("ghost");
        set_tint(color);
    }

    void OnUpdate() override {
        if (g_state != GameState::kPlaying || g_paused || g_player == nullptr) return;
        if (++timer_ < 12) return;
        timer_ = 0;

        // 選擇可走的方向，再移動一格。
    }

private:
    bool random_;
    int timer_ = 0;
    int direction_ = 0;
};
```

玩家每 8 幀移動，鬼每 12 幀移動，因此在相同更新率下鬼會稍慢，而每隻鬼各自累加 `timer_`，也不會重設其他實例的計時器。這是依賴幀率的簡化計時；若要固定每秒速度，應改用實際經過時間。`set_tint(color)` 則讓四隻鬼共用同一張白色素材，再以不同顏色繪製。

## 收集合法方向

鬼的移動選擇可以先分成三步理解：第一步列出所有不是牆的相鄰方向，第二步在仍有其他選擇時排除立刻回頭的方向，第三步才從剩餘候選中隨機挑選，或比較哪一格更接近玩家。下列程式只完成前兩步，其中 `any` 保存所有非牆方向，`options` 再排除目前方向的反向，只有走到死路、`options` 為空時，才退回 `any` 允許鬼轉身。

```cpp title="main.cpp 節錄：Ghost::OnUpdate() 收集候選方向"
int options[4];
int option_count = 0;
int any[4];
int any_count = 0;

for (int direction = 0; direction < 4; ++direction) {
    if (g_maze->IsWall(x() + kDirectionX[direction],
                       y() + kDirectionY[direction])) {
        continue;
    }

    any[any_count++] = direction;
    if (direction == (direction_ ^ 2)) continue;
    options[option_count++] = direction;
}

int* pool = option_count > 0 ? options : any;
const int pool_count = option_count > 0 ? option_count : any_count;
if (pool_count == 0) return;
```

方向編號 0、1、2、3 對應右、上、左、下，二進位表示分別是 `00`、`01`、`10`、`11`。和 `2`（二進位 `10`）做 XOR 會得到 `0 ↔ 2`、`1 ↔ 3`，所以 `direction_ ^ 2` 正好取得反向方向；若四周全是牆，`pool_count` 為 0，鬼便保留原位。

## 隨機與追蹤策略

第四隻鬼在合法方向中隨機選擇。其餘鬼計算每個候選格到玩家的曼哈頓距離，選擇距離最小的方向。

```cpp title="main.cpp 節錄：Ghost::OnUpdate() 選擇並移動"
int pick = pool[0];
if (random_) {
    pick = pool[GetRandomValue(0, pool_count - 1)];
} else {
    int best = 1 << 30;
    for (int i = 0; i < pool_count; ++i) {
        const int direction = pool[i];
        const int distance = ManhattanDistance(
            x() + kDirectionX[direction],
            y() + kDirectionY[direction],
            g_player->x(),
            g_player->y()
        );
        if (distance < best) {
            best = distance;
            pick = direction;
        }
    }
}

direction_ = pick;
Move(kDirectionX[direction_], kDirectionY[direction_]);
```

`g_player` 是 Engine 擁有玩家物件的借用指標。Ghost 只讀取座標，不釋放也不保存所有權。重新建立關卡時，`BuildLevel()` 先把它設為 `nullptr`，再指向新玩家。

## 建立不同的鬼

地圖中的每個 `3` tile 生成一隻 Ghost。顏色依生成順序循環，第四隻使用隨機策略。

```cpp title="main.cpp 節錄：BuildLevel() 生成鬼的部分"
const Color colors[4] = {RED, PINK, SKYBLUE, ORANGE};
int ghost_count = 0;

if (tile == 3) {
    game.Spawn(new Ghost(
        x,
        y,
        colors[ghost_count % 4],
        ghost_count == 3
    ));
    ++ghost_count;
}
```

## 本節小結

- 每個 Ghost 實例保存自己的方向、計時器與策略。
- `IsWall()` 在移動前排除牆面和地圖外座標。
- 鬼優先避免回頭，死路時允許反向移動。
- `tint` 讓多個物件以不同顏色共用同一素材。
