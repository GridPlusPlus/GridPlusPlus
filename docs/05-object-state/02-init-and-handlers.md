# Init 與 Handler

建立物件後可立即設定不依賴時間的資料：

```cpp
auto ghost = game.addObject("ghost", InitGhost, MoveGhost);
ghost.setPosition(5, 3);
ghost.set("direction", 0);
```

Init 則處理必須等 `run()` 才有意義的工作：

```cpp
void InitGhost(gridpp::GameEngine game, gridpp::GridObject self) {
    self.set("nextMove", game.time() + 0.5);
}
```

Handler 可安全複製：

```cpp
auto alias = ghost;             // 同一隻鬼
auto snapshot = ghost.deepCopy();  // 新的獨立實體
```

`remove()` 後，所有指向原實體的 Handler 都會讓 `exists()` 回傳 false；其他操作會報錯。這比懸空
指標容易檢查，也不需要學生理解 `delete`。
