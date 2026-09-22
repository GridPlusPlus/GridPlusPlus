# 執行期間新增與刪除

callback 中可以直接加入新內容：

```cpp
void SpawnEnemy(Game game, ObjectHandler) {
    game.addObject("enemy", InitEnemy, MoveEnemy);
}
```

本幀中加入的元素先進入待加入佇列；幀末依實際加入順序執行 Init，下一幀才 Update、碰撞與繪製。
Init 又加入的內容會接在同一份順序後方完成 Init。

`self.remove()` 讓實體立即停止參與本幀後續碰撞與繪製，實際釋放延後到安全的幀末。呼叫後應立即
return，不再操作這個 Handler。

`hide()` 適合暫時隱藏：實體保留並繼續 Update，但不繪製或碰撞。`remove()` 適合永久移除。

`game.clearObjects()` 清除圖片物件、Shapes 與 Maze，但保留 Overlay 和素材；
`game.clearOverlays()` 清除圖片、文字與按鈕 Overlay。這讓 Restart 可以重建關卡而保留介面。
