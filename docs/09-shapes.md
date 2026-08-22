# 不需素材的基本圖形

素材適合呈現角色和場景的完整圖片，但不是每個遊戲都需要先準備美術。原型、作業與規則測試通常只需要能辨識不同物件的形狀和顏色。直接使用 raylib 繪製可以做到這件事，但每個物件都自行換算格子中心和尺寸會重複相同程式。

`GridShapes.h` 將常用的填滿圖形包裝成 GridObject。圖形具有網格座標、顏色和尺寸，也沿用一般物件的 tag、visible、z-index、碰撞與 Engine 所有權。它們和素材物件的差別只在繪製方式：基本圖形直接呼叫 raylib，不會向素材包查詢圖片。

這個模組不由 `GridPlusPlus.h` 自動引入，因為不是所有遊戲都需要基本圖形。使用時需額外 include；所有圖形位於 `gridpp::shapes` namespace，避免 `Circle` 或 `Square` 等通用名稱和遊戲自己的類別衝突。

```cpp
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

```cpp
Square(int x, int y, int size, Color color = BLACK);
Circle(int x, int y, int size, Color color = BLACK);
Triangle(int x, int y, int size, Color color = BLACK);
Pentagon(int x, int y, int size, Color color = BLACK);
Star(int x, int y, int size, Color color = BLACK);
```

下列程式將五種圖形放在同一列。因為它們都是 `GridObject`，所以一律透過 `Spawn()` 交給引擎管理。

```cpp
using namespace gridpp::shapes;

game.Spawn(new Square(0, 1, 32, RED));
game.Spawn(new Circle(1, 1, 32, ORANGE));
game.Spawn(new Triangle(2, 1, 32, GREEN));
game.Spawn(new Pentagon(3, 1, 32, BLUE));
game.Spawn(new Star(4, 1, 32, PURPLE));
```

## 尺寸與顏色

`size()` 讀取目前尺寸，`set_size()` 修改尺寸。尺寸必須大於 0，否則建構子或 setter 會丟出 `std::invalid_argument`。顏色沿用 `GridObject::tint()` 與 `set_tint()`，沒有另一套圖形專用狀態。

```cpp
auto* target = new gridpp::shapes::Circle(3, 3, 24);
target->set_size(40);
target->set_tint(MAROON);
game.Spawn(target);
```

基本圖形仍具有座標、tag、visible、z-index 和碰撞行為。它們沒有素材名稱，修改 `asset_name` 不會改變圖形的 `Render()`。需要 callback 行為時，可衍生其中一個圖形類別並覆寫生命週期函式；若只需要一個可點擊或可碰撞的圖形，也可以保存回傳指標並由其他遊戲邏輯修改它。
