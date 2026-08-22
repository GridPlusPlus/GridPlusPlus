# 開始使用

Grid++ 是一個 C++17 的 header-only 網格遊戲函式庫。它建立在 raylib 之上，負責視窗、遊戲迴圈、網格座標、遊戲物件、同格碰撞與繪製順序。你只需要把 Grid++ 的 header 放在專案中，並在編譯時連結 raylib。

本書假設你已經使用過變數、條件判斷、迴圈、函式、陣列與指標。我們會在需要時介紹 class、繼承與成員變數，但不會逐一解釋基礎 C++ 語法。

本章會完成兩件事：安裝 Grid++ 唯一的外部依賴 raylib，然後編譯一個最小的 Grid++ 程式。完成後，畫面上會出現 8×8 的空白網格。第 2 章將從這個程式開始製作打地鼠。

## Grid++ 專案包含什麼

一般遊戲只需要引入 `GridPlusPlus.h`。這個主 header 會帶入三個核心部分：

- `GridEngine` 建立並管理遊戲世界。
- `GridObject` 表示存在網格中的玩家、敵人與道具。
- `Overlay` 表示以像素定位的文字、按鈕與其他畫面資訊。

迷宮和基本圖形是選用模組，使用時才額外引入 `GridMaze.h` 或 `GridShapes.h`。我們會先使用核心 header 完成打地鼠，再在後面的章節加入這些模組。

[安裝 raylib](01-installation.md){ .md-button .md-button--primary }
