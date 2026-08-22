# GridEngine

遊戲程式需要在視窗存在期間持續工作。它必須反覆讀取輸入、更新角色、判斷碰撞、清除上一張畫面並繪製下一張畫面，還要確保圖片和物件在正確時間建立與釋放。這些工作共同構成整個遊戲依賴的執行環境，不由個別玩家或敵人負責。

`GridEngine` 就是 Grid++ 提供的執行環境。它代表一個正在運作的網格遊戲世界，同時管理 raylib 視窗、網格尺寸、主迴圈、遊戲物件、Overlay 和素材。遊戲規則不必分別維護這些系統，只需把物件交給 Engine，再描述物件在更新或碰撞時應做什麼。

## Engine 與遊戲內容的邊界

Engine 決定「遊戲如何運轉」，使用者程式決定「這是什麼遊戲」。例如 Engine 知道每一幀要檢查同格物件，卻不知道玩家碰到某個物件代表得分、受傷或過關；這個意義由物件的碰撞行為決定。Engine 知道如何把素材畫到指定格子，卻不知道哪張圖代表玩家。

這個邊界讓不同遊戲共用同一套基礎設施。打地鼠和 Pacman 的規則完全不同，但兩者都需要視窗、更新、繪製和資源清理，因此都可以由 GridEngine 管理。使用者不需要為每個遊戲重寫 raylib 主迴圈。

一個程式通常只建立一個 GridEngine。這個 Engine 對應一個視窗和一個遊戲世界；加入其中的 GridObject 與 Overlay 都由它擁有。重新開始關卡時，通常保留同一個 Engine，再清除並重建其中的物件。

## 建立遊戲世界

建立 GridEngine 時，程式要先決定世界有多少格，以及每一格在視窗中占多少像素。這三個數值建立了遊戲邏輯座標與實際畫面之間的比例關係。

```cpp
#include "GridPlusPlus.h"

using gridpp::GridEngine;

int main() {
    GridEngine game(8, 8, 64);
    game.set_background_color(BEIGE);
    game.set_show_grid(true);
    game.Run();
}
```

這段程式建立打地鼠使用的遊戲世界。此時 Engine 只顯示 8×8 網格，尚未加入地鼠和畫面資訊。

## 網格與視窗尺寸

建構子的三個參數依序是欄數、列數與單格像素尺寸。`GridEngine game(8, 8, 64)` 會建立寬高皆為 `8 × 64 = 512` 像素的視窗。`cols()`、`rows()` 與 `grid_size()` 可在執行期間讀取這三個設定。

```cpp
int width_in_cells = game.cols();       // 8
int height_in_cells = game.rows();      // 8
int cell_size = game.grid_size();       // 64
```

三個建構參數都必須大於 0，視窗寬度與高度不得超過 8192 像素。Engine 會先使用較大的整數型別計算乘積，再檢查限制，因此極大的欄數或格子尺寸不會在驗證前造成整數溢位。尺寸錯誤會丟出英文 `std::invalid_argument`，而且不會建立 raylib 視窗。

網格座標不等於像素座標。物件位於 `(3, 2)` 且每格為 64 像素時，該格左上角的像素位置是 `(192, 128)`。一般物件只使用網格座標；Overlay 和直接呼叫 raylib 的自訂繪製才需要像素座標。

## 背景與網格線

`set_background_color()` 設定每幀清除畫面時使用的 raylib `Color`，`background_color()` 讀取目前設定。`set_show_grid(true)` 顯示格線，適合開發時確認物件位置；正式畫面可以關閉。

```cpp
game.set_background_color(Color{245, 235, 210, 255});
game.set_show_grid(true);

Color background = game.background_color();
bool grid_is_visible = game.show_grid();
```

格線只影響繪製，不會建立牆壁或限制移動。物件能否進入某一格，仍由遊戲程式或 `GridMaze` 判斷。

## 遊戲主迴圈

`Run()` 持續執行遊戲幀，直到使用者關閉視窗。在桌面平台，它等同於反覆執行 Grid++ 的內部 tick；在 WebAssembly 平台，瀏覽器負責安排每一幀。遊戲程式不應在 `Run()` 外再建立另一個 raylib while-loop。

每幀包含物件更新、Overlay 更新、同格碰撞、物件繪製與 Overlay 繪製。詳細順序以及 runtime spawn、destroy 的延後規則集中在[生命週期與所有權](../08-lifecycle/index.md)。一般功能只需要在 callback 或覆寫函式中提供行為，不需要直接控制主迴圈。

`Run()` 是阻塞函式。桌面版只有在視窗關閉後才會回傳，因此必須在呼叫前完成初始物件、Overlay 與素材設定。

## 管理物件

`Spawn()` 將 `GridObject` 加入遊戲並接管所有權。函式有兩種主要形式：一種接受已用 `new` 建立的物件，另一種接受素材、位置和 callback，由 Engine 建立 `CallbackGridObject`。

```cpp
gridpp::GridObject* first = game.Spawn(new gridpp::GridObject("mole", 2, 3));
gridpp::GridObject* second = game.Spawn("mole", 5, 4, UpdateMole, HitMole);
```

`Destroy()` 移除單一物件；`ClearObjects()` 移除所有物件。這些函式不會清除 Overlay 或素材。`AddOverlay()` 加入一個 Overlay 並接管其所有權。所有回傳或保留的指標都是借用指標，不應由呼叫端 delete。

```cpp
game.Destroy(first);
game.ClearObjects();
game.AddOverlay(new gridpp::Label("Score: 0", 12, 12));
```

Engine 可以在主迴圈中接受新的物件與 Overlay。為了避免正在迭代的容器失效，本幀新增的內容會依類型套用明確的延後規則；使用者不需要自行建立佇列。

## 載入素材

`LoadAssets(path)` 載入 Grid++ 素材包。素材應在建立需要它的物件前載入；物件只保存素材名稱，實際 texture 由 Engine 統一持有。

```cpp
GridEngine game(8, 8, 64);
game.LoadAssets("assets.db");
game.Spawn("mole", 3, 4, UpdateMole);
```

再次載入會取代目前素材，但只有在新素材包完整載入成功後才清除舊 texture。素材檔案與命名規則在[素材與素材包](../06-drawing/01-assets.md)說明。不使用素材包時可以依賴紅色 fallback 方塊，或額外引入 `GridShapes.h`。

## DrawCell

`DrawCell()` 是提供給自訂 `Render()` 和模組使用的低階繪製函式。它將素材畫在指定網格座標，可同時設定旋轉方向與 tint。

```cpp
void Player::Render(GridEngine* engine) {
    engine->DrawCell("player", x(), y(), direction(), tint());
}
```

一般 `GridObject` 已經透過預設 `Render()` 呼叫等效操作，因此不需重複覆寫。只有需要組合多次繪製或改變預設畫面時才直接使用 `DrawCell()`。

## 資源清理

Engine 解構時會釋放所有物件、Overlay 和 texture，再關閉 raylib 視窗。將 Engine 建立為 `main()` 的區域變數，能讓 C++ 作用域自然控制整個遊戲生命週期。

```cpp
int main() {
    GridEngine game(8, 8, 64);
    game.Run();
}  // 自動清理資源並關閉視窗
```

Engine 不可複製，因為兩個 Engine 不能同時擁有同一組視窗與資源。需要重新開始遊戲時，通常保留原本的 Engine，並使用 `ClearObjects()` 重建關卡。

[GridObject](02-grid-object.md){ .md-button .md-button--primary }

完整函式簽名見 [GridEngine API](../api/classgridpp_1_1_grid_engine.md)。
