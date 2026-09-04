# 不需素材的基本圖形

素材適合呈現角色和場景的完整圖片，但不是每個遊戲都需要先準備美術。原型、作業與規則測試通常只需要能辨識不同物件的形狀和顏色。直接使用 raylib 繪製可以做到這件事，但每個物件都自行換算格子中心和尺寸會重複相同程式。

`GridShapes.h` 將常用的填滿圖形包裝成 GridObject。圖形具有網格座標、顏色和尺寸，也沿用一般物件的 tag、`visible`、z-index、碰撞與 Engine 所有權。它們和素材物件的差別只在繪製方式：基本圖形直接呼叫 raylib，不會向素材包查詢圖片。

這個模組不由 `GridPlusPlus.h` 自動引入，因為不是所有遊戲都需要基本圖形。使用時需額外 include；所有圖形位於 `gridpp::shapes` namespace，避免 `Circle` 或 `Square` 等通用名稱和遊戲自己的類別衝突。

```cpp title="main.cpp：生成一個圓形"
#include "GridPlusPlus.h"
#include "GridShapes.h"

int main() {
    gridpp::GridEngine game(8, 8, 64);
    game.Spawn(new gridpp::shapes::Circle(2, 3, 36, BLUE));
    game.Run();
}
```

圖形建構子的參數依序是網格 x、網格 y、像素尺寸與顏色。尺寸代表圖形外接框的寬度，圖形會置中於所在格。尺寸可以大於格子，引擎不會裁切或自動縮小；超出視窗的部分由 raylib 正常裁切。

## 可用圖形

五種圖形使用相同的建構介面：

```cpp title="建構子簽名示意"
Square(int x, int y, int size, Color color = BLACK);
Circle(int x, int y, int size, Color color = BLACK);
Triangle(int x, int y, int size, Color color = BLACK);
Pentagon(int x, int y, int size, Color color = BLACK);
Star(int x, int y, int size, Color color = BLACK);
```

下列程式將五種圖形放在同一列。因為它們都是 `GridObject`，所以一律透過 `Spawn()` 交給引擎管理。

```cpp title="main() 節錄：生成五種基本圖形"
using namespace gridpp::shapes;

game.Spawn(new Square(0, 1, 32, RED));
game.Spawn(new Circle(1, 1, 32, ORANGE));
game.Spawn(new Triangle(2, 1, 32, GREEN));
game.Spawn(new Pentagon(3, 1, 32, BLUE));
game.Spawn(new Star(4, 1, 32, PURPLE));
```

<figure markdown="span">
  ![五個格子依序顯示方形、圓形、三角形、五邊形與五芒星](../images/grid-shapes.svg)
  <figcaption>五種圖形共用相同建構介面，`size` 控制圖形在格子中的像素大小。</figcaption>
</figure>

## 尺寸與顏色

`size()` 讀取目前尺寸，`set_size()` 修改尺寸。尺寸必須大於 0，否則建構子或 setter 會丟出 `std::invalid_argument`。顏色沿用 `GridObject::tint()` 與 `set_tint()`，沒有另一套圖形專用狀態。

```cpp title="main() 節錄：修改圖形尺寸與顏色"
auto* target = new gridpp::shapes::Circle(3, 3, 24);
target->set_size(40);
target->set_tint(MAROON);
game.Spawn(target);
```

基本圖形仍具有座標、tag、`visible`、z-index 和碰撞能力，只是它們不依賴素材名稱，因此修改 `asset_name` 不會改變圖形的 `Render()`。如果圖形需要每個實例各自保存計時器或其他狀態，可以像第 5 章的 `Mole` 一樣衍生圖形類別並覆寫生命週期方法；若它只是可點擊或可碰撞的標記，則保留 Engine 回傳的借用指標，由既有遊戲邏輯修改位置、尺寸或顏色即可。

## 本節小結

- 五種基本圖形不需素材包，也能參與一般物件的更新與碰撞流程。
- 圖形尺寸使用像素，座標仍使用網格單位；顏色沿用 `tint`。
- 需要個別狀態時可以衍生圖形類別，只需外部控制時則保留借用指標。

執行五種圖形的範例後，視窗中應在 y=1 的一列由左至右看到紅色方形、橘色圓形、綠色三角形、藍色五邊形與紫色五芒星，而且每個圖形都置中於自己的格子。如果圖形偏離格子中心，應先檢查傳入的是網格座標而不是像素座標；如果圖形互相重疊，則應比較 `size` 與 Engine 的 `grid_size`。

完整類別索引見 [shapes namespace API](../api/namespacegridpp_1_1shapes.md)。
