# 繪製狀態與順序

GridObject 內建的外觀狀態包含素材名稱、方向、tint、visible 與 z-index。預設 `Render()` 會把這些值交給 Engine；自訂 Render 也可以選擇使用相同狀態，讓遊戲程式透過一致的 getter 和 setter 控制畫面。

## 方向與顏色

`set_direction()` 使用 90 度為單位旋轉素材。0 保持原始方向，1、2、3 分別逆時針旋轉 90、180、270 度；其他整數會正規化到 0～3。Pacman 只需一張朝右的素材，再依移動方向設定旋轉值。

```cpp
player->set_direction(0);  // right
player->set_direction(1);  // up
player->set_direction(2);  // left
player->set_direction(3);  // down
```

`set_tint()` 使用 raylib 的 `Color`。`WHITE` 保留圖片原色；其他顏色與 texture 混合。Pacman 的鬼共用白色 ghost 素材，再以 `RED`、`PINK`、`SKYBLUE` 和 `ORANGE` 顯示不同角色。

## 顯示與隱藏

`set_visible(false)` 使物件停止繪製和碰撞。物件仍留在 Engine 中並執行更新，因此可以在倒數完成後再次顯示。打地鼠在 30 秒結束時隱藏地鼠，按 R 後移動並重新顯示同一物件。

```cpp
mole->set_visible(false);

// Restart
mole->set_x(0);
mole->set_y(0);
mole->set_visible(true);
```

永久移除的物件應交給 `Destroy()`。visible 是物件狀態，不會釋放資源，也不會停止 `OnUpdate()`。

## z-index

Engine 先繪製較小的 z-index，再繪製較大的值，因此數值較大的物件顯示在上方。預設值是 0，負數可用於地板或背景。

```cpp
floor->set_z_index(-10);
pellet->set_z_index(0);
player->set_z_index(10);
```

相同 z-index 保留 spawn 順序，後 spawn 的物件會畫在先 spawn 的物件上方。每幀更新期間呼叫 `set_z_index()`，新值會影響同一幀稍後的繪製。z-index 不改變更新與碰撞順序，也不適用於 Overlay；Overlay 永遠位於所有 GridObject 上方。

## 自訂 Render

衍生 GridObject 可覆寫 `Render(GridEngine* engine)`。下列物件先使用預設素材繪製，再在格子中央加上生命值指示點。

```cpp
void Render(GridEngine* engine) override {
    GridObject::Render(engine);

    const int center_x = x() * engine->grid_size() + engine->grid_size() / 2;
    const int center_y = y() * engine->grid_size() + engine->grid_size() / 2;
    DrawCircle(center_x, center_y, health_, RED);
}
```

也可以完全省略 `GridObject::Render(engine)`，只使用 raylib 函式。自訂 Render 仍受 visible 和 z-index 控制，Engine 只會對可見且尚未排定刪除的物件呼叫它。

## Summary

- direction 以 90 度為單位旋轉素材，tint 改變繪製顏色。
- 隱藏物件停止繪製和碰撞，但仍持續更新。
- z-index 越大越晚繪製；相同值依 spawn 順序。
- 覆寫 `Render()` 可加入或取代預設素材繪製。
