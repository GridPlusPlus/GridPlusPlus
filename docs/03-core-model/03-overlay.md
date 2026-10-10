# Overlay

前兩節已經說明世界中的物件如何由 Engine 建立並逐幀運作，但一個完整遊戲還需要顯示分數、倒數與操作按鈕。這些內容出現在遊戲畫面上，卻不屬於遊戲世界：分數不是站在某一格的物件，重新開始按鈕也不應因為玩家走到同一位置而碰撞。`Overlay` 專門表示這類畫面資訊，讓介面排版不必混入 GridObject 的網格規則。

## 與 GridObject 的差別

GridObject 與 Overlay 都以視窗左上角為座標原點，但兩者使用不同單位。GridObject 的位置代表第幾欄、第幾列，會受到網格規則與碰撞影響；Overlay 則直接使用像素座標，不受格子大小影響。例如文字位於 `(12, 12)`，意思是距離視窗左方與上方各 12 像素，而不是位於第 12 欄、第 12 列。

| | GridObject | Overlay |
| --- | --- | --- |
| 表示 | 世界中的實體 | 世界上方的畫面資訊 |
| 座標 | 網格座標 | 像素座標 |
| 同格碰撞 | 參與 | 不參與 |
| 繪製順序 | 依 layer | 永遠在 GridObject 之後 |

表中的差異最後仍回到內容在遊戲中的意義。固定在畫面角落、只提示瞄準方向的準星可以是 Overlay，因為它不占據地圖；如果一座控制台會占據某一格並阻擋玩家通過，它就應該是 GridObject。外觀看起來同樣像介面元件，是否參與世界規則才是真正的分類依據。

## 用文字顯示狀態

打地鼠目前只需要在畫面左上角顯示分數，因此可以直接使用 Grid++ 內建的文字 Overlay。以下程式建立一行文字，並透過回傳的 Handler 更新顯示內容：

```cpp title="main() 節錄：加入並更新分數文字"
gridpp::Overlay score_label = game.addTextOverlay("Score: 0", 12, 12, 24, BLACK);

score_label.setText("Score: 10");
```

`addTextOverlay()` 的參數依序是文字、像素 x、像素 y、字體大小與顏色；字體大小預設為 20，顏色預設為黑色，所以只寫前三個參數也可以。回傳的 `score_label` 和 GridObject 一樣是 Handler：文字本身由 Engine 保存，程式只透過它修改內容。

需要特別區分的是，文字 Overlay 保存的是「目前要顯示的文字」，而不是計分規則本身。玩家點中地鼠時，遊戲程式先修改整數 `score`，再把新數值轉成文字交給 Overlay。如此一來，分數如何增加仍由遊戲規則決定，Overlay 只負責呈現結果；日後即使更換畫面配置，也不必重寫計分邏輯。

## 三種 Overlay

除了文字，Grid++ 還內建圖片與按鈕兩種 Overlay。三種內容共用同一個 `Overlay` 型別，也共用 `setPosition()`、`move()`、`show()`、`hide()` 與 `remove()` 等操作：

| 建立函式 | 內容 | 專用操作 |
| --- | --- | --- |
| `addTextOverlay(text, x, y, size, color)` | 單行文字 | `setText()`、`text()` |
| `addButton(text, x, y, width, height, click)` | 可點擊的按鈕 | `setText()`、`setClickFunction()` |
| `addOverlay(image)` | 32×32 像素的圖片 | `setImage()`、`image()` |

在錯誤種類上呼叫專用操作會丟出清楚的錯誤訊息，例如對圖片 Overlay 呼叫 `setText()`。

## Overlay 也能有每幀行為

文字只需在內容改變時接收新值，但有些介面元件也需要每幀處理狀態。和 GridObject 一樣，建立 Overlay 時可以另外交給它 Init 與 Update 函式；按鈕則有專屬的 Click 函式，在玩家用滑鼠左鍵點擊它時執行。第三章只會使用文字；等第 8 章的完整遊戲真的需要互動介面時，再介紹按鈕如何依遊戲階段顯示或隱藏。

Overlay 沒有 GridObject 的 layer 或同格碰撞，因為它不屬於網格世界。如果一項內容開始需要這些能力，問題通常不是替 Overlay 增加更多設定，而是重新判斷它是否其實應該成為 GridObject。至於執行期間新增 Overlay 的時機與其他生命週期邊界，第 9 章會在出現相應需求後集中說明。

至此，製作第一個遊戲所需的角色已經各就各位：GridObject 表示地鼠，Update 函式描述地鼠每幀如何改變，GameEngine 推進整個遊戲，文字 Overlay 則把分數與時間呈現在世界上方。不過，地鼠何時移動、玩家點了哪一格，以及倒數經過多久，仍需要讀取輸入與時間；下一節先整理這些遊戲規則會直接用到的函式，再把它們組合成完整的打地鼠程式。

[讀取輸入、時間與亂數](04-input-and-time.md){ .md-button .md-button--primary }

完整介面見 [Overlay API](../api/classgridpp_1_1Overlay.md)。
