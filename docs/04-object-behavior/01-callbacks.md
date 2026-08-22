# CallbackGridObject

打地鼠把 `UpdateMole` 傳給 `Spawn()`，之後沒有自行撰寫 while-loop。Engine 已經擁有主迴圈，每幀會在固定階段呼叫這個函式。預先交給系統、等事件發生時再由系統呼叫的函式稱為 callback。

Grid++ 使用 `CallbackGridObject` 保存普通函式的指標。函式式 `Spawn()` 會在內部建立這個類別，公開回傳型別維持 `GridObject*`。呼叫端可以使用座標、素材、tag、visible 與 z-index 等共通 API，不必知道內部 instance 的實際型別。

```cpp
GridObject* mole = game.Spawn("mole", 0, 0, UpdateMole);
```

`mole` 指向的實際物件是 CallbackGridObject，這個 class 繼承 GridObject。繼承讓 Engine 能以相同方式保存 callback 物件和後續建立的自訂物件。第 5 章會在物件需要額外 instance 狀態時直接使用這個關係。

!!! info "C++ 函式指標"

    函式編譯後也位於記憶體中。函式指標保存函式的位址，程式可以先保存它，之後再透過指標呼叫該函式。函式指標的型別由回傳型別與參數型別共同決定。

    CallbackGridObject 使用兩種函式指標。更新 callback 指向接受一個 `GridObject*` 的函式；碰撞 callback 指向接受兩個 `GridObject*` 的函式。

    ```cpp
    void (*update)(GridObject*);
    void (*collide)(GridObject*, GridObject*);
    ```

    兩者的回傳型別都是 `void`。括號內的 `*update` 和 `*collide` 表示它們是指標，後面的括號列出目標函式必須接受的參數。

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

    傳入 `nullptr` 代表沒有指定函式，對應事件發生時便不會呼叫 callback。

## 函式式 Spawn

函式式多載依序接受素材名稱、初始 x、初始 y、更新 callback 與碰撞 callback。兩個 callback 都是函式指標；碰撞參數有預設值，可省略。

```cpp
game.Spawn(asset_name, x, y, update, collide);
```

update 或 collide 可以傳入 `nullptr`，表示物件不處理該事件。固定牆面或背景通常不需要更新；只會移動且沒有碰撞反應的物件可以省略 collide。

```cpp
game.Spawn("wall", 2, 2, nullptr);
game.Spawn("player", 1, 1, MovePlayer);
game.Spawn("pellet", 4, 1, nullptr, CollectPellet);
```

## 更新 callback

更新函式的型別為 `void (*)(GridObject* self)`。`self` 指向目前收到更新的物件，使同一個函式可以讀寫不同物件的 GridObject 狀態。

```cpp
void MovePlayer(GridObject* self) {
    GridEngine* game = self->engine();

    if (IsKeyPressed(KEY_RIGHT) && self->x() < game->cols() - 1) {
        self->Move(1, 0);
    }
    if (IsKeyPressed(KEY_LEFT) && self->x() > 0) {
        self->Move(-1, 0);
    }
    if (IsKeyPressed(KEY_DOWN) && self->y() < game->rows() - 1) {
        self->Move(0, 1);
    }
    if (IsKeyPressed(KEY_UP) && self->y() > 0) {
        self->Move(0, -1);
    }
}
```

物件經過 Spawn 後，`self->engine()` 會回傳所屬 Engine 的借用指標。這裡以 Engine 的欄列數限制移動範圍。callback 不應保存 `self` 的所有權或自行 delete；需要永久移除物件時呼叫 `self->engine()->Destroy(self)`。

打地鼠的 `UpdateMole` 使用同一個參數取得格子大小、欄列數和自己的位置：

```cpp
void UpdateMole(GridObject* self) {
    GridEngine* game = self->engine();
    const Vector2 mouse = GetMousePosition();
    const int mouse_x = static_cast<int>(mouse.x) / game->grid_size();
    const int mouse_y = static_cast<int>(mouse.y) / game->grid_size();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        mouse_x == self->x() && mouse_y == self->y()) {
        ++score;
        next_move = 0.0;
    }
}
```

完整倒數、Label 更新與 R 重設仍由第 2 章的同一個 callback 完成。這些全局變數屬於整局遊戲，單一地鼠不需要額外資料。

## 碰撞 callback

碰撞函式的型別為 `void (*)(GridObject* self, GridObject* other)`。`self` 是接收通知的物件，`other` 是同格的另一個物件。

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

一個格子可同時有三個以上物件，碰撞 callback 也可能在同一幀執行多次。配對與中途隱藏、移動或刪除的精確規則見[同格碰撞](02-collisions.md)。

## Callback 的資料範圍

callback 可以直接操作 GridObject 內建狀態，也可以讀寫整局共用的全局狀態。函式本身不會為每個 instance 建立一份持久資料；兩個物件使用同一 callback 時，全局與 static 變數仍只有一份。

牆、豆子、單一玩家或只有一份共用計時器的物件適合 callback。當多個 instance 分別需要生命值、方向或計時器時，資料應存入物件本身。下一章會從三隻地鼠共用 `next_move` 的錯誤行為開始，建立自訂 GridObject。

## Summary

- 函式式 Spawn 在內部建立 CallbackGridObject，回傳 GridObject 借用指標。
- update callback 與 collision callback 都是函式指標。
- update callback 每幀收到 `self`；collision callback 另外收到 `other`。
- `nullptr` 表示不處理對應事件。
- callback 適合使用 GridObject 內建狀態或整局共用狀態的物件。

型別與建構子簽名見 [CallbackGridObject API](../api/classgridpp_1_1_callback_grid_object.md)。
