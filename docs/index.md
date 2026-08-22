# Grid++

Grid++ 是用於製作網格遊戲的 C++17 函式庫。它提供視窗、遊戲迴圈、繪圖、物件生命週期與同格碰撞，遊戲程式只需定義物件和規則。

本教學假設讀者已熟悉變數、條件判斷、迴圈、函式、陣列與指標，不要求預先具備物件導向程式設計經驗。

## 文件內容

[Grid++ 總覽](01-overview.md)說明 `GridEngine`、`GridObject` 與 `Overlay` 的責任、彼此關係和主要 API。安裝章節接著建立可執行環境；後續章節依序深入 Engine、物件共通狀態、Overlay、callback、自訂類別、素材、基本圖形、迷宮與完整生命週期。

打地鼠與 Pacman 作為 API 的使用案例。打地鼠示範函式、指標與 `CallbackGridObject`；Pacman 說明多個物件需要分別保存狀態時，如何衍生 `GridObject`。範例用於解釋功能，不取代各類別的完整說明。

API 參考由公開 header 中的 Doxygen 註解自動產生，提供類別、函式、參數和例外的精確簽名。概念與操作方式應從編號章節閱讀；需要查詢單一函式時使用 API 參考。

[Grid++ 總覽](01-overview.md){ .md-button .md-button--primary }
