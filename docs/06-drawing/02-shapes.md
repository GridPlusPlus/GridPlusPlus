# 基本圖形

五種圖形都由 `GameEngine` 建立並回傳 `GridObject`：

```cpp
auto square = game.addSquare(0, 1, 32, RED);
auto circle = game.addCircle(1, 1, 32, ORANGE);
auto triangle = game.addTriangle(2, 1, 32, GREEN);
auto pentagon = game.addPentagon(3, 1, 32, BLUE);
auto star = game.addStar(4, 1, 32, YELLOW);
```

參數依序是格子 x、格子 y、像素尺寸與顏色，後方仍可附加 Init、Update、Collide：

```cpp
auto target = game.addCircle(3, 3, 24, RED, InitTarget, MoveTarget, HitTarget);
target.setColor(PINK);
target.move(1, 0);
```

圖形沒有圖片，因此對它呼叫 `image()` 或 `setImage()` 會報錯。其餘 GridObject 操作與圖片物件
完全相同；`clearObjects()` 也會一起清除。
