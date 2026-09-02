# Grid++

Grid++ 是用來建立網格遊戲的 C++17 函式庫。遊戲畫面被分成固定大小的格子；玩家、敵人和道具以整數座標位於格子中，Grid++ 負責視窗、主迴圈、物件更新、同格碰撞與繪製。它建立在 [raylib](https://www.raylib.com/) 上，並以 header-only library 的形式提供。

本文件以兩個可執行專案介紹 Grid++。第 2 章先使用普通函式完成打地鼠，讓 Engine、GridObject 與 Overlay 出現在實際程式中。後續章節分析這些元件的責任，並在多個物件需要分別保存狀態時引入自訂 class。第 7 章再以 Pacman 組合迷宮、素材、碰撞、Overlay 與遊戲狀態。

讀者應已熟悉 C/C++ 語言之變數、條件判斷、迴圈、函式、陣列與指標。本文件會說明 Grid++ 如何使用函式指標、繼承與虛擬函式，但不重複教授基礎 C++ 語法。

教學中的公開 API 會連結至自動產生的 [API 參考](api/index.md)。API 參考適合查詢完整簽名、參數與例外；編號章節說明各項功能在遊戲中的用途與組合方式。

[開始使用](01-getting-started/index.md){ .md-button .md-button--primary }
