# 物件狀態

座標、圖片、方向、顏色、層級與可見性已有專用函式。遊戲額外需要的每實例資料，可放入 Handler
自己的狀態區：

```cpp
ghost.set("timer", 0);
ghost.set("random", true);
ghost.set("name", "Blinky");
```

支援 `int`、`long long`、`double`、`bool`、`string`，以及任意可複製型別的 `vector<T>`。這讓多隻鬼
可以共用同一個 Update 函式，同時各自保存計時器和方向，不必建立 GridObject 子類別。
