# 多隻鬼的獨立狀態

四隻鬼使用相同的移動程式，但每隻都要記住自己的方向、移動計時器、顏色與策略。`Ghost` class 讓每次建立 instance 時同時建立一份 `timer_`、`direction_` 和 `random_`。這正是第 5 章從多隻地鼠遇到的共享狀態問題所引出的設計。

```cpp
class Ghost : public GridObject {
public:
    Ghost(int x, int y, Color color, bool is_random)
        : GridObject("ghost", x, y), random_(is_random) {
        set_tag("ghost");
        set_tint(color);
    }

    void OnUpdate() override {
        if (g_state != 1 || g_paused || g_player == nullptr) return;
        if (++timer_ < 12) return;
        timer_ = 0;

        // Choose a legal direction, then move one cell.
    }

private:
    bool random_;
    int timer_ = 0;
    int direction_ = 0;
};
```

玩家每 8 幀移動，鬼每 12 幀移動。每隻鬼各自累加 `timer_`，互不重設其他 instance 的計時器。`set_tint(color)` 讓四隻鬼共用同一張白色素材，再以不同顏色繪製。

## 收集合法方向

鬼每次準備移動時檢查四個相鄰格。`any` 保存所有非牆方向；`options` 再排除目前方向的反向。只有走到死路、`options` 為空時，才允許從 `any` 回頭。

```cpp
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

方向編號 0、1、2、3 對應右、上、左、下。`direction_ ^ 2` 會把右和左、上和下互換，得到反向方向。若四周全是牆，`pool_count` 為 0，鬼保留原位。

## 隨機與追蹤策略

第四隻鬼在合法方向中隨機選擇。其餘鬼計算每個候選格到玩家的曼哈頓距離，選擇距離最小的方向。

```cpp
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

```cpp
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

## Summary

- 每個 Ghost instance 保存自己的方向、計時器與策略。
- `IsWall()` 在移動前排除牆面和地圖外座標。
- 鬼優先避免回頭，死路時允許反向移動。
- tint 讓多個物件以不同顏色共用同一素材。
