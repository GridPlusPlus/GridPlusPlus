# Overlay

前兩節已經說明世界中的物件如何加入 Engine 並逐幀運作，但一個完整遊戲還需要顯示分數、倒數與操作按鈕。這些內容出現在遊戲畫面上，卻不屬於遊戲世界：分數不是站在某一格的物件，重新開始按鈕也不應因為玩家走到同一位置而碰撞。`Overlay` 專門表示這類畫面資訊，讓介面排版不必混入 GridObject 的網格規則。

## 與 GridObject 的差別

GridObject 與 Overlay 都以視窗左上角為座標原點，但兩者使用不同單位。GridObject 的位置代表第幾欄、第幾列，會受到網格規則與碰撞影響；Overlay 則直接使用像素座標，不受 `grid_size` 影響。例如 `Label` 位於 `(12, 12)`，意思是距離視窗左方與上方各 12 像素，而不是位於第 12 欄、第 12 列。

| | GridObject | Overlay |
| --- | --- | --- |
| 表示 | 世界中的實體 | 世界上方的畫面資訊 |
| 座標 | 網格座標 | 像素座標 |
| 同格碰撞 | 參與 | 不參與 |
| 繪製順序 | 依 z-index | 永遠在 GridObject 之後 |

表中的差異最後仍回到內容在遊戲中的意義。固定在畫面角落、只提示瞄準方向的準星可以是 Overlay，因為它不占據地圖；如果一座控制台會占據某一格並阻擋玩家通過，它就應該是 GridObject。外觀看起來同樣像介面元件，是否參與世界規則才是真正的分類依據。

## 用 Label 顯示狀態

打地鼠目前只需要在畫面左上角顯示分數，因此可以直接使用 Grid++ 內建的 `Label`。以下程式先建立一行文字，接著把 Label 加入 Engine，最後透過保留下來的借用指標更新顯示內容：

```cpp title="main() 節錄：加入並更新分數標籤"
gridpp::Label* score_label = new gridpp::Label("Score: 0", 12, 12, 24, BLACK);
game.AddOverlay(score_label);

score_label->set_text("Score: 10");
```

建構子參數依序是文字、像素 x、像素 y、字體大小與顏色。呼叫 `AddOverlay()` 後，Engine 會像管理 GridObject 一樣取得 Label 的所有權；`score_label` 只負責讓程式找到並修改它，因此不可自行 `delete`。

需要特別區分的是，Label 保存的是「目前要顯示的文字」，而不是計分規則本身。玩家點中地鼠時，遊戲程式先修改整數 `score`，再把新數值轉成文字交給 Label。如此一來，分數如何增加仍由遊戲規則決定，Overlay 只負責呈現結果；日後即使更換畫面配置，也不必重寫計分邏輯。

## Overlay 也能有每幀行為

Label 本身只需在文字改變時接收新內容，但有些介面元件也需要每幀處理輸入，例如按鈕必須持續判斷滑鼠位置與點擊。第三章只會使用 Label；等後面的完整遊戲真的需要互動介面時，再介紹 Button 以及如何自訂 Overlay 的更新與繪製方式。

Overlay 沒有 GridObject 的 tag、visible、z-index 或同格碰撞，因為它不屬於網格世界。如果一項內容開始需要上述能力，問題通常不是替 Overlay 增加更多設定，而是重新判斷它是否其實應該成為 GridObject。至於執行期間新增 Overlay 的時機與其他生命週期邊界，第 8 章會在出現相應需求後集中說明。

至此，製作第一個遊戲所需的角色已經各就各位：GridObject 表示地鼠，`UpdateFn` 描述地鼠每幀如何改變，GridEngine 推進整個遊戲，Label 則把分數與時間呈現在世界上方。不過，地鼠何時移動、玩家點了哪一格，以及倒數經過多久，仍要透過 raylib 取得；下一節先整理這些遊戲規則會直接用到的函式，再把它們組合成完整的打地鼠程式。

[使用 raylib 讀取輸入與時間](04-raylib-input.md){ .md-button .md-button--primary }

完整介面見 [Overlay API](../api/classgridpp_1_1_overlay.md)、[Label API](../api/classgridpp_1_1_label.md)與 [Button API](../api/classgridpp_1_1_button.md)。
