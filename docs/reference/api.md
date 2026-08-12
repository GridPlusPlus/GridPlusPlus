# API 速查

涵蓋學生會用到的公開介面。`GridSQLite.h` 中的內部命名空間 `gridpp::internal`
不在此列，詳見 [迷你 SQLite 讀取器](../internals/sqlite-reader.md)。

以下型別都位於 `gridpp` namespace。

## GridEngine

主引擎。定義於 `GridEngine.h`（主標頭 `GridPlusPlus.h` 也會引入）。

```cpp
GridEngine(int cols, int rows, int grid_size = 32);
void LoadAssets(const std::filesystem::path& database_path);
void set_background_color(Color color);   // 背景色，預設 RAYWHITE
Color background_color() const;
void set_show_grid(bool show);            // 網格線開關，預設關閉
bool show_grid() const;
int cols() const;
int rows() const;
int grid_size() const;
GridObject* Spawn(GridObject* object);
void Destroy(GridObject* object);
void ClearObjects();
void AddOverlay(Overlay* overlay);
void Run();
void DrawCell(const std::string& asset_name, int grid_x, int grid_y,
              int direction = 0, Color tint = WHITE);
```

| 成員 | 說明 |
|---|---|
| `GridEngine(cols, rows, grid_size)` | 建立引擎並開視窗；視窗寬、高各不得超過 8192 像素。 |
| `LoadAssets(path)` | 載入 `assets.db`。需在建立引擎後呼叫。 |
| `set_background_color/background_color` | 背景顏色（每幀清畫面用），預設 `RAYWHITE`。 |
| `set_show_grid/show_grid` | 網格線開關，預設關閉，需要時傳 `true` 打開。 |
| `cols/rows/grid_size()` | 查詢地圖大小與格子像素。 |
| `Spawn(obj)` | 放入物件並呼叫其 `OnSpawn()`。 |
| `Destroy(obj)` | 停止並刪除指定物件。 |
| `ClearObjects()` | 刪除並清空所有遊戲物件（用於重新開始；不影響覆蓋層）。 |
| `AddOverlay(overlay)` | 加入畫面覆蓋層（畫在網格之上）。 |
| `Run()` | 進入主循環直到視窗關閉。 |
| `DrawCell(asset, gx, gy)` | 在某格畫素材（給 `Render()` 用）。 |

## GridObject

所有遊戲物件的基底類別。定義於 `GridObject.h`（主標頭 `GridPlusPlus.h` 也會引入）。

```cpp
GridObject();
GridObject(std::string asset_name, int x, int y);

virtual void OnSpawn();
virtual void OnUpdate();
virtual void OnCollide(GridObject* other);
virtual void Render(GridEngine* engine);

int  x() const;  int  y() const;
void set_x(int x);   void set_y(int y);
void Move(int dx, int dy);

const std::string& asset_name() const;
void set_asset_name(const std::string& asset_name);
const std::string& tag() const;
void set_tag(const std::string& tag);

int   direction() const;          // 朝向 0~3
void  set_direction(int direction);   // 0~3，逆時針每 +1 轉 90°
Color tint() const;
void  set_tint(Color tint);           // 調色，預設 WHITE
int   z_index() const;
void  set_z_index(int z_index);       // 數值越大越晚繪製，預設 0

bool visible() const;
void set_visible(bool visible);

GridEngine* engine() const;       // 取得所屬引擎（Spawn 後才有效）
```

| 成員 | 說明 |
|---|---|
| 建構子（兩個） | 空的 / 指定素材與座標（重載示範）。 |
| `OnSpawn()` | 被 `Spawn` 時呼叫一次。覆寫用。 |
| `OnUpdate()` | 每幀呼叫。覆寫用。 |
| `OnCollide(other)` | 同格碰撞時呼叫。覆寫用。 |
| `Render(engine)` | 畫自己；預設畫 `asset_name`。可覆寫。 |
| `x/y/set_x/set_y/Move` | 存取格子座標。 |
| `asset_name/set_asset_name` | 讀取或更換物件繪製的素材名稱。 |
| `tag/set_tag` | 身分標記，碰撞時分辨對象。 |
| `direction/set_direction` | 朝向 0~3，繪製時旋轉素材（重複利用同一張圖）。 |
| `tint/set_tint` | 調色，把素材染成不同顏色（diffuse color）。 |
| `z_index/set_z_index` | 繪製層級；數值越大越上層，相同時後 Spawn 的在上方。 |
| `visible/set_visible` | 控制是否繪製與參與碰撞。 |
| `engine()` | 取得所屬引擎（`Spawn` 後才有效），例如查地圖大小做邊界檢查。 |

