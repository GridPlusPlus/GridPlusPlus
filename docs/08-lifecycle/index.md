# 生命週期

學生不負責配置或釋放實體；`Game` 保存它建立的所有物件、迷宮與 Overlay。程式只保留 Handler，
並透過 `exists()` 判斷實體是否仍在遊戲中。

```text
game.addObject / addMaze / addOverlay
                ↓
          回傳 Handler
                ↓
       Init → 每幀 Update
                ↓
      remove / clear / Game 結束
                ↓
        Handler 的 exists() 為 false
```

本章說明 Handler 的複製語意、每幀順序，以及 callback 中安全新增與移除內容的規則。
