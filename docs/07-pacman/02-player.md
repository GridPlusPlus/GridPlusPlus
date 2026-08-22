# 玩家移動

Pacman 會持續沿目前方向移動。玩家可以在抵達路口前先按下方向鍵；程式記住這個輸入，等目標方向不再是牆時完成轉向。這需要保存目前方向、玩家希望前往的方向，以及控制移動速度的計時器。

`Pacman` 繼承 `GridObject`，三項資料都成為每個 instance 的成員變數。四個方向以 0～3 表示，並使用兩個陣列取得 x、y 位移。

```cpp
constexpr int kDirectionX[4] = {1, 0, -1, 0};
constexpr int kDirectionY[4] = {0, -1, 0, 1};

class Pacman : public GridObject {
public:
    Pacman(int x, int y) : GridObject("pacman", x, y) {
        set_tag("pacman");
    }

    void OnUpdate() override {
        if (g_state != 1 || g_paused) return;

        if (IsKeyDown(KEY_RIGHT)) wanted_direction_ = 0;
        if (IsKeyDown(KEY_UP)) wanted_direction_ = 1;
        if (IsKeyDown(KEY_LEFT)) wanted_direction_ = 2;
        if (IsKeyDown(KEY_DOWN)) wanted_direction_ = 3;

        if (++timer_ < 8) return;
        timer_ = 0;

        if (wanted_direction_ >= 0 &&
            !g_maze->IsWall(x() + kDirectionX[wanted_direction_],
                            y() + kDirectionY[wanted_direction_])) {
            direction_ = wanted_direction_;
        }

        if (direction_ >= 0 &&
            !g_maze->IsWall(x() + kDirectionX[direction_],
                            y() + kDirectionY[direction_])) {
            Move(kDirectionX[direction_], kDirectionY[direction_]);
            set_direction(direction_);
        }
    }

private:
    int timer_ = 0;
    int direction_ = -1;
    int wanted_direction_ = -1;
};
```

每幀都讀取按鍵，讓短暫輸入能更新 `wanted_direction_`。實際移動每 8 幀發生一次；到達移動時機後，程式先嘗試採用希望的方向，再檢查目前方向能否前進。`GridMaze::IsWall()` 對地圖外座標回傳 `true`，所以同一段判斷也處理邊界。

`set_direction(direction_)` 只旋轉素材，不會移動物件。方向編號和素材旋轉規則使用相同的右、上、左、下順序，因此一張朝右的 Pacman 圖片可以顯示四個方向。

玩家在掃描地圖時先以 `Pacman*` 暫存，最後才 spawn：

```cpp
Pacman* player = nullptr;

// Inside the tile loop:
if (tile == 2) {
    player = new Pacman(x, y);
}

// After the tile loop:
if (player != nullptr) {
    g_player = player;
    game.Spawn(player);
}
```

`LevelMap` 已保證恰好有一個玩家，因此有效地圖會建立 `player`。把玩家留到最後 spawn，也讓預設 z-index 相同時的玩家顯示在豆子和鬼上方。`g_player` 是 Ghost 查詢位置的借用指標，重新建立關卡前必須清除。

## Summary

- `wanted_direction_` 保存尚未能執行的轉向輸入。
- `direction_` 保存目前移動方向，`timer_` 控制移動間隔。
- 角色在移動前使用 `GridMaze::IsWall()` 查詢目標格。
- `set_direction()` 旋轉素材，`Move()` 修改網格座標。
