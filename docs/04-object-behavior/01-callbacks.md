# Init 與 Update

物件函式的固定形式是：

```cpp
void FunctionName(gridpp::Game game, gridpp::ObjectHandler self);
```

建立時依序傳入 Init、Update 與 Collide：

```cpp
auto player = game.addObject("player", InitPlayer, UpdatePlayer, HitPlayer);
```

- Init：`game.run()` 後執行一次。所有已加入元素都完成 Init 後，第一幀才開始。
- Update：每幀執行一次。隱藏物件也會 Update。
- `nullptr`：該位置暫時沒有函式。

也可以稍後設定：

```cpp
player.setUpdateFunction(UpdatePlayer);
```

替換已執行過的 Init 不會讓它重跑。遊戲執行中加入的新元素會在該幀結束依加入順序 Init，並從
下一幀開始 Update。
