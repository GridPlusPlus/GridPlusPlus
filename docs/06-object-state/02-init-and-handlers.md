# Init 與 Handler

上一節讓每隻地鼠保存自己的計時器，但初始值是在 `main()` 中逐一 `set()` 的。有些初始值可以在建立物件時立刻決定，有些卻必須等遊戲真正開始才有意義；本節先區分這兩種情況，再進一步說明程式手上的 Handler 在複製、移除之後會有什麼行為。

## 建立時設定，還是在 Init 中設定

建立物件之後、呼叫 `run()` 之前，程式可以立刻設定不依賴遊戲時間的資料，例如位置、顏色、種類與移動方向：

```cpp title="main() 節錄：建立後立即設定"
GridObject ghost = game.addObject("ghost", InitGhost, MoveGhost);
ghost.setPosition(5, 3);
ghost.set("type", "ghost");
ghost.set("direction", 0);
```

「半秒後才第一次移動」這類設定則依賴遊戲開始的時刻。`main()` 在 `run()` 之前可能還要載入素材、讀取地圖，這些工作花費的時間無法預先得知，因此這類初始值應交給 Init：

```cpp title="main.cpp 節錄：Init 處理依賴時間的初始值"
void InitGhost(GameEngine game, GridObject self) {
    self.set("nextMove", game.time() + 0.5);
}
```

Init 會在 `run()` 開始、第一幀之前執行一次，也會在遊戲進行中新建立的物件開始運作前執行一次。因此無論鬼是在開場時建立，還是在第 8 章按下 Restart 後重新建立，`nextMove` 都會以「它真正開始運作的時刻」為基準。

## Handler 是遙控器，不是物件本身

`addObject()` 回傳的 `GridObject` 是一個 Handler。複製 Handler 就像多拿一支遙控器，兩支都控制同一台電視，所以透過任何一支修改，另一支都看得到結果：

```cpp title="Handler 複製示意"
GridObject pacman = game.addObject("pacman");
pacman.setPosition(2, 1);

GridObject same_pacman = pacman;  // 同一個物件的另一個 Handler
same_pacman.move(1, 0);           // pacman.x() 現在也是 3
```

這也是為什麼 Grid++ 的函式可以直接把 `GridObject` 寫成參數，而不需要參考（`&`）或指標：Engine 傳給 Update 的 `self`、程式保存在全域的 `pacman`，以及上例的 `same_pacman`，操作的全都是同一個實體。

需要另一個真正獨立的物件時，使用 `deepCopy()`。它會建立一個新物件，複製原物件的位置、外觀、`set()` 保存的資料與三個行為函式，並回傳指向新物件的 Handler：

```cpp title="deepCopy 示意"
GridObject snapshot = pacman.deepCopy();  // 新的獨立物件
snapshot.move(0, 1);                      // 只有 snapshot 移動，pacman 不變
```

複本和其他新建立的內容一樣，要到下一次初始化時才開始運作。若原物件已經執行過 Init，複本會一併保留「已經初始化」的狀態，不會再執行一次 Init；若是在 `run()` 之前複製，兩者都會在 `run()` 開始時各自執行 Init。

## Handler 失效之後

物件被移除後，所有指向它的 Handler 都會同時失效，不論這些 Handler 是複製了幾份、保存在哪裡。`exists()` 可以檢查 Handler 是否仍指向存在的物件：

```cpp title="檢查 Handler 是否有效"
GridObject alias = ghost;
ghost.remove();

if (!alias.exists()) {
    // alias 與 ghost 都已失效。
}
```

對失效的 Handler 呼叫 `exists()` 以外的函式，例如 `alias.x()`，會丟出 `Grid++ Error: GridObject no longer exists` 例外；程式沒有處理這個例外時，會印出訊息並結束。這個訊息明確指出了問題所在，比起使用已釋放記憶體時可能出現的隨機數值或當機，更容易追查。全域宣告、尚未指定的 Handler（例如 `GridObject pacman;`）同樣不指向任何物件，使用前必須先由 `addObject()` 指定。

Pac-Man 的鬼每幀都會讀取玩家座標，而玩家可能在重新開始時被清除並重建。因此鬼的 Update 開頭先檢查 `pacman.exists()`，確定玩家仍在遊戲中才繼續追蹤。

## 本節小結

- 不依賴時間的初始值在建立後立即設定，依賴遊戲開始時刻的初始值交給 Init。
- 複製 Handler 只會多一個操作同一物件的入口；`deepCopy()` 才建立獨立物件。
- 物件移除後，所有指向它的 Handler 都失效；`exists()` 可以安全檢查。
- 對失效 Handler 呼叫其他函式會得到明確的錯誤訊息。

[素材與繪製](../07-drawing/index.md){ .md-button .md-button--primary }
