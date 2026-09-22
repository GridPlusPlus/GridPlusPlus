# ObjectHandler

`addObject()` 建立網格物件並回傳 `ObjectHandler`：

```cpp
gridpp::ObjectHandler mole = game.addObject("mole");
mole.setPosition(3, 2);
mole.move(-1, 1);
```

常用操作包括 `x()`、`y()`、`setPosition()`、`move()`、`setImage()`、`setDirection()`、
`setColor()`、`setLayer()`、`show()` 與 `hide()`。隱藏物件仍會更新，但不繪製也不碰撞。

物件行為是普通函式：

```cpp
void UpdateMole(gridpp::Game game, gridpp::ObjectHandler self) {
    if (game.keyPressed(KEY_RIGHT)) self.move(1, 0);
}

auto mole = game.addObject("mole", nullptr, UpdateMole);
```

第一個可選函式是 Init，第二個是 Update，第三個是 Collide。不想在建立時指定時可傳 `nullptr`，
之後再用 `setInitFunction()`、`setUpdateFunction()` 或 `setCollideFunction()` 設定。
