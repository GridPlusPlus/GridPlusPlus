# 同格碰撞

網格遊戲通常以格子判斷互動。玩家與豆子位於同一格時，豆子被收集；玩家與敵人位於同一格時，遊戲結束。Grid++ 會在每幀更新完成後比較所有 GridObject 的座標，並對位於相同有效格子的物件呼叫碰撞行為。

碰撞 callback 的型別是 `void (*)(GridObject* self, GridObject* other)`。`self` 指向接收通知的物件，`other` 指向同格的另一個物件。兩者都是 Engine 擁有物件的借用指標。

## 使用 tag 辨識物件

座標只能說明兩個物件相遇，無法說明對方的角色。`tag` 是由遊戲指定的文字標記，碰撞函式可用它區分玩家、豆子和敵人。

```cpp
int score = 0;

void CollectPellet(GridObject* self, GridObject* other) {
    if (other->tag() != "player") return;

    ++score;
    self->engine()->Destroy(self);
}

GridObject* player = game.Spawn("player", 1, 1, MovePlayer);
player->set_tag("player");

GridObject* pellet = game.Spawn("pellet", 3, 1, nullptr, CollectPellet);
pellet->set_tag("pellet");
```

玩家走到 `(3, 1)` 後，Engine 呼叫豆子的 `CollectPellet`。函式先確認對方 tag，再增加分數並要求 Engine 刪除豆子。`Destroy(self)` 之後應立即停止使用 `self`；實際釋放時機會在[執行期間新增與刪除物件](../08-lifecycle/01-runtime-changes.md)中說明。

## 雙向通知

每組碰撞會先通知較早 spawn 的物件，再通知較晚 spawn 的物件。若玩家和豆子都定義碰撞行為，雙方會各收到一次通知，而且 `self` 與 `other` 的角色互換。

```text
player->OnCollide(pellet)
pellet->OnCollide(player)
```

第一個 callback 可能改變物件狀態。若它將任一物件隱藏、移到地圖外或排定刪除，該物件會失去碰撞資格，第二個 callback 便不執行。需要雙方都處理的規則，不應讓第一個 callback 提前移除其中一方。

## 三個以上物件位於同一格

Engine 依 spawn 順序檢查每一組物件。假設 A、B、C 依序 spawn，而且三者位於同一格，配對順序為 A–B、A–C、B–C；每組內仍先通知較早 spawn 的物件。

```text
A with B
A with C
B with C
```

callback 對座標、visible 或生命週期所做的修改會立刻影響尚未檢查的配對。例如 A 在 A–B 的 callback 中呼叫 `set_visible(false)`，A–C 便不會發生，B–C 仍照常檢查。z-index 只控制繪製順序，不參與碰撞排序。

## 有效座標與可見狀態

物件必須符合下列條件才參與碰撞：

- `visible()` 為 `true`。
- x 位於 `0` 到 `cols() - 1`。
- y 位於 `0` 到 `rows() - 1`。
- 尚未由 `Destroy()` 排定刪除。

位於 `(-1, -1)` 或其他地圖外座標的物件仍會更新與繪製，但 Engine 會忽略它的碰撞。暫時停用物件時，`set_visible(false)` 能同時停止繪製與碰撞，意圖也比特殊座標清楚。

## Summary

- 同格碰撞在物件更新完成後檢查。
- `tag` 讓 callback 辨識碰撞對象。
- 一組碰撞最多通知雙方各一次，順序依 spawn 次序。
- 三個以上物件會依配對順序處理，前面的 callback 可改變後續結果。
- 隱藏、越界或排定刪除的物件不參與碰撞。
