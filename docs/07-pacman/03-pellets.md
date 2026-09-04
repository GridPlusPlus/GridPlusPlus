# 豆子與勝利條件

每個 `0` tile 會建立一顆豆子。豆子留在固定座標，沒有每幀更新行為；它需要記住自己是否已被吃掉，並在玩家進入同一格時更新全域剩餘數量。這份狀態屬於單一豆子，因此 `Pellet` 使用成員變數 `eaten_`。以下類別位於共享狀態之後、`Ghost` 與 `Pacman` 之前。

```cpp title="main.cpp 節錄：Pellet 類別"
class Pellet : public GridObject {
public:
    Pellet(int x, int y) : GridObject("pellet", x, y) {
        set_tag("pellet");
    }

    void OnCollide(GridObject* other) override {
        if (!eaten_ && other->tag() == "pacman") {
            eaten_ = true;
            if (--g_pellets_left <= 0) g_state = GameState::kWon;
        }
    }

    void Render(GridEngine* engine) override {
        if (!eaten_) GridObject::Render(engine);
    }

private:
    bool eaten_ = false;
};
```

`OnCollide()` 先以 `other->tag()` 確認對方是玩家。`eaten_` 防止同一顆豆子重複扣除計數；當 `g_pellets_left` 降到 0，遊戲進入 `GameState::kWon`。`Render()` 只在豆子尚未被吃掉時呼叫基底繪製。

這個版本刻意保留已吃掉的豆子，藉此延續第 5 章「每個實例保存自己的狀態」；`eaten_` 同時阻止重複計分並控制繪製。如果豆子消失後不再需要被查詢，碰撞時直接呼叫 `engine()->Destroy(this)` 會更簡單，此時可以移除 `eaten_` 與自訂 `Render()`。兩種設計都符合碰撞模型，差別在於物件消失後是否仍有資料需要保留。

## 建立所有豆子

`BuildLevel()` 掃描已驗證的 tile 陣列。每遇到 `0`，便生成一顆 Pellet 並增加剩餘數量。

```cpp title="main.cpp 節錄：BuildLevel() 生成豆子的部分"
g_pellets_left = 0;

for (int y = 0; y < level.rows; ++y) {
    for (int x = 0; x < level.cols; ++x) {
        const int tile = level.tiles[y][x];
        if (tile == 0) {
            game.Spawn(new Pellet(x, y));
            ++g_pellets_left;
        }
    }
}
```

剩餘數量屬於整局遊戲，所有豆子與 Overlay 都會讀取它，因此使用共享狀態。重新建立關卡時必須在掃描前歸零，否則新一局會累加上一局的數量。

## 玩家碰到鬼

豆子負責自己的收集行為；玩家則負責碰到鬼時的結果。Pacman 的 `OnCollide()` 使用 ghost tag 將遊戲切換到失敗狀態。

```cpp title="main.cpp 節錄：Pacman::OnCollide()"
void OnCollide(GridObject* other) override {
    if (other->tag() == "ghost") g_state = GameState::kLost;
}
```

碰撞發生在所有物件更新完成後。同一幀中，玩家與鬼可能各自移入相同格子，Engine 仍會在接下來的碰撞階段找到它們。素材的透明區域與 z-index 不影響結果。

## 本節小結

- 每顆 Pellet 以 `eaten_` 保存自己的收集狀態。
- tag 讓豆子辨識玩家，讓玩家辨識鬼。
- `g_pellets_left` 是整局共享狀態，歸零時進入勝利狀態。
- 碰撞依網格座標判斷，和素材像素與繪製層級無關。
