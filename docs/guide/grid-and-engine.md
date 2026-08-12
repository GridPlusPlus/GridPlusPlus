# 網格與引擎

## 網格座標

Grid++ 的世界是一張格子表。座標以「格」為單位，不是像素：

- `x` 是「欄」（column），由左往右。
- `y` 是「列」（row），由上往下。
- 左上角是 `(0, 0)`。
- 一格實際多大由 `grid_size`（像素）決定。

物件永遠對齊在格子上，移動是「一次跳一整格」的瞬間移動。

## GridEngine

`GridEngine` 是主引擎：開視窗、跑主循環、畫網格與物件、處理碰撞。
你的程式通常就四步：

```cpp
GridEngine game(10, 10, 32);   // 1. 建立：10 欄 x 10 列，每格 32 像素
game.LoadAssets("assets.db");  // 2. 載入素材包
game.Spawn(new Player());      // 3. 放入物件（可呼叫多次）
game.Run();                    // 4. 開始遊戲（內建主循環）
```

| 方法 | 說明 |
|---|---|
| `GridEngine(cols, rows, grid_size = 32)` | 建立並開視窗。視窗大小 = `cols*grid_size` × `rows*grid_size`，寬、高各不得超過 8192 像素。 |
| `LoadAssets(path)` | 從 `assets.db` 載入素材（要在建立引擎之後呼叫）。 |
| `Spawn(obj)` | 把物件放進世界，並呼叫它的 `OnSpawn()`。 |
| `Run()` | 進入遊戲主循環，直到關閉視窗或按 ESC。 |
| `set_background_color(color)` | 設定背景顏色（每幀清畫面時使用），預設 `RAYWHITE`。 |
| `set_show_grid(show)` | 開啟/關閉網格線。**預設關閉**，需要時傳 `true` 打開。 |
| `cols()` / `rows()` / `grid_size()` | 查詢地圖大小，常用來做邊界檢查。 |
| `DrawCell(asset, x, y)` | 在某格畫一張素材。給物件的 `Render()` 使用。 |

## 主循環做了什麼

`Run()` 內部每一幀（frame）固定做三件事：

1. **更新**：對每個物件呼叫 `OnUpdate()`（你在這裡讀鍵盤、移動）。
2. **碰撞**：兩兩比對，若兩個物件在同一格，對雙方呼叫 `OnCollide()`。
3. **繪製**：用背景色清畫面 →（若開啟）畫網格線 → 依 `z_index` 對每個物件呼叫 `Render()`。

背景色與網格線可分別用 `set_background_color()` 與 `set_show_grid()` 調整；網格線**預設關閉**。

```mermaid
graph LR
  A[OnUpdate 全部物件] --> B[碰撞偵測]
  B --> C[Render 全部物件]
  C --> A
```

理解這個順序很重要：你在 `OnUpdate` 改變座標，引擎在同一幀稍後就會用新座標做碰撞與繪製。

物件的 `z_index` 預設為 `0`。數值越大越晚繪製、看起來越上層；相同時維持
`Spawn()` 順序，因此後 Spawn 的物件在上方。它只影響繪製，不影響更新與碰撞順序。

## 幀率（frame rate）

引擎固定以 **60 FPS** 為目標執行主循環（建立引擎時設定，目前不可調整）。也就是說
`OnUpdate()` 每秒大約被呼叫 60 次。

!!! note "用「幀數」控制速度"
    因為每幀都會跑一次 `OnUpdate()`，若每幀都 `Move()`，角色會快到看不清。常見作法是
    自己數幀數、每隔幾幀才動一次：

    ```cpp
    if (++timer < 8) return;   // 每 8 幀（約每 0.13 秒）才走一格
    timer = 0;
    Move(1, 0);
    ```

    Pac-Man 範例就是用這個方式讓玩家與鬼有不同速度。由於速度是綁在幀數上，60 FPS 是
    它們快慢的基準。

## 邊界檢查的小範例

```cpp
void OnUpdate() override {
    if (IsKeyPressed(KEY_RIGHT) && x() < engine()->cols() - 1)
        Move(1, 0);
}
```

`engine()` 會回傳物件所屬引擎的指標，`Spawn()` 時會自動設好。
