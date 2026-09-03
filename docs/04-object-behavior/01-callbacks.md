# 用 UpdateFn 與 CollideFn 描述行為

第 3 章的打地鼠程式把 `UpdateMole` 交給 `Spawn()` 之後，便能在沒有自行撰寫主迴圈的情況下持續處理滑鼠輸入與地鼠移動，原因是 Engine 已經負責反覆更新畫面，並且會在每一幀的固定階段呼叫這個函式。從遊戲作者的角度來看，現在最重要的不是先記住這種機制的術語，而是理解兩個清楚的角色：`UpdateFn` 負責物件每一幀要做的事，`CollideFn` 則負責物件和另一個物件進入同一格時要做的事。

## 讓物件每一幀更新

一個可作為 `UpdateFn` 使用的函式會接收 `GridObject* self`，其中 `self` 指向這次正在更新的物件，因此同一個函式不必把物件寫死在全域變數中，也能查詢它的位置、改變它的素材，或透過 `engine()` 取得所在的遊戲世界。以下玩家移動函式先讀取方向鍵，再用 Engine 的欄列數避免玩家走出網格；把它交給 `Spawn()` 後，Engine 便會在每一幀替玩家呼叫它。

```cpp
void MovePlayer(GridObject* self) {
    GridEngine* game = self->engine();

    if (IsKeyPressed(KEY_RIGHT) && self->x() < game->cols() - 1) {
        self->Move(1, 0);
    } else if (IsKeyPressed(KEY_LEFT) && self->x() > 0) {
        self->Move(-1, 0);
    } else if (IsKeyPressed(KEY_DOWN) && self->y() < game->rows() - 1) {
        self->Move(0, 1);
    } else if (IsKeyPressed(KEY_UP) && self->y() > 0) {
        self->Move(0, -1);
    }
}

GridObject* player = game.Spawn("player", 1, 1, MovePlayer);
```

`self` 與 `game` 都只是借用指標，物件的生命週期仍由 Engine 管理，因此函式不應自行 `delete self`，也不應把取得的指標當成自己擁有的資源；若物件需要永久離開世界，應要求所屬 Engine 執行 `Destroy(self)`，詳細的安全時機會集中到第 8 章說明。

!!! info "C++ 函式指標"

    `UpdateFn` 與 `CollideFn` 背後使用的是函式指標：函式編譯後也位於記憶體中，而函式指標保存函式的位址，使程式可以先記住要執行哪個函式，等更新或碰撞發生時再透過該位址呼叫它。

    `UpdateFn` 指向接受一個 `GridObject*` 的函式，`CollideFn` 則指向接受兩個 `GridObject*` 的函式；兩者都不回傳資料，因此展開後可以寫成：

    ```cpp
    void (*update)(GridObject*);
    void (*collide)(GridObject*, GridObject*);
    ```

    括號內的 `*update` 和 `*collide` 表示它們是指標，後面的括號則列出目標函式必須接受的參數。

    ```cpp
    void UpdateMole(GridObject* self) {
        // Update the mole.
    }

    void HitMole(GridObject* self, GridObject* other) {
        // Handle the collision.
    }

    void (*update)(GridObject*) = UpdateMole;
    void (*collide)(GridObject*, GridObject*) = HitMole;

    update(mole);
    collide(mole, other);
    ```

    指派時可以直接寫函式名稱 `UpdateMole`；C++ 會在這個位置取得函式位址，因此通常不需要寫成 `&UpdateMole`。被傳入的函式必須使用相同的回傳型別與參數型別，編譯器會拒絕不相容的函式。

    這段語法是理解底層機制的補充，而不是使用 Grid++ 時必須反覆書寫的形式；實際程式直接把符合參數規格的函式名稱交給 `Spawn()` 即可，若傳入 `nullptr`，則表示物件不需要處理對應事件。

## 同時指定更新與碰撞行為

當物件除了每幀更新之外，也需要在相遇時作出反應，`Spawn()` 可以在素材名稱與初始座標之後依序接收 `UpdateFn` 和 `CollideFn`；沒有某項行為時可以傳入 `nullptr`，而最後的 `CollideFn` 具有預設值，因此只需要更新的物件可以直接省略它。

```cpp
game.Spawn(asset_name, x, y, update_fn, collide_fn);
```

```cpp
game.Spawn("wall", 2, 2, nullptr);
game.Spawn("player", 1, 1, MovePlayer);
game.Spawn("pellet", 4, 1, nullptr, CollectPellet);
```

前三行分別建立不需更新的牆面、只需更新的玩家，以及只需處理碰撞的豆子。豆子的 `CollectPellet` 接收 `self` 與 `other`：前者是正在處理碰撞的豆子，後者是和它位於同一格的另一個物件，因此函式必須先辨認對方是不是玩家，才能決定是否加分與移除自己。

```cpp
void CollectPellet(GridObject* self, GridObject* other) {
    if (other->tag() != "player") return;

    ++score;
    self->engine()->Destroy(self);
}

GridObject* pellet = game.Spawn(
    "pellet", 2, 3, nullptr, CollectPellet
);
```

這裡的 `tag` 把「座標相同」轉換成有遊戲意義的「玩家吃到豆子」，下一節會完整說明同格判定與 tag 的分工；至於同一幀內的精確配對順序，以及碰撞過程中刪除物件會如何影響後續事件，則留到第 8 章建立完整生命週期模型後再處理。

## Summary

- `UpdateFn` 每幀收到 `self`；`CollideFn` 在同格時另外收到 `other`。
- 兩者背後使用函式指標，通常只要把符合參數規格的函式名稱交給 `Spawn()`。
- `nullptr` 表示不處理對應事件。
- 函式式行為適合只需使用 GridObject 內建資料或整局共用狀態的物件。

學會更新與碰撞之後，下一節會把兩者組合成玩家吃豆子的完整互動；等到第 5 章需要讓多個物件各自保存計時器時，再進一步處理普通函式無法直接承擔的資料歸屬問題。
