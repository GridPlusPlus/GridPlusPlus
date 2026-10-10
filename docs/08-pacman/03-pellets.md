# 豆子與勝利條件

每個 `0` tile 會建立一顆豆子。豆子留在固定座標，沒有每幀更新行為；它只需要在玩家進入同一格時消失，並更新全域的剩餘數量。

## 建立所有豆子

`BuildLevel()` 掃描已驗證的 tile 陣列，每遇到 `0`，便建立一顆豆子、記錄它的種類，並增加剩餘數量：

```cpp title="main.cpp 節錄：BuildLevel() 建立豆子的部分"
for (int y = 0; y < level.rows; ++y) {
    for (int x = 0; x < level.cols; ++x) {
        const int tile = level.tiles[y][x];
        if (tile == 1) {
            // 設定牆面（8.1 節）。
        } else if (tile == 0) {
            GridObject pellet = game.addObject("pellet", nullptr, nullptr, EatPellet);
            pellet.setPosition(x, y);
            pellet.set("type", "pellet");
            ++pellets_left;
        }
        // ...
    }
}
```

豆子沒有 Init 與 Update，所以前兩個函式位置都是 `nullptr`，只在第三個位置交出 Collide 函式 `EatPellet`。`pellet` 這個 Handler 只在迴圈的這一輪使用，離開大括號後就消失，但豆子本身仍由 Engine 保存；程式之後不需要再個別找到某一顆豆子。

剩餘數量屬於整局遊戲，所有豆子與分數文字都會讀取它，因此使用全域變數。`BuildLevel()` 開頭會先把 `pellets_left` 歸零，否則重新開始時新一局會累加上一局的數量。

## 吃掉豆子

碰撞時，豆子先確認對方是玩家，接著移除自己並減少剩餘數量：

```cpp title="main.cpp 節錄：TypeOf() 與 EatPellet()"
std::string TypeOf(GridObject object) {
    std::string type;
    object.get("type", type);
    return type;
}

void EatPellet(GameEngine, GridObject self, GridObject other) {
    if (TypeOf(other) != "pacman") return;

    self.remove();
    if (--pellets_left <= 0) game_state = GameState::kWon;
}
```

`TypeOf()` 是範例自己寫的小工具函式，把「宣告字串、呼叫 `get()`、回傳結果」三行包在一起，讓碰撞函式可以直接比較。鬼經過豆子時，`TypeOf(other)` 是 `"ghost"`，函式立即返回，豆子不受影響。

`self.remove()` 讓豆子立即離開遊戲：它不再參與這一幀剩下的碰撞，也不會被畫出來，因此同一顆豆子不可能被重複計算。`--pellets_left` 先把數量減一再比較，當最後一顆豆子被吃掉、數量降到 0 時，遊戲進入 `GameState::kWon`。

## 玩家碰到鬼

豆子負責自己的收集行為；玩家則負責碰到鬼時的結果。玩家的 Collide 函式 `HitPlayer` 用同樣的方式辨認對方：

```cpp title="main.cpp 節錄：HitPlayer()"
void HitPlayer(GameEngine, GridObject, GridObject other) {
    if (TypeOf(other) == "ghost") game_state = GameState::kLost;
}
```

這個函式用不到 `game` 與 `self`，所以兩個參數都只寫型別。碰撞發生在所有物件更新完成後；同一幀中，玩家與鬼可能各自移入相同格子，Engine 仍會在接下來的碰撞階段找到它們。素材的透明區域與 layer 都不影響結果。

## 本節小結

- 每顆豆子只有 Collide 函式，並以 `"type"` 辨認玩家。
- `remove()` 讓吃掉的豆子立即離開遊戲，不會重複計分。
- `pellets_left` 是整局共享狀態，歸零時進入勝利狀態。
- 碰撞依網格座標判斷，和素材像素與繪製層級無關。

[多隻鬼的獨立狀態](04-ghosts.md){ .md-button .md-button--primary }
