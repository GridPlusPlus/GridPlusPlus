# 多隻鬼的獨立狀態

所有鬼共用 `MoveGhost`，但每個 Handler 保存自己的方向、計時器與行為種類：

```cpp
auto ghost = game.addObject("ghost", nullptr, MoveGhost);
ghost.set("timer", 0);
ghost.set("direction", 0);
ghost.set("random", ghost_count == 3);
```

Update 以 `get()` 取出目前值，計算後再 `set()` 回去。貪心鬼從可走且不回頭的方向中，選擇移動後
離玩家曼哈頓距離最短者；隨機鬼則用 `game.random()` 選擇。

這個設計證明「多實例各有狀態」不等於必須繼承：函式負責規則，Handler state 負責每隻鬼的資料。
