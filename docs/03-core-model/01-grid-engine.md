# GridEngine

前一節說明了 GridObject 如何保存位置與行為，但物件不會自行輪流更新，也不知道何時應該重畫畫面。`GridEngine` 補上這個缺少的執行環境：它建立視窗、保存已加入的物件與 Overlay，並反覆安排更新、碰撞和繪製。遊戲程式決定角色如何移動、碰撞代表什麼；Engine 則保證這些規則在每一幀按照固定順序被呼叫。

這項分工讓不同遊戲共用相同的基礎設施。打地鼠與 Pacman 的內容和勝負條件完全不同，卻都需要視窗、主迴圈與資源清理；將這些重複工作集中在 Engine 後，遊戲程式便能專注於自身規則，而不必為每個專案重新撰寫 raylib while-loop。

## 建立與設定世界

Engine 必須先知道世界與視窗的大小，才能把網格座標轉成實際畫面。建構子的三個參數依序是欄數、列數與單格像素大小，建立後還可以設定背景顏色與是否顯示參考格線：

```cpp
gridpp::GridEngine game(8, 8, 64);
game.set_background_color(BEIGE);
game.set_show_grid(true);
```

這會建立 8 欄、8 列、每格 64 像素的世界，因此視窗寬高都是 `8 × 64 = 512` 像素。`cols()`、`rows()` 與 `grid_size()` 可讀取這三個設定。格線只協助辨認座標，不會建立牆壁或限制移動。

這些設定只建立了世界的範圍與外觀，還沒有加入任何遊戲內容。一般程式只需要一個 Engine，接著在 `Run()` 前生成初始物件；以下範例加入一隻靜止的地鼠後，才讓世界開始運行：

```cpp
int main() {
    gridpp::GridEngine game(8, 8, 64);
    game.set_show_grid(true);

    game.Spawn("mole", 3, 2, nullptr);

    game.Run();
}
```

## Engine 與遊戲規則的邊界

Engine 知道要檢查兩個物件是否同格，卻不知道同格代表得分、受傷或過關；使用者提供的碰撞函式（`CollideFn`）才賦予它意義。Engine 知道如何把素材畫到指定格子，卻不知道哪個素材名稱代表玩家。

從這兩個例子可以看出，Engine 提供的是各種網格遊戲都需要的固定流程，使用者程式填入的則是每個遊戲不同的內容與規則：

| Engine 負責 | 遊戲程式負責 |
| --- | --- |
| 建立與清除視窗 | 決定世界大小與背景 |
| 逐幀呼叫物件行為 | 定義移動、計分與結束條件 |
| 檢查同格座標 | 解釋碰撞代表什麼 |
| 依狀態重新繪製 | 選擇素材與畫面內容 |
| 釋放加入的內容 | 不自行刪除借用指標 |

## `Run()` 前後的界線

上述分工也形成 `Run()` 前後的明確界線。桌面程式在呼叫前建立世界、載入素材，並加入一開始就要存在的物件與 Overlay；呼叫之後，Engine 便持續執行遊戲幀，直到使用者關閉視窗。典型的初始化順序如下：

```cpp
gridpp::GridEngine game(8, 8, 64);
game.Spawn("mole", 0, 0, UpdateMole);
game.AddOverlay(new gridpp::Label("Score: 0", 12, 12));
game.Run();
```

這條界線不表示遊戲開始後永遠不能改變內容。`UpdateFn` 或 `CollideFn` 仍可在執行期間生成敵人、刪除道具或切換關卡，只是 Engine 為了避免在巡覽容器時同時改動它，會按照明確規則延後套用部分操作。這些情況尚未出現在打地鼠中，第 8 章再配合實際新增與刪除的需求完整說明。

## 結束時的清理

Engine 取得所有加入內容的所有權。當 `main()` 結束、區域變數 `game` 離開作用域時，Engine 會釋放物件、Overlay 和素材，再關閉視窗。

```cpp
int main() {
    gridpp::GridEngine game(8, 8, 64);
    game.Run();
}  // game 在這裡解構並清理資源
```

由於 Engine 代表整個遊戲的執行環境，重新開始一局通常不等於再建立另一個 Engine。較直接的做法是保留原本的視窗和主迴圈，只把物件位置、分數與倒數等遊戲狀態恢復為初始值。第三章的打地鼠將採用這種方式：時間結束後仍保留同一隻地鼠，按下 R 才重新顯示它並開始下一局。

[最後區分 Overlay](03-overlay.md){ .md-button .md-button--primary }

尺寸限制、素材載入、`DrawCell()`、執行期間增刪與完整函式簽名分別見後續主題章及 [GridEngine API](../api/classgridpp_1_1_grid_engine.md)。
