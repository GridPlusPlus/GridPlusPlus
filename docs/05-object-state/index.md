# 物件狀態

座標、圖片、方向、顏色、層級與可見性已有專用函式。遊戲額外需要的每實例資料，可放入 Handler
自己的狀態區：

```cpp
ghost.set("timer", 0);
ghost.set("random", true);
ghost.set("name", "Blinky");
```

目前支援 `long long`（也接受 `int`）、`double`、`bool` 與 `string`。這讓多隻鬼可以共用同一個
Update 函式，同時各自保存計時器和方向，不必建立自訂 class。
