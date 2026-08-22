# 每幀執行順序

GridEngine 的每一幀由更新、碰撞、繪製與生命週期套用組成。順序決定同一幀中的狀態何時可見，例如 OnUpdate 修改座標後，本幀碰撞會使用新座標；修改 z-index 後，本幀繪製也會使用新值。

```text
1. GridObject update (spawn order)
2. Overlay update (frame-start overlays)
3. GridObject collision pairs (spawn order)
4. Clear background and draw grid
5. GridObject render (z-index, then spawn order)
6. Overlay draw (frame-start overlays)
7. Apply deferred destroy and spawn
```

## 更新

Engine 依原始 spawn 順序呼叫每個有效物件的 `OnUpdate()`，接著更新本幀開始時已存在的 Overlay。隱藏的物件仍更新；排定刪除的物件會被跳過。前一個物件所做的狀態修改可由後一個物件在同一更新階段讀取。

Overlay 更新位於物件更新之後。callback 中加入的新 Overlay 不在本幀開始時記錄的範圍內，下一幀才更新與繪製。

## 碰撞

更新全部完成後，Engine 依 spawn 順序檢查每一組有效物件。座標修改會在同一幀生效；隱藏、移到地圖外或 Destroy 也會立即改變後續配對的資格。完整配對規則見[同格碰撞](../04-object-behavior/02-collisions.md)。

碰撞不依賴 z-index。畫在最上方的物件沒有較高碰撞優先權，遊戲規則若需要優先順序，應在 callback 或遊戲狀態中明確表達。

## 繪製

Engine 清除背景並選擇性繪製格線，再複製目前物件指標，使用 stable sort 依 z-index 由小到大排列。相同 z-index 維持 spawn 順序。排序只作用於這份繪製用副本，不會改變下一幀更新或碰撞順序。

所有可見 GridObject 繪製完成後，Engine 依加入順序呼叫 Overlay 的 `Draw()`。Overlay 沒有 z-index，後加入者會畫在較早加入者上方。

## 幀末變更

繪製完成後，Engine 先釋放排定刪除的物件，再將待加入物件移入正式集合並呼叫其 `OnSpawn()`。新物件在下一幀才進入更新、碰撞與繪製。

如果幀末的 `OnSpawn()` 丟出例外，Engine 會移除並釋放該物件，例外離開 `Run()`。呼叫端可以在 `main()` 捕捉並印出錯誤；已存在的 Engine 資源仍會由解構流程清理。

## Summary

- 更新後的座標和 z-index 會影響同一幀的碰撞與繪製。
- 碰撞和更新使用 spawn 順序，繪製使用 z-index 的 stable sort。
- Overlay 在所有 GridObject 後更新與繪製。
- runtime spawn 與實際 delete 在幀末套用。
