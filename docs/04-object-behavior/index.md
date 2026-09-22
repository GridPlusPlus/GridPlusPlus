# 定義物件行為

行為是一個具名普通函式，再把函式名稱註冊給物件：

```cpp
void UpdatePlayer(gridpp::Game game, gridpp::ObjectHandler self) {
    if (game.keyPressed(KEY_RIGHT)) self.move(1, 0);
}
```

`Game` 讓行為讀取輸入及世界尺寸，`self` 代表這次正在執行的物件。學生不需要自己呼叫這個函式；
Engine 會在每一幀透過已註冊的函式指標呼叫它。

Grid++ 提供 Init、Update、Collide 三個物件時機，Overlay 另有 Click。函式可在建立時傳入，也可用
名稱以 `set...Function()` 開頭的 setter 隨時替換。
