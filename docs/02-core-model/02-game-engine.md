# GameEngine

前一節說明了 GridObject 如何保存位置與行為，但物件不會自行輪流更新，也不知道何時應該重畫畫面。`GameEngine` 補上這個缺少的執行環境：它建立視窗、保存所有遊戲內容，並反覆安排更新、碰撞和繪製。遊戲程式決定角色如何移動、碰撞代表什麼；Engine 則保證這些規則在每一幀按照固定順序被呼叫。

這項分工讓不同遊戲共用相同的基礎設施。打地鼠與 Pac-Man 的內容和勝負條件完全不同，卻都需要視窗、主迴圈與資源清理；將這些重複工作集中在 Engine 後，遊戲程式便能專注於自身規則，而不必為每個專案重新撰寫遊戲迴圈。

## 建立與設定世界

Engine 必須先知道世界與視窗的大小，才能把網格座標轉成實際畫面。建構子的三個參數依序是欄數、列數與單格像素大小，建立後還可以設定背景顏色與是否顯示參考格線：

```cpp title="main() 節錄：建立並設定世界"
gridpp::GameEngine game(8, 8, 64);
game.setBackgroundColor(BEIGE);
game.showGrid(true);
```

這會建立 8 欄、8 列、每格 64 像素的世界，因此視窗寬高都是 `8 × 64 = 512` 像素。`BEIGE` 是 raylib 提供的顏色常數，其他常見顏色還有 `BLACK`、`WHITE`、`RED` 與 `SKYBLUE`；沒有設定時，背景是接近白色的 `RAYWHITE`。`cols()`、`rows()` 與 `gridSize()` 可讀取這三個設定。格線只協助辨認座標，不會建立牆壁或限制移動。

## 由 Engine 建立所有內容

這些設定只建立了世界的範圍與外觀，還沒有加入任何遊戲內容。Grid++ 的所有內容都透過 Engine 的 `add...()` 函式建立，每個函式都回傳對應的 Handler：

| 函式 | 建立的內容 | 回傳 | 說明位置 |
| --- | --- | --- | --- |
| `addObject()` | 使用圖片的網格物件 | `GridObject` | 本章、第 4 章 |
| `addSquare()`、`addCircle()` 等 | 不需圖片的基本圖形 | `GridObject` | 第 6 章 |
| `addTextOverlay()`、`addButton()`、`addOverlay()` | 文字、按鈕與圖片介面 | `Overlay` | 下一節、第 7 章 |
| `addMaze()` | 整張地圖的牆面 | `Maze` | 第 7 章 |

一般程式只需要一個 Engine，接著在 `run()` 前建立初始內容。以下範例加入一隻靜止的地鼠後，才讓世界開始運行：

```cpp title="完整 main()：建立物件後啟動 Engine"
int main() {
    gridpp::GameEngine game(8, 8, 64);
    game.showGrid(true);

    gridpp::GridObject mole = game.addObject("mole");
    mole.setPosition(3, 2);

    game.run();
    return 0;
}
```

## Engine 與遊戲規則的邊界

Engine 知道要檢查兩個物件是否同格，卻不知道同格代表得分、受傷或過關；使用者提供的 Collide 函式才賦予它意義。Engine 知道如何把素材畫到指定格子，卻不知道哪個素材名稱代表玩家。

從這兩個例子可以看出，Engine 提供的是各種網格遊戲都需要的固定流程，使用者程式填入的則是每個遊戲不同的內容與規則：

| Engine 負責 | 遊戲程式負責 |
| --- | --- |
| 建立與關閉視窗 | 決定世界大小與背景 |
| 逐幀呼叫物件的 Update | 定義移動、計分與結束條件 |
| 檢查同格座標 | 解釋碰撞代表什麼 |
| 依狀態重新繪製 | 選擇素材與畫面內容 |
| 保存並釋放所有內容 | 只透過 Handler 操作內容 |

## `run()` 前後的界線

上述分工也形成 `run()` 前後的明確界線。桌面程式在呼叫前建立世界、載入素材，並加入一開始就要存在的物件與 Overlay；呼叫之後，Engine 先為所有已建立的內容各執行一次 Init，接著才持續執行遊戲幀，直到使用者關閉視窗。典型的初始化順序如下：

```cpp title="main() 節錄：run() 前完成初始化"
gridpp::GameEngine game(8, 8, 64);
game.addObject("mole", nullptr, UpdateMole);
game.addTextOverlay("Score: 0", 12, 12, 24);
game.run();
```

這條界線不表示遊戲開始後永遠不能改變內容。Update 或 Collide 函式仍可在執行期間建立敵人、移除道具或切換關卡，只是 Engine 會按照明確規則安排新內容何時開始運作。這些情況尚未出現在打地鼠中，第 8 章再配合實際新增與刪除的需求完整說明。

## 在函式中取得 Engine

每個回呼函式的第一個參數都是 `GameEngine game`。這讓物件的行為可以查詢世界大小、讀取輸入，或在執行期間建立新內容：

```cpp title="更新函式節錄：透過 game 查詢世界"
void UpdateMole(gridpp::GameEngine game, gridpp::GridObject self) {
    if (self.x() < game.cols() - 1) self.move(1, 0);
}
```

`GameEngine` 和 GridObject 一樣是 Handler：Engine 呼叫函式時傳入的 `game`，和 `main()` 中的 `game` 操作的是同一個遊戲，而不是另外建立一個新視窗。因此函式參數可以直接寫成 `GameEngine game`，不需要參考（`&`）或指標語法。

## 結束時的清理

Engine 擁有所有由它建立的內容。當 `main()` 結束、區域變數 `game` 離開作用域時，Engine 會釋放物件、Overlay 和素材，再關閉視窗。

```cpp title="完整 main()：離開作用域時清理"
int main() {
    gridpp::GameEngine game(8, 8, 64);
    game.run();
    return 0;
}  // game 在這裡結束並清理資源
```

由於 Engine 代表整個遊戲的執行環境，重新開始一局通常不等於再建立另一個 Engine。較直接的做法是保留原本的視窗和主迴圈，只把物件位置、分數與倒數等遊戲狀態恢復為初始值。第三章的打地鼠將採用這種方式：時間結束後仍保留同一隻地鼠，按下 R 才重新顯示它並開始下一局。

[最後區分 Overlay](03-overlay.md){ .md-button .md-button--primary }

尺寸限制、素材載入、執行期間增刪與完整函式簽名分別見後續主題章及 [GameEngine API](../api/classgridpp_1_1GameEngine.md)。
