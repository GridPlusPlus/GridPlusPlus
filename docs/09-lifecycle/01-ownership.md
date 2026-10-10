# GameEngine 與 Handler 的壽命

GameEngine 擁有所有由它建立的遊戲內容：GridObject、基本圖形、Maze 與 Overlay。「擁有」代表 Engine 保存內容本身，也負責在適當時機釋放它；遊戲程式拿到的 Handler 只用來操作仍存在的內容，本身不負責釋放任何東西。

這項契約讓學生程式完全不需要 `new`、`delete` 或位址語法：內容一律透過 `add...()` 建立，建立函式的回傳值就是 Handler。

```cpp title="建立內容並取得 Handler"
GameEngine game(8, 8, 64);
GridObject pacman = game.addObject("pacman");
Overlay score = game.addTextOverlay("Score: 0", 8, 8);
Maze maze = game.addMaze(8, 8);
```

## Handler 是可複製的存取入口

Handler 可以像 `int` 一樣複製、當作函式參數或存成全域變數。複製 Handler 不會複製內容，所有複本仍操作同一個實體；需要獨立副本時才使用 `deepCopy()`：

```cpp title="複製 Handler 與 deepCopy"
GridObject alias = pacman;
alias.move(1, 0);  // pacman 與 alias 指向同一實體

GridObject copy = pacman.deepCopy();
copy.move(1, 0);   // copy 是獨立實體，pacman 不受影響
```

程式可以同時保存很多個指向同一內容的 Handler，也可以隨時讓 Handler 變數離開作用域；這些都不會影響內容本身是否存在。

## Handler 的有效期限

下列事件會讓內容被釋放，所有指向它的 Handler 也會同時失效：

| 事件 | 失效的 Handler |
| --- | --- |
| `handler.remove()` | 指向該內容的所有 Handler |
| `game.clearObjects()` | 所有 GridObject 與 Maze 的 Handler |
| `game.clearOverlays()` | 所有 Overlay 的 Handler |
| GameEngine 結束 | 這個 Engine 的所有 Handler |

`exists()` 可以隨時安全地檢查 Handler 是否仍有效。對失效 Handler 呼叫其他函式會丟出 `std::runtime_error`，訊息會指出是哪一種內容已不存在，例如 `Grid++ Error: GridObject no longer exists`。這比起使用已釋放記憶體時可能出現的隨機數值或當機，更容易判斷問題所在。

全域宣告但尚未指定的 Handler（例如範例中的 `GridObject pacman;`）同樣不指向任何內容，`exists()` 為 `false`。跨關卡保存的全域 Handler，應在清除舊關卡後立即重新指定；Pac-Man 的 `BuildLevel()` 在 `clearObjects()` 之後就重新指定 `maze` 與 `pacman`，而鬼在讀取玩家座標前也會先檢查 `pacman.exists()`。

## GameEngine 也是 Handler

`GameEngine` 本身同樣可以複製：Engine 呼叫回呼函式時傳入的 `game`，就是 `main()` 中那個 Engine 的複本，兩者操作同一個遊戲。和其他 Handler 不同的是，GameEngine 的複本會共同讓遊戲保持存在；最後一份 GameEngine 消失時，Engine 才會釋放素材與所有內容，並關閉視窗。

```cpp title="main()：Engine 離開作用域時統一清理"
int main() {
    GameEngine game(8, 8, 64);
    game.addObject("pacman", nullptr, MovePacman);
    game.run();
    return 0;
}  // game 與它擁有的內容都會在此釋放。
```

最簡單、也最推薦的寫法，就是把 GameEngine 建立為 `main()` 的區域變數，最後呼叫 `run()`。視窗關閉後 `run()` 回傳，`main()` 結束時 Engine 自然完成清理；回呼函式需要 Engine 時使用參數 `game` 即可，不需要另外把它存成全域變數。

## 本節小結

- GameEngine 擁有所有由它建立的內容，程式只透過 Handler 操作。
- 複製 Handler 不會複製內容；`deepCopy()` 才建立獨立實體。
- `remove()`、`clearObjects()`、`clearOverlays()` 與 Engine 結束會讓對應的 Handler 失效。
- `exists()` 可以安全檢查 Handler；對失效 Handler 的其他操作會得到明確錯誤。
- 最後一份 GameEngine 消失時，Engine 釋放所有內容並關閉視窗。

[每幀執行順序](02-frame-order.md){ .md-button .md-button--primary }
