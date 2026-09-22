# 素材包

素材沿用 Grid++ 的 `.db` 素材包。開啟遊戲後一次載入，再以素材名稱建立物件：

```cpp
gridpp::Game game(8, 8, 64);
game.loadAssets("game.db");

auto player = game.addObject("player");
auto logo = game.addOverlay("logo");
```

`ObjectHandler::setImage()` 與 `OverlayHandler::setImage()` 可在執行期間更換圖片。若名稱不存在，
畫面會顯示紅色方塊，讓規則仍可在素材尚未完成時測試。

網格圖片會配合 `gridSize()` 填滿格子；圖片 Overlay 使用像素座標與素材原始的 32×32 顯示大小。
