# Engine 所有權與借用指標

GridEngine 擁有所有透過 `Spawn()` 加入的 GridObject，以及所有透過 `AddOverlay()` 加入的 Overlay。擁有者負責在適當時機呼叫 `delete`；遊戲程式取得的回傳指標只用來操作仍存在的物件，稱為借用指標。

這項契約讓最基本的 API 保持直接：程式建立物件並交給 Engine，之後由 Engine 統一清理。Grid++ 的介面使用原始指標，而原始指標本身不會記錄誰負責釋放記憶體，所以呼叫端必須另外遵守所有權與有效期限。

## 轉交所有權

傳入 `Spawn(GridObject*)` 或 `AddOverlay(Overlay*)` 的物件必須由 `new` 建立，而且尚未交給其他 Engine。呼叫成功後，不可自行 `delete`。

```cpp title="所有權轉交示意"
GridObject* player = game.Spawn(new Player(2, 3));

Label* score = new Label("Score: 0", 8, 8);
game.AddOverlay(score);
```

`player` 和 `score` 都是借用指標。程式可以透過它們修改物件，但記憶體由 `game` 釋放。同一個物件不可生成兩次，也不可交給兩個 Engine；Grid++ 會檢查物件是否已有 Engine，並在重複加入時丟出 `std::logic_error`。

## 不可傳入區域物件

區域變數離開作用域時會由 C++ 自動解構，Engine 之後也會嘗試 `delete` 自己擁有的物件。把區域物件的位址交給 Engine，會形成兩個清理者，可能造成無效記憶體釋放、程式崩潰或未定義行為。

```cpp title="錯誤示意：不可交出區域物件的位址"
// 錯誤：local 是區域物件。
GridObject local("box", 1, 1);
game.Spawn(&local);
```

正確版本以 `new` 建立動態物件，並立即把所有權轉交給 Engine：

```cpp title="正確示意：以 new 建立並立即轉交"
game.Spawn(new GridObject("box", 1, 1));
```

函式式 `Spawn()` 不會暴露這項配置步驟；`game.Spawn("box", 1, 1, UpdateBox)` 會由 Engine 在內部建立 `CallbackGridObject`，再以相同規則管理。

## 借用指標的期限

下列事件會使 GridObject 借用指標失效：

- `Destroy(pointer)` 套用後。
- `ClearObjects()` 清除關卡後。
- GridEngine 解構後。

程式在呼叫 `Destroy()` 後應立即停止使用指標，即使 `delete` 延後到幀末。跨關卡保存的全域指標應在 `ClearObjects()` 時設為 `nullptr`，待新物件生成後重新指定。

Overlay 借用指標通常有效到 Engine 解構，因為目前沒有移除單一 Overlay 的 API。Label 的 `set_text()` 可以在這段期間安全使用，前提是其 Engine 仍存在。

## Engine 解構

GridEngine 解構時依序釋放 GridObject、Overlay 與素材紋理，最後關閉 raylib 視窗。紋理卸載時仍需要有效的圖形環境，因此清理順序由 Engine 固定處理。

```cpp title="main()：Engine 離開作用域時統一清理"
int main() {
    GridEngine game(8, 8, 64);
    game.Spawn(new Player(0, 0));
    game.Run();
}  // game 與它擁有的資源都會在此釋放。
```

將 Engine 建立為 `main()` 的區域變數，可讓視窗關閉後自然執行完整清理。Engine 不可複製，避免兩個實例同時宣稱擁有同一批資源。

## 本節小結

- `Spawn()` 與 `AddOverlay()` 成功後，Engine 接管物件所有權。
- 傳入的原始指標必須來自 `new`，不可指向區域物件。
- 回傳指標只借用物件，呼叫端不可 `delete`。
- `Destroy()`、`ClearObjects()` 與 Engine 解構會終止借用期限。
- Engine 會在關閉視窗前釋放素材紋理與所有動態物件。
