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

學生從 Gitea（<https://git.gridplusplus.ntuee.org>）取得專案，完整步驟見[開始使用](docs/01-getting-started/index.md)。Windows 請使用 WSL，並依照下方 WSL / Linux 的指令操作。

| 專案 | 用途 |
|---|---|
| `GridPlusPlus/GridPlusPlus-Pacman` | 第一個專案，跟著「從零做出 Pac-Man」教學使用。 |
| `GridPlusPlus/GridPlusPlus-Template` | 空白範本，學完教學後做自己的遊戲。 |

安裝 raylib 並 fork、clone 其中一個專案後，在專案根目錄執行：

### WSL / Linux

```bash
g++ -std=c++17 main.cpp -o game \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```

### macOS

```bash
g++ -std=c++17 main.cpp -o game $(pkg-config --cflags --libs raylib)
./game
```

## 文件

[`docs/`](docs/index.md) 以完全沒有經驗的初學者為對象：第 1 章從安裝工具、取得專案到上傳作品；第 2 章分六步從零做出 Pac-Man。第 3 章以後是進階內容，有系統地說明核心模型、打地鼠、Init／Update／Collide、物件狀態、繪製、完整版 Pac-Man 解析、生命週期與發布流程。

章節使用編號資料夾，節使用資料夾內的編號檔名。教學程式碼以 snippet 從 `examples/` 引入，所以文件與 CI 編譯的是同一份程式；多頁共用的段落放在 `docs_includes/`。API 參考由公開標頭檔中的 Doxygen 註解自動產生，與操作教學分開維護。完整範例位於 [`examples/`](examples/)。

## 專案結構

| 路徑 | 用途 |
|---|---|
| `GridPlusPlus.h` | 學生程式唯一需要引入的標頭。 |
| `GameEngine.h` | `GameEngine`、遊戲迴圈與實體管理。 |
| `GridObject.h`、`Overlay.h`、`GridMaze.h` | 三種 Handler 與對應的實體資料。 |
| `GridValue.h` | `set()`、`get()` 使用的型別安全儲存。 |
| `GridAssetManager.h`、`GridSQLite.h` | 素材包載入與 texture 管理。 |
| `template.cpp` | 可直接編譯的起始程式，同步為 GridPlusPlus-Template 的 `main.cpp`。 |
| `template/README.md` | 同步為 GridPlusPlus-Template 的 `README.md`。 |
| `examples/pacman_tutorial/` | 「從零做出 Pac-Man」的起始程式、素材與每一步的參考答案，同步為 GridPlusPlus-Pacman。 |
| `examples/` | Pac-Man 教學、打地鼠與完整版 Pac-Man 範例。 |
| `tools/create_asset_pack.py` | 將 32×32 PNG 轉成素材包。 |
| `docs/` | 教學文件原始檔。 |
| `docs_includes/` | 多個文件頁面共用的段落（以 snippet 引入，不是獨立頁面）。 |
