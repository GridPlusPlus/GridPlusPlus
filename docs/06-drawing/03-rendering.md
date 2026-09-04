# 用同一個物件狀態控制畫面

載入素材或建立基本圖形之後，畫面仍會隨遊戲狀態改變：玩家轉彎時圖片需要旋轉，受傷的敵人可能變色，遊戲結束後地鼠要暫時消失，而同格物件還必須決定誰畫在上方。GridObject 因此把素材名稱、方向、色調（tint）、可見狀態（visible）與 z-index 保存為外觀狀態。預設 `Render()` 會把這些值交給 Engine，自訂 `Render()` 也能沿用它們，遊戲規則不必直接操縱底層紋理。

## 方向與顏色

先處理同一個角色在格子內如何改變外觀。`set_direction()` 使用 90 度為單位旋轉素材，0 保持原始方向，1、2、3 分別逆時針旋轉 90、180、270 度，而其他整數會正規化到 0～3。下一章的 Pacman 會讓右、上、左、下使用相同編號，因此只需一張朝右的圖片便能顯示四個方向。

```cpp title="方向值示意"
player->set_direction(0);  // right
player->set_direction(1);  // up
player->set_direction(2);  // left
player->set_direction(3);  // down
```

方向解決旋轉，`set_tint()` 則處理同一張圖片的顏色變化。它接受 raylib 的 `Color`：`WHITE` 保留原色，其他顏色會與紋理混合。因此 Pacman 的四隻鬼可以共用白色 ghost 素材，再分別以 `RED`、`PINK`、`SKYBLUE` 和 `ORANGE` 顯示；素材名稱決定基礎圖片，`direction` 與 `tint` 則在繪製時補上物件自己的呈現狀態。

## 顯示與隱藏

旋轉與染色改變「怎麼畫」，`visible` 則決定「現在要不要畫」。`set_visible(false)` 會使物件停止繪製與碰撞，卻仍把物件保留在 Engine 中繼續更新，因此打地鼠可以在 30 秒結束時隱藏地鼠，等玩家按下 R 後重設座標並重新顯示同一個物件，而不必為一次暫停銷毀並重建資料。

```cpp title="遊戲流程節錄：隱藏與重新顯示"
mole->set_visible(false);

// 重新開始。
mole->set_x(0);
mole->set_y(0);
mole->set_visible(true);
```

永久移除的物件應交給 `Destroy()`。`visible` 是物件狀態，不會釋放資源，也不會停止 `OnUpdate()`。

## z-index

當多個可見物件落在同一格時，Engine 先繪製較小的 z-index，再繪製較大的值，因此數值較大的物件會覆蓋在上方；預設值是 0，負數可用於地板或背景，而玩家則可使用較大的值，讓角色不會被同格豆子遮住。

```cpp title="繪製層級示意"
floor->set_z_index(-10);
pellet->set_z_index(0);
player->set_z_index(10);
```

<figure markdown="span">
  ![繪製層次由地板、豆子、玩家到 Overlay 逐層向上](../images/render-layers.svg)
  <figcaption>GridObject 依 z-index 由小到大繪製；Overlay 最後繪製。</figcaption>
</figure>

相同 z-index 會保留生成順序，後生成的物件畫在先生成的物件上方。每幀更新期間呼叫 `set_z_index()`，新值會影響同一幀稍後的繪製；z-index 不改變更新與碰撞順序，也不適用於 Overlay，因為 Overlay 永遠位於所有 GridObject 上方。

## 自訂 `Render()`

前述狀態足以完成大多數素材物件，但若畫面需要同時包含基礎圖片與程式產生的效果，衍生 GridObject 可以覆寫 `Render(GridEngine* engine)`。下列物件先呼叫基底方法，保留素材名稱、方向與 `tint` 的預設繪製，再用 Engine 的格子大小換算中心位置，疊上一個代表生命值的紅色圓點。

```cpp title="物件類別節錄：自訂 Render()"
void Render(GridEngine* engine) override {
    GridObject::Render(engine);

    const int center_x = x() * engine->grid_size() + engine->grid_size() / 2;
    const int center_y = y() * engine->grid_size() + engine->grid_size() / 2;
    DrawCircle(center_x, center_y, health_, RED);
}
```

如果物件完全不需要圖片，也可以省略 `GridObject::Render(engine)`，只呼叫 raylib 函式完成自訂畫面；無論採取哪一種方式，`visible` 與 z-index 仍由 Engine 在呼叫之前統一處理。`Render()` 的責任是把已決定的遊戲狀態轉換成畫面，不應在此改變座標、生命值或勝負結果，否則同一幀的碰撞已經結束，畫面與規則便可能出現難以理解的一幀落差。

## 本節小結

- `direction` 以 90 度為單位旋轉素材，`tint` 改變繪製顏色。
- 隱藏物件停止繪製和碰撞，但仍持續更新。
- z-index 越大越晚繪製；相同值依生成順序。
- 覆寫 `Render()` 可加入或取代預設素材繪製。