## CallbackGridObject

函式版物件。定義於 `GridObject.h`（繼承 `GridObject`）。用「傳進來的函式」決定行為，
不必先學繼承與覆寫。概念與限制見 [函式版物件](../guide/callback-grid-object.md)。

```cpp
using UpdateFn  = void(*)(GridObject* self);                 // 每幀要跑的函式
using CollideFn = void(*)(GridObject* self, GridObject* other); // 同格碰撞時要跑的函式

CallbackGridObject(std::string asset, int x, int y,
                   UpdateFn update, CollideFn collide = nullptr);
```

| 成員 | 說明 |
|---|---|
| `CallbackGridObject(asset, x, y, update, collide)` | 建立物件；`update` 每幀呼叫，`collide` 選填（同格碰撞時呼叫）。 |
| 其餘 | 與 `GridObject` 相同（`x/Move/set_tag/set_tint/engine`…）。 |

!!! note "限制"
    普通函式沒有「每個物件自己的狀態」。只適合**同種角色只有一個**或**不需記狀態**的情況；
    需要多個各自記狀態的角色時，請改成繼承 `GridObject`。

## GridMaze

選用的迷宮模組。定義於 `GridMaze.h`（繼承 `GridObject`）。

```cpp
GridMaze(int cols, int rows);
void SetWall(int x, int y, bool wall);
bool IsWall(int x, int y) const;

void SetWallAsset(const std::string& asset);
void SetWallTiles(const std::string& iso, const std::string& end,
                  const std::string& straight, const std::string& corner,
                  const std::string& tee, const std::string& cross);
int  width() const;
int  height() const;
```

| 成員 | 說明 |
|---|---|
| `GridMaze(cols, rows)` | 建立 1～64 欄、1～64 列的空迷宮；非法尺寸丟出例外。 |
| `SetWall(x, y, wall)` | 設定某格是不是牆；界外座標丟出例外。 |
| `IsWall(x, y)` | 那格是否為牆（界外當牆）。 |
| `SetWallAsset(name)` | 所有牆同一張圖。 |
| `SetWallTiles(...)` | 6 種基本形狀，靠旋轉自動拼接。 |
| `width/height` | 迷宮欄數 / 列數。 |

## GridShapes

不需素材包的選用圖形模組。額外 `#include "GridShapes.h"` 後使用：

```cpp
game.Spawn(new gridpp::shapes::Circle(2, 3, 24, BLUE));
```

`Square`、`Circle`、`Triangle`、`Pentagon`、`Star` 都接受
`(x, y, size, color)`；`size` 是像素，並可用 `size()` / `set_size()` 修改。

## Overlay

畫面覆蓋層的基底類別。定義於 `Overlay.h`（主標頭 `GridPlusPlus.h` 也會引入）。

```cpp
virtual void OnUpdate();   // 每幀呼叫（互動用）
virtual void Draw();       // 每幀呼叫，用像素座標繪製，畫在網格之上
```

繼承它、覆寫 `Draw()`（與選擇性的 `OnUpdate()`），再用 `engine.AddOverlay(...)` 加入。

## Label / Button

現成的畫面元件，定義於 `Overlay.h`（皆繼承 `Overlay`）。

```cpp
// 文字標籤
Label(std::string text, int x, int y, int font_size = 20, Color color = BLACK);
void set_text(const std::string& text);

// 可點按鈕（覆寫 OnClick 決定行為）
Button(std::string text, int x, int y, int width, int height);
virtual void OnClick();
```

| 成員 | 說明 |
|---|---|
| `Label(text, x, y, …)` | 在 (x, y) 畫一行文字。 |
| `Label::set_text(text)` | 更新文字內容。 |
| `Button(text, x, y, w, h)` | 在像素矩形畫一個按鈕。 |
| `Button::OnClick()` | 覆寫它：被滑鼠左鍵點擊時呼叫。 |

## 常用的 raylib 函式

這些來自 raylib，寫遊戲時很常用：

| 函式 | 說明 |
|---|---|
| `IsKeyPressed(KEY_*)` | 該鍵「剛按下的那一幀」回傳 true。 |
| `IsKeyDown(KEY_*)` | 該鍵「持續被按住」回傳 true。 |
| `GetRandomValue(min, max)` | 取一個範圍內的隨機整數。 |
| `DrawText(text, x, y, size, color)` | 在畫面上畫文字（像素座標）。 |
