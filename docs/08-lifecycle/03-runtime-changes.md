# 執行期間新增與刪除

遊戲執行期間常會產生子彈、敵人與道具，也會在碰撞後移除物件。如果新內容在一幀進行到一半時立刻加入，它是否參與本幀剩下的碰撞、會不會被畫出來，就會取決於它在清單中的位置，讓結果難以預測。Grid++ 因此為新增與刪除訂下固定的時點。

## 執行期間建立內容

回呼函式中可以直接呼叫 `game.addObject()` 等建立函式，例如敵人產生器每隔三秒建立一個新敵人：

```cpp title="更新函式節錄：定時建立敵人"
void UpdateSpawner(GameEngine game, GridObject self) {
    double next_spawn = 0.0;
    self.get("nextSpawn", next_spawn);
    if (game.time() < next_spawn) return;

    GridObject enemy = game.addObject("enemy", InitEnemy, MoveEnemy);
    enemy.setPosition(self.x(), self.y());
    self.set("nextSpawn", game.time() + 3.0);
}
```

新敵人會立刻得到有效的 Handler，因此建立後可以馬上設定位置、顏色或 `set()` 資料。但它還沒有開始運作：本幀剩下的更新、碰撞與繪製都不會處理它，幀末才執行它的 Init，下一幀起才開始 Update、碰撞與繪製。這個固定邊界讓建立時機不依賴產生器在清單中的位置。

幀末若有多個新內容，Engine 依實際建立順序執行 Init；Init 又建立的內容會接在同一次幀末處理的下一批完成 Init。`deepCopy()` 產生的複本也遵守相同規則。

## 移除內容

`remove()` 會立即釋放內容。被移除的物件不再參與本幀後續的更新、碰撞或繪製，所有指向它的 Handler 也立刻失效。在自己的回呼函式中移除自己時，`remove()` 應是最後一個操作：

```cpp title="碰撞函式節錄：碰撞後移除自己"
void CollectPellet(GameEngine, GridObject self, GridObject other) {
    std::string type;
    other.get("type", type);
    if (type != "player") return;

    ++score;
    self.remove();
    // 不要再使用 self。
}
```

回呼函式也可以移除其他物件，例如炸彈移除周圍的敵人；規則相同，被移除者立即失效。移除後若還呼叫 `self.x()` 等函式，會得到 `GridObject no longer exists` 錯誤，提醒程式在移除後繼續使用了失效的 Handler。

Overlay 與 Maze 也有 `remove()`，規則和 GridObject 一樣。

## 隱藏或移除

`hide()` 適合之後還會出現的物件。它會立即停止繪製與碰撞，物件仍每幀更新，`set()` 保存的資料也都保留。打地鼠的倒數結束使用這個方式，按 R 重設時再 `show()` 同一隻地鼠。

`remove()` 適合永久移除。若後續不再需要物件的資料，移除可以讓遊戲只處理仍有意義的內容；若物件需要自行倒數並重新出現，`hide()` 則能保留它的狀態。

## 清除關卡

`clearObjects()` 立即移除目前所有 GridObject 與 Maze，包括本幀才建立、尚未開始運作的內容。Overlay 與素材不受影響，因此 Pac-Man 的 Restart 按鈕可以重建迷宮與角色，同時保留文字、按鈕和已載入的素材。

```cpp title="關卡重建函式節錄"
void BuildLevel(GameEngine game) {
    game.clearObjects();
    pellets_left = 0;
    paused = false;

    maze = game.addMaze(level.cols, level.rows);
    // 從已驗證的地圖建立新的一局。
}
```

清除之後，所有指向舊關卡的 Handler 都已失效；範例在同一個函式中立即把 `maze` 與 `player` 重新指定為新內容。對應地，`clearOverlays()` 會移除所有 Overlay，但保留網格內容與素材。

## 本節小結

- 執行期間建立的內容立刻有有效的 Handler，但在幀末 Init，下一幀才開始運作。
- `remove()` 立即釋放內容並讓 Handler 失效；移除自己後應立即結束函式。
- `hide()` 保留物件與資料，適合暫時隱藏；`remove()` 適合永久移除。
- `clearObjects()` 清除 GridObject 與 Maze，保留 Overlay 與素材；`clearOverlays()` 則相反。

[從本機成果到可重現的專案](../09-sharing/index.md){ .md-button .md-button--primary }
