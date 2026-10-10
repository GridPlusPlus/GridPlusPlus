# 每幀執行順序

GameEngine 的每一幀由更新、碰撞、Overlay 更新、繪製與初始化組成。順序決定同一幀中的狀態何時可見，例如 Update 修改座標後，本幀碰撞會使用新座標；修改 layer 後，本幀繪製也會使用新值。

```text
0. run() 開始時：為所有已建立的內容執行 Init

每一幀：
1. 依建立順序執行 GridObject 與 Maze 的 Update
2. 依建立順序檢查 GridObject 的同格碰撞
3. 依建立順序執行 Overlay 的 Update；可見的按鈕處理點擊
4. 清除背景並繪製格線
5. 依 layer 與建立順序繪製 GridObject 與 Maze
6. 依建立順序繪製可見的 Overlay
7. 為本幀新建立的內容執行 Init
```

## 開始之前

呼叫 `run()` 時，Engine 先為所有已建立的內容各執行一次 Init：先依建立順序處理 GridObject 與 Maze，再處理 Overlay。Init 若又建立新內容，這些內容會形成下一批，同樣依「世界內容、Overlay」的順序初始化；全部完成後，第一幀才開始。

## 更新

每幀開始時，Engine 先記下目前已運作的內容清單，本幀的更新、碰撞與繪製都只處理這份清單。接著依原始建立順序呼叫每個 GridObject 與 Maze 的 Update。隱藏的物件仍會更新；已被 `remove()` 的物件則會被跳過。前一個物件所做的狀態修改，可由後一個物件在同一更新階段讀取。

## 碰撞

全部更新完成後，Engine 依建立順序兩兩檢查 GridObject。一組物件同格時，先建立的一方先收到 Collide，後建立的一方接著收到；兩者都以對方作為 `other`。

每次呼叫 Collide 之後，Engine 都會重新確認雙方是否仍能碰撞。因此在 Collide 中隱藏、移出地圖或移除物件，會立即改變後續配對的資格：例如豆子在自己的 Collide 中 `remove()` 後，玩家就不會再收到與這顆豆子的碰撞通知。完整資格條件見[同格碰撞](../05-object-behavior/02-collisions.md)。

碰撞不依賴 layer。畫在最上方的物件沒有較高碰撞優先權；遊戲規則若需要優先順序，應在物件行為或共享狀態中明確表達。

## Overlay 更新與按鈕點擊

碰撞之後，Engine 依建立順序執行每個 Overlay 的 Update。隱藏的 Overlay 仍會更新；如果這個 Overlay 是按鈕，而且執行完 Update 後仍然可見，Engine 才檢查滑鼠是否位於按鈕上、左鍵是否剛被按下，成立時呼叫它的 Click 函式。

這個順序讓按鈕可以在同一幀中先決定自己是否應該出現，再處理點擊。Pac-Man 的 Restart 按鈕便是如此：遊戲進行中，它的 Update 先把自己隱藏，因此即使玩家點到那個位置，也不會誤觸重新開始。

## 繪製

Engine 清除背景並選擇性繪製格線，再把本幀的 GridObject 與 Maze 依 layer 由小到大排序繪製；相同 layer 維持建立順序，Maze 的 layer 固定為 0。排序只作用於繪製，不會改變下一幀的更新或碰撞順序。隱藏的物件不會被畫出來。

所有網格內容繪製完成後，Engine 依建立順序繪製可見的 Overlay。Overlay 沒有 layer，後加入者會畫在較早加入者上方。

## 幀末初始化

繪製完成後，Engine 為本幀新建立的內容執行 Init，規則與 `run()` 開始時相同。新內容從下一幀才進入更新、碰撞與繪製。

如果 Init 或任何回呼函式丟出例外，例外會離開 `run()`。呼叫端可以像 Pac-Man 的 `main()` 一樣用 `try`／`catch` 捕捉並印出錯誤；Engine 擁有的資源仍會在 GameEngine 結束時清理。

## 本節小結

- `run()` 先完成所有 Init，第一幀才開始。
- 更新後的座標和 layer 會影響同一幀的碰撞與繪製。
- 碰撞中的每一組由先建立的一方先處理，並在每次呼叫後重新檢查資格。
- Overlay 在碰撞之後更新；按鈕只有在 Update 後仍可見時才處理點擊。
- 本幀新建立的內容在幀末 Init，下一幀開始運作。

[執行期間新增與刪除](03-runtime-changes.md){ .md-button .md-button--primary }
