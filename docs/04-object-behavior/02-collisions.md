# 同格互動

兩個可見物件位於同一個有效格子時，雙方的 Collide 函式都會收到對方 Handler：

```cpp
void EatPellet(gridpp::GameEngine, gridpp::GridObject self, gridpp::GridObject other) {
    std::string type;
    other.get("type", type);
    if (type == "player") self.remove();
}
```

建立豆子時保存種類並註冊碰撞：

```cpp
auto pellet = game.addObject("pellet", nullptr, nullptr, EatPellet);
pellet.set("type", "pellet");
```

隱藏或已呼叫 `remove()` 的物件不再碰撞。迷宮牆面不靠同格碰撞；移動前以
`maze.isWall(target_x, target_y)` 查詢，地圖外也會視為牆。
