# GridObject

上一節先將遊戲內容分成 GridObject 與 Overlay，現在從真正存在於網格世界中的物件開始。玩家、敵人、道具和地鼠的規則雖然不同，卻都必須保存位置、決定外觀，並在輪到自己時執行行為。`GridObject` 將這些共同能力整理成一致的介面，使 Engine 不必知道每個角色的具體規則，也能以相同流程更新、碰撞和繪製它們。

## 從一個靜態物件開始

物件不能單獨存在，必須由 Engine 建立，才會參與每一幀的更新與繪製。以下呼叫要求 `game` 建立一隻地鼠，並回傳一個可以用來操作它的 `GridObject`：

```cpp title="main() 節錄：建立靜態物件"
gridpp::GridObject mole = game.addObject("mole");
mole.setPosition(3, 2);
```

`addObject()` 的第一個參數是素材名稱，也就是之後要用哪一張圖片畫出這個物件。新物件一開始位於 `(0, 0)`，所以第二行把它移到 `(3, 2)`。若 Engine 尚未載入名為 `mole` 的素材，畫面會先以紅色方塊代替；這個結果仍足以驗證物件確實已被加入，而且網格位置符合預期。

`mole` 是上一節提到的 Handler，也就是地鼠的「遙控器」。地鼠本身由 Engine 保存，`mole` 只是讓程式可以找到並修改它；即使 `mole` 這個變數離開作用域，地鼠仍會留在遊戲裡。這項關係將在 Engine 小節接著說明，現在只要先記住：`mole` 是找到物件的方式，不是物件本身。

## 位置與移動

建立物件後，最直接的操作就是改變它的位置。`x()` 和 `y()` 讀取目前座標，適合判斷物件位於哪一格；`setPosition(x, y)` 指定新的絕對位置，而 `move(dx, dy)` 則從現有位置做相對移動。下面的程式先把地鼠放到 `(4, 5)`，再向左移動一格，因此最後讀到的位置是 `(3, 5)`。

```cpp title="物件操作節錄：讀寫位置"
mole.setPosition(4, 5);
mole.move(-1, 0);  // 現在位於 (3, 5)

int column = mole.x();
int row = mole.y();
```

Grid++ 不會自動阻止物件離開地圖。遊戲規則應先根據 `game.cols()`、`game.rows()` 或迷宮判斷目標位置是否合法。暫時不想顯示或碰撞時，可以呼叫 `hide()`，之後再用 `show()` 讓它重新出現；隱藏的物件仍會繼續執行更新行為。

## 把行為交給物件

直接修改座標只能讓物件在一開始出現在指定位置；若要讓它在遊戲執行期間持續行動，就必須把移動規則交給每幀流程。Grid++ 的做法是：先寫一個普通函式描述「輪到這個物件時要做什麼」，再把函式名稱交給 `addObject()`。

```cpp title="main.cpp 節錄：更新函式放在 main() 前，建立物件放在 main() 內"
void UpdateMole(gridpp::GameEngine game, gridpp::GridObject self) {
    self.move(1, 0);
}

// 放在 main() 內：
gridpp::GridObject mole = game.addObject("mole", nullptr, UpdateMole);
```

`addObject()` 在素材名稱之後可以接收三個函式，依序稱為 **Init**、**Update** 與 **Collide**：Init 在遊戲開始時執行一次，Update 每一幀執行一次，Collide 在物件和其他物件同格時執行。地鼠目前只需要 Update，所以 Init 的位置傳入 `nullptr`，表示「這裡沒有函式」，而最後的 Collide 可以直接省略。

`UpdateMole` 不是由 `main()` 直接呼叫的。每當一幀輪到地鼠更新時，Engine 才呼叫 `UpdateMole`，並傳入兩個參數：`game` 代表這個物件所在的遊戲，`self` 則是這次正在更新之物件的 Handler。因為 `self` 由 Engine 提供，同一個函式可以交給很多個物件使用，每次呼叫時 `self` 都會指向當下輪到的那一個。

這種「先把函式交給別人，等事件發生時再由對方呼叫」的寫法稱為**回呼函式（callback）**。Pac-Man 教學中的 `MovePacman`、`ChasePacman` 是 Update，第 4 步的 `PacmanHit` 則是 Collide，它們都是以這種方式交給物件的。它只描述物件的行為，並不負責維持整個遊戲迴圈。

## 每幀執行不等於每幀移動

由於上述 `UpdateMole` 每次被呼叫都向右移動一格，而 Engine 一秒大約更新 60 次，畫面中的地鼠會移動得快到難以看清，而且很快就跑出視窗。真正的打地鼠不能把「每幀執行更新函式」直接等同於「每幀都要移動」，而要另外保存下一次允許移動的時間。這個需求也引出了另一個問題：除了 GridObject 已有的座標之外，遊戲新增的狀態究竟應該放在哪裡？

## 先辨認狀態描述誰

判斷狀態的歸屬，可以先問「這份資料描述誰」。座標、素材、方向與顯示狀態描述單一物件，因此已經存於 GridObject；分數和整局倒數描述整場遊戲，應由遊戲流程共同管理。第 4 章會先以一隻地鼠練習這項區分，第 6 章再處理多個物件需要各自保存額外資料的情況。

## 其他外觀設定

`setImage()` 可切換素材；`setDirection()`、`setColor()` 與 `setLayer()` 分別控制旋轉、顏色與繪製層級。這些設定不改變物件的座標或遊戲規則，將在第 7 章搭配素材與繪製順序完整說明。

[接著理解 GameEngine](02-game-engine.md){ .md-button .md-button--primary }

完整成員列表見 [GridObject API](../api/classgridpp_1_1GridObject.md)。
