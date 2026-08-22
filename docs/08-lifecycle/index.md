# 生命週期與所有權

GridObject 從建立到釋放會經過 Engine 控制的生命週期。這項控制同時處理兩個問題：確保物件只由一個負責者釋放，以及避免 callback 執行期間修改物件容器，導致迭代器或借用指標失效。

```text
new object
    |
    v
Spawn() transfers ownership
    |
    v
OnSpawn() runs once
    |
    v
Update, collision, and rendering
    |
    v
Destroy() or ClearObjects()
    |
    v
Engine deletes the object
```

遊戲開始前的 Spawn 可以立即完成。主迴圈內的 Spawn 與 Destroy 則延後套用，讓本幀的更新、碰撞和繪製看到一致的物件集合。Overlay 有獨立容器與較簡單的生命週期，但仍由 Engine 接管所有權。

本章精確說明執行期間新增與刪除的時機、raw pointer 的所有權契約，以及每一幀的完整順序。這些規則在一般遊戲程式中由 Engine 自動維持；保存借用指標或從 callback 改變關卡時，才需要直接依賴細節。
