# Engine 所有權與借用指標

GridEngine 擁有所有透過 `Spawn()` 加入的 GridObject，以及所有透過 `AddOverlay()` 加入的 Overlay。擁有者負責在適當時機呼叫 `delete`；遊戲程式取得的回傳指標只用來操作仍存在的物件，稱為借用指標。

這項契約讓最基本的 API 保持直接：學生可以建立物件、交給 Engine，之後由 Engine 統一清理。raw pointer 本身不會記錄所有權，因此呼叫端必須遵守建立方式與有效期限。

## 轉交所有權

傳入 `Spawn(GridObject*)` 或 `AddOverlay(Overlay*)` 的物件必須由 `new` 建立，而且尚未交給其他 Engine。呼叫成功後，不可自行 delete。

```cpp
GridObject* player = game.Spawn(new Player(2, 3));

Label* score = new Label("Score: 0", 8, 8);
game.AddOverlay(score);
```

`player` 和 `score` 都是借用指標。程式可以透過它們修改物件，但記憶體由 `game` 釋放。同一個物件不可 spawn 兩次，也不可交給兩個 Engine；Grid++ 會檢查物件是否已有 Engine，並在重複加入時丟出 `std::logic_error`。

## Stack object 錯誤

區域變數離開作用域時由 C++ 自動解構，Engine 之後也會嘗試 delete 自己擁有的物件。將 stack object 的位址交給 Engine 會形成兩個清理者，可能造成 invalid free、crash 或未定義行為。

```cpp
// Error: local is a stack object.
GridObject local("box", 1, 1);
game.Spawn(&local);
```

正確版本在 heap 建立物件並立即轉交：

```cpp
game.Spawn(new GridObject("box", 1, 1));
```

函式式 Spawn 不會暴露這項配置步驟。`game.Spawn("box", 1, 1, UpdateBox)` 會在 Engine 內部建立 CallbackGridObject，再以相同規則管理。

## 借用指標的期限

下列事件會使 GridObject 借用指標失效：

- `Destroy(pointer)` 套用後。
- `ClearObjects()` 清除關卡後。
- GridEngine 解構後。

程式在呼叫 Destroy 後應立即停止使用指標，即使 delete 延後到幀末。跨關卡保存的全域指標應在 `ClearObjects()` 時設為 `nullptr`，待新物件 spawn 後重新指定。

Overlay 借用指標通常有效到 Engine 解構，因為目前沒有移除單一 Overlay 的 API。Label 的 `set_text()` 可以在這段期間安全使用，前提是其 Engine 仍存在。

## Engine 解構

GridEngine 解構時依序釋放 GridObject、Overlay 與素材 texture，最後關閉 raylib 視窗。Texture 需要有效的 OpenGL context 才能卸載，因此清理順序由 Engine 固定處理。

```cpp
int main() {
    GridEngine game(8, 8, 64);
    game.Spawn(new Player(0, 0));
    game.Run();
}  // game and all owned resources are released here.
```

將 Engine 建立為 `main()` 的區域變數，可讓視窗關閉後自然執行完整清理。Engine 不可複製，避免兩個 instance 同時宣稱擁有同一批資源。

## Summary

- Spawn 與 AddOverlay 成功後，Engine 接管物件所有權。
- 傳入的 raw pointer 必須來自 `new`，不可指向 stack object。
- 回傳指標只借用物件，呼叫端不可 delete。
- Destroy、ClearObjects 與 Engine 解構會終止借用期限。
- Engine 會在關閉視窗前釋放 texture 與所有動態物件。
