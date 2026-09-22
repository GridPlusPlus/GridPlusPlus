# GameEngine 與 Handler 的壽命

`GameEngine` 是所有實體的擁有者。學生只使用建立函式的回傳值，不需要 `new`、`delete` 或位址語法。

```cpp
GameEngine game(8, 8, 64);
GridObject player = game.addObject("player");
Overlay score = game.addTextOverlay("Score: 0", 8, 8);
```

Handler 是可複製的存取憑證：

```cpp
auto alias = player;
alias.move(1, 0);  // player 與 alias 指向同一實體

auto copy = player.deepCopy();
copy.move(1, 0);   // copy 是獨立實體
```

`remove()`、對應的 `clear...()` 或 `GameEngine` 離開作用域後，`exists()` 回傳 false。對失效 Handler 做其他
操作會丟出清楚錯誤，而不是留下無法判斷的懸空指標。

`GameEngine` 本身也可按值複製，所有複本操作同一個內部遊戲；最後一份 GameEngine 消失時，視窗、素材與實體
一起清理。最簡單的寫法仍是把 GameEngine 建立在 `main()` 中，最後呼叫 `run()`。
