# 使用 set 與 get

`set(key, value)` 會新增或更新一筆資料；同一 key 的型別固定：

```cpp
self.set("timer", 0);
self.set("speed", 1.5);
self.set("active", true);
self.set("name", "ghost");
```

`get(key, output)` 接近 `scanf` 的回傳方式：找到資料回傳 1，找不到回傳 0 並把輸出設為預設值。

```cpp
long long timer = 0;
if (self.get("timer", timer) == 0) {
    self.set("timer", 0);
}
```

若 key 已存在但取用型別不同，Grid++ 會丟出清楚錯誤，避免把錯誤資料靜默轉型。

只屬於整局的資料，例如分數與遊戲階段，可以直接使用程式的全域變數；只屬於某個實體的資料才
放進 `self`。
