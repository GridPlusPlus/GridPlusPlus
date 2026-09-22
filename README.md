# Grid++

Grid++ 是給 C++ 初學者使用的網格遊戲函式庫。學生只需要建立 `Game`、加入內容、註冊普通函式，
不需要自行使用指標、`new`、`delete`、繼承或 template。

```cpp
#include "GridPlusPlus.h"

using gridpp::Game;
using gridpp::ObjectHandler;

void Move(Game game, ObjectHandler self) {
    if (game.keyPressed(KEY_RIGHT)) self.move(1, 0);
}

int main() {
    Game game(8, 8, 64);
    game.showGrid(true);
    game.addObject("player", nullptr, Move);
    game.run();
}
```

主要公開類別：

- `Game`：建立遊戲、加入內容、讀取輸入並啟動主迴圈。
- `ObjectHandler`：操作網格物件與基本圖形。
- `OverlayHandler`：操作圖片、文字與按鈕。
- `MazeHandler`：設定及查詢迷宮牆面。

複製 Handler 只會取得同一個實體的另一張存取憑證；需要獨立副本時使用 `deepCopy()`。
物件由 `Game` 管理，學生不需要也不應自行釋放。

安裝、教學與完整範例請見 [`docs/`](docs/index.md) 與 [`examples/`](examples/)。

## 專案檔案

| 檔案 | 用途 |
|---|---|
| `GridPlusPlus.h` | 學生程式唯一需要引入的標頭。 |
| `Game.h` | `Game` 與三種 Handler 的公開介面。 |
| `GridEngine.h`、`GridObject.h`、`Overlay.h` | 相容舊程式及內部實作。新教材不直接使用。 |
| `GridMaze.h`、`GridShapes.h` | 迷宮與圖形的內部實作及舊介面。 |
