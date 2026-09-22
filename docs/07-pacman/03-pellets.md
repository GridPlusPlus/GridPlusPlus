# 豆子與勝利條件

豆子與玩家的種類存在 Handler state，不需要舊式 tag 或自訂 class：

```cpp
pellet.set("type", "pellet");
player.set("type", "pacman");
```

碰撞時讀取對方種類，吃到後直接移除自己：

```cpp
void EatPellet(GameEngine, GridObject self, GridObject other) {
    std::string type;
    other.get("type", type);
    if (type != "pacman") return;

    self.remove();
    if (--pellets_left <= 0) game_state = GameState::kWon;
}
```

`remove()` 會使豆子立即停止後續碰撞與繪製，實際記憶體釋放由 GameEngine 安排，學生不需處理。
