# 物件生命週期

生命週期描述一個物件從建立、加入遊戲、持續運作到最後被移除的完整過程。遊戲主迴圈正在讀取物件時，其他 callback 仍可能要求生成或刪除物件；如果任何程式都能立刻修改容器或釋放記憶體，正在執行的迴圈和指標便可能失效。

Grid++ 把生命週期集中交給 GridEngine。Engine 擁有所有透過 `Spawn()` 加入的 GridObject，決定物件從哪一幀開始參與遊戲、何時停止，以及何時安全地釋放記憶體。使用者保存的是借用指標，只用來操作仍然存在的物件。

這套規則也定義了遊戲行為。runtime spawn 的物件不會在半個 frame 中途突然參與碰撞；要求 destroy 的物件會立即停止遊戲行為，但實際記憶體釋放延後到容器不再迭代的時間。以下流程是所有 GridObject 共用的狀態轉換。

```text
new 建立物件
    ↓
Spawn() 轉交所有權
    ↓
OnSpawn() 執行一次
    ↓
每幀：OnUpdate → OnCollide → Render
    ↓
Destroy() 或 ClearObjects()
    ↓
引擎釋放物件
```

## Spawn 與 OnSpawn

主迴圈尚未執行時，`Spawn()` 立即將物件加入引擎並呼叫 `OnSpawn()`。在 `OnUpdate()`、`OnCollide()` 或其他本幀 callback 中 spawn 時，引擎先把物件放入待加入佇列；幀末才呼叫 `OnSpawn()`，物件從下一幀開始更新、碰撞與繪製。

延後加入可避免物件容器正在迭代時失效，也建立一致的規則：本幀新產生的物件不會突然收到本幀剩餘的 callback。`OnSpawn()` 若丟出例外，引擎會先移除並 delete 該物件，再繼續傳遞例外。

## 每幀執行順序

每一幀依序執行下列階段：

1. 依 spawn 順序呼叫可用物件的 `OnUpdate()`。
2. 呼叫本幀開始前已存在之 Overlay 的 `OnUpdate()`。
3. 依 spawn 順序檢查每一組位於相同有效格子的物件，呼叫雙方 `OnCollide()`。
4. 複製物件指標並依 z-index 穩定排序，再呼叫 `Render()`。
5. 繪製本幀開始前已存在的 Overlay。
6. 套用延後的 spawn 與 destroy。

z-index 只用於第三階段，不會重新排列引擎保存的物件，因此不影響更新與碰撞順序。相同 z-index 的物件維持原始 spawn 順序。

## 隱藏與刪除

`set_visible(false)` 適合暫時停用畫面與碰撞。物件仍留在引擎中並執行 `OnUpdate()`，因此可以自行計時後再次顯示。

`Destroy(object)` 適合永久移除物件。在主迴圈內呼叫時，物件立即停止後續更新、碰撞與繪製，實際 delete 延後到幀末。傳入 `nullptr` 或不屬於此引擎的指標不會產生效果。

```cpp
void Collect(GridObject* self, GridObject* other) {
    if (other->tag() == "player") {
        self->engine()->Destroy(self);
        return;  // self 已排定刪除，不再讀取它。
    }
}
```

呼叫 `Destroy()` 後應立即停止使用該借用指標，即使記憶體到幀末才釋放。`ClearObjects()` 使用相同規則移除所有目前與待加入物件，但不影響 Overlay 或已載入素材。

## 碰撞資格

物件必須可見，且 x、y 位於引擎網格內，才會參與碰撞。隱藏、已排定刪除或位於地圖外的物件會被忽略。碰撞以座標完全相同為準，不檢查素材透明區域或像素邊界。

三個以上物件位於同一格時，引擎檢查每一組配對。callback 將物件隱藏、移出地圖或 destroy 後，該物件立即停止參與本幀後續碰撞；在有效範圍內移動則會影響尚未檢查的配對。已開始處理的一組碰撞仍會通知雙方，除非其中一方失去碰撞資格。碰撞規則不受 z-index 影響。

## 引擎結束

GridEngine 解構時，會依序釋放 GridObject、Overlay、raylib texture，最後關閉視窗。Texture 必須在 OpenGL context 關閉前釋放，因此不應把引擎管理的 texture 或物件延長到引擎生命週期之外。

最直接的安全做法是在 `main()` 建立區域 GridEngine，將所有動態物件立即交給 `Spawn()` 或 `AddOverlay()`，並且不自行 delete 借用指標。視窗關閉、`Run()` 回傳後，離開 `main()` 作用域即可完成清理。
