# 生命週期與所有權

前面章節已經用 `addObject()` 建立物件、透過 `remove()` 吃掉豆子，也曾把迷宮與玩家的 Handler 存在全域，讓其他物件的函式查詢；這些操作看似分散，背後其實都依賴同一份生命週期契約。Engine 必須先確定誰擁有遊戲內容、Handler 何時失效，再決定一幀進行到一半時能在什麼時候安全地加入或移除內容，否則遊戲程式即使偶爾能執行，也可能在某些時機出現難以追查的錯誤。

```text
game.addObject() / addMaze() / addTextOverlay() ...
       ↓
Engine 建立並保存內容，回傳 Handler
       ↓
Init 執行一次
       ↓
每幀 Update、碰撞與繪製
       ↓
remove() / clearObjects() / clearOverlays() / Engine 結束
       ↓
內容被釋放，所有 Handler 的 exists() 變成 false
```

本章因此先從所有權開始，釐清 Engine 與 Handler 各自負責什麼、Handler 在哪些事件後失效，接著建立每幀更新、碰撞與繪製的完整順序，最後才說明執行期間的新增與刪除如何生效。這個順序讓「Handler 失效後不能再使用」不只是孤立規定，也讓「新物件下一幀才開始運作」能從 Engine 的每幀流程自然推導出來。

這裡也是整份文件唯一完整列出精確時序的地方；前面章節只在需要時連到本章，而不各自重述一套簡化版本，因此當碰撞中移除物件、更新中建立敵人，或跨關卡保存 Handler 時，都應以本章契約為準。

## 完成本章後

- 能說明 Engine 擁有哪些內容，以及 Handler 在哪些事件後失效。
- 能依序列出一幀中的更新、碰撞、Overlay 更新、繪製與初始化。
- 能預測在更新或碰撞期間建立、移除內容時，變更會在哪一個時點生效。

[GameEngine 與 Handler 的壽命](01-ownership.md){ .md-button .md-button--primary }
