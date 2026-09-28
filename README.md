# Grid++

Grid++ 是供程式設計入門課程使用的 C++17 網格遊戲函式庫。它處理視窗、遊戲迴圈、繪圖與碰撞，讓學生只需要建立 `GameEngine`、加入內容、註冊普通函式，就能完成遊戲；過程中不需要自行使用指標、`new`、`delete`、繼承或設計 template。

![Pac-Man 範例畫面](docs/images/preview.png)

```cpp
#include "GridPlusPlus.h"

using gridpp::GameEngine;
using gridpp::GridObject;

void Move(GameEngine game, GridObject self) {
    if (game.keyPressed(KEY_RIGHT)) self.move(1, 0);
}

int main() {
    GameEngine game(8, 8, 64);
    game.showGrid(true);
    game.addObject("player", nullptr, Move);
    game.run();
}
```

## 主要內容

- `GameEngine`：建立遊戲、加入內容、讀取輸入並啟動主迴圈。
- `GridObject`：操作網格中的玩家、敵人、道具與基本圖形。
- `Overlay`：操作以像素座標顯示在遊戲上方的圖片、文字與按鈕。
- `Maze`：設定及查詢迷宮牆面。

四者都是 Handler：複製 Handler 只會取得同一個實體的另一個存取入口，需要獨立副本時使用 `deepCopy()`。所有內容由 `GameEngine` 管理，學生不需要也不應自行釋放。

Grid++ 是只需引入標頭檔的函式庫，唯一的外部相依是 [raylib](https://www.raylib.com/)。

## 快速執行

Windows 建議使用 WSL；想產生原生 Windows 執行檔時再使用 MinGW-w64。請先依照[安裝 raylib](docs/01-getting-started/01-installation.md)準備函式庫，再 Fork [GridPlusPlus-Template](https://github.com/GridPlusPlus/GridPlusPlus-Template) 並 clone 自己的 fork。以下指令都在專案根目錄執行。

### WSL / Linux

```bash
g++ -std=c++17 main.cpp -o game \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```

### Windows / MinGW-w64

```bash
g++ -std=c++17 main.cpp -o game.exe \
    -Iraylib/include -Lraylib/lib -lraylib -lgdi32 -lwinmm
./game.exe
```

### macOS

```bash
g++ -std=c++17 main.cpp -o game $(pkg-config --cflags --libs raylib)
./game
```

## 文件

[`docs/`](docs/index.md) 先建立核心模型，再以普通函式完成打地鼠，接著說明 Init、Update、Collide、碰撞、每個物件自己的狀態與繪製。Pac-Man 範例組合迷宮、共享狀態、素材與 Overlay；最後整理生命週期、Handler 的有效期限、每幀順序與可重現的發布流程。

章節使用編號資料夾，節使用資料夾內的編號檔名。API 參考由公開標頭檔中的 Doxygen 註解自動產生，與操作教學分開維護。完整範例位於 [`examples/`](examples/)。

## 專案結構

| 路徑 | 用途 |
|---|---|
| `GridPlusPlus.h` | 學生程式唯一需要引入的標頭。 |
| `GameEngine.h` | `GameEngine`、遊戲迴圈與實體管理。 |
| `GridObject.h`、`Overlay.h`、`GridMaze.h` | 三種 Handler 與對應的實體資料。 |
| `GridValue.h` | `set()`、`get()` 使用的型別安全儲存。 |
| `GridAssetManager.h`、`GridSQLite.h` | 素材包載入與 texture 管理。 |
| `template.cpp` | 可直接編譯的起始程式，同步為 GridPlusPlus-Template 的 `main.cpp`。 |
| `examples/` | 打地鼠、Pac-Man 入門版與完整版範例。 |
| `tools/create_asset_pack.py` | 將 32×32 PNG 轉成素材包。 |
| `docs/` | 教學文件原始檔。 |
