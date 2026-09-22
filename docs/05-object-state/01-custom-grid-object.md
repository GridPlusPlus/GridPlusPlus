# 使用 set 與 get

`set(key, value)` 會新增或更新一筆資料；同一 key 的型別固定：

```cpp
self.set("timer", 0);
self.set("speed", 1.5);
self.set("active", true);
self.set("name", "ghost");
```

`get(key, output)` 使用一般 status code：成功回傳 0，找不到回傳 1 並把輸出設為預設值。

```cpp
long long timer = 0;
if (self.get("timer", timer) != 0) {
    self.set("timer", 0);
}
```

若 key 已存在但取用型別不同，Grid++ 會丟出清楚錯誤，避免把錯誤資料靜默轉型。

也可以保存任意可複製型別的 `vector<T>`：

```cpp
vector<Point> path;
self.set("path", path);
self.get("path", path);
```

vector 會依照標準 C++ 規則逐項複製。若 `T` 是指標，只複製位址；Grid++ 不取得指標所有權。

只屬於整局的資料，例如分數與遊戲階段，可以直接使用程式的全域變數；只屬於某個實體的資料才
放進 `self`。
