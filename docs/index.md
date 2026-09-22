# Grid++ 教學

Grid++ 讓初學者用少量 C++ 語法完成網格遊戲。公開操作集中在 `Game` 與 Handler：

```text
Game
├── ObjectHandler：角色、道具、基本圖形
├── MazeHandler：迷宮牆面
└── OverlayHandler：圖片、文字、按鈕
```

學生不需要建立或保存指標，也不需要理解所有權、繼承與虛擬函式。遊戲行為寫成普通函式，
再交給 `addObject()`、`addOverlay()` 或對應的 `set...Function()`。

建議依章節順序閱讀：先建立第一個視窗，再完成打地鼠，接著學會碰撞、每物件狀態、圖形、迷宮與
Pac-Man。第 8 章說明 Handler 的生命週期與執行順序。

[開始安裝](01-getting-started/01-installation.md){ .md-button .md-button--primary }
[第一個程式](01-getting-started/02-hello-gridpp.md){ .md-button }
