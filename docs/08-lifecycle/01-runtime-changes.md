# 執行期間新增與刪除物件

遊戲執行期間常會產生子彈、敵人與道具，也會在碰撞後移除物件。若 callback 立刻修改 Engine 正在巡覽的物件陣列，後續迭代可能跳過元素或讀取已釋放的記憶體。Grid++ 使用 deferred changes：先記錄要求，在安全的幀末統一修改容器。

## Runtime Spawn

主迴圈外呼叫 `Spawn()` 時，Engine 立即加入物件並執行 `OnSpawn()`。主迴圈內呼叫時，物件先進入待加入佇列；幀末執行 `OnSpawn()`，下一幀才開始更新、碰撞與繪製。

```cpp
void Spawner::OnUpdate() {
    if (ShouldCreateEnemy()) {
        engine()->Spawn(new Enemy(x(), y()));
    }
}
```

新 Enemy 不會參與本幀剩餘的碰撞與繪製。這建立固定邊界，讓生成時機不依賴 Spawner 在物件陣列中的位置。`OnSpawn()` 若丟出例外，Engine 會移除並釋放新物件，再傳遞例外。

## Deferred Destroy

`Destroy(object)` 在主迴圈內會立即把物件標記為待刪除。該物件不再參與本幀後續更新、碰撞或繪製，實際 `delete` 在幀末發生。

```cpp
void Pellet::OnCollide(GridObject* other) {
    if (other->tag() != "player") return;
    engine()->Destroy(this);
    return;
}
```

呼叫 Destroy 後便應停止使用指標。延後釋放是 Engine 內部的容器安全措施，不會延長使用者的借用期限。傳入 `nullptr` 或不屬於該 Engine 的指標不產生效果。

## 隱藏物件

`set_visible(false)` 適合之後還會出現的物件。它會立即停止繪製與碰撞，物件仍每幀更新，也仍由 Engine 擁有。打地鼠的倒數結束使用這個方式，R 重設時再顯示同一 instance。

Destroy 適合永久移除。若 callback 之後不再需要物件資料，刪除可以縮小物件集合；若物件需要自行倒數並重新出現，visible 能保留成員狀態。

## 清除關卡

`ClearObjects()` 移除目前所有 GridObject 和待加入物件。主迴圈內呼叫時使用相同的延後機制。Overlay 與素材不受影響，因此 Pacman 的 Restart 按鈕可以重建迷宮與角色，同時保留選單和已載入的 texture。

```cpp
void BuildLevel(GridEngine& game, const LevelMap& level) {
    game.ClearObjects();
    g_maze = nullptr;
    g_player = nullptr;
    // Spawn the new level from validated data.
}
```

清除物件後，所有指向舊關卡的借用指標都應立即視為無效。範例先將 `g_maze` 和 `g_player` 設為 `nullptr`，再指向新物件。

## Runtime Overlay

`AddOverlay()` 可以在 Overlay 或物件 callback 中呼叫。Engine 在每幀開始時記住 Overlay 數量，本幀更新與繪製都只處理這批 Overlay；執行期間加入的 Overlay 從下一幀開始運作。

Grid++ 目前沒有刪除單一 Overlay 的公開函式。會切換畫面的 Overlay 可在 `OnUpdate()` 和 `Draw()` 中根據遊戲狀態選擇行為，Pacman 的三個按鈕便採用這種方式。

## Summary

- 主迴圈內產生的物件從下一幀開始運作。
- Destroy 立即停止物件行為，實際釋放延後到幀末。
- visible 保留 instance，適合暫時隱藏；Destroy 永久移除。
- `ClearObjects()` 清除 GridObject，保留 Overlay 與素材。
- callback 中新增的 Overlay 從下一幀開始更新與繪製。
