# Grid++

Grid++ 是供程式設計入門課程使用的 C++17 網格遊戲函式庫。它處理視窗、遊戲迴圈、繪圖與碰撞，讓課程先使用函式與指標建立遊戲，再以 class、繼承與物件狀態處理更複雜的行為。

![Pacman 範例畫面](docs/images/preview.png)

## 主要內容

- `GridEngine`：管理視窗、遊戲迴圈、物件與碰撞。
- `GridObject`：存在網格中的玩家、敵人與道具。
- `Overlay`：使用像素座標，顯示在遊戲物件上方。
- `GridMaze`：選用的迷宮與牆面模組。
- `gridpp::shapes`：不需素材包的基本圖形。

Grid++ 是 header-only library，唯一的外部依賴是 [raylib](https://www.raylib.com/)。

## 快速執行

Windows 建議使用 WSL；想產生原生 Windows 執行檔時再使用 MinGW-w64。請先依照[安裝與建立專案](docs/02-install.md)準備函式庫，再執行對應指令。

### WSL / Linux

```bash
git clone https://github.com/GridPlusPlus/GridPlusPlus.git
cd GridPlusPlus
g++ -std=c++17 template.cpp -o game \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```

### Windows / MinGW-w64

```bash
git clone https://github.com/GridPlusPlus/GridPlusPlus.git
cd GridPlusPlus
g++ -std=c++17 template.cpp -o game.exe \
    -Iraylib/include -Lraylib/lib -lraylib -lgdi32 -lwinmm
./game.exe
```

### macOS

```bash
git clone https://github.com/GridPlusPlus/GridPlusPlus.git
cd GridPlusPlus
g++ -std=c++17 template.cpp -o game $(pkg-config --cflags --libs raylib)
./game
```

## 文件

文件以 Grid++ 功能為主線，依序說明核心架構、`GridEngine`、`GridObject`、Overlay、callback、自訂物件、素材、基本圖形、迷宮與物件生命週期。打地鼠與 Pacman 分散在相關章節中，用於展示 API 在完整情境中的組合方式。

文件檔名使用兩位數編號，與建議閱讀順序一致。API reference 由公開 header 中的 Doxygen 註解自動產生，不混入操作教學。

## 專案結構

| 路徑 | 用途 |
|---|---|
| `GridPlusPlus.h` | 一般遊戲使用的主 header。 |
| `GridEngine.h` | `GridEngine`。 |
| `GridObject.h` | `GridObject` 與 `CallbackGridObject`。 |
| `Overlay.h` | `Overlay`、`Label` 與 `Button`。 |
| `GridMaze.h` | 選用的迷宮模組。 |
| `GridShapes.h` | 選用的基本圖形。 |
| `template.cpp` | 可直接編譯的起始程式。 |
| `examples/` | Pacman 的 callback 與物件導向版本。 |
| `tests/` | 引擎、生命週期、迷宮、圖形與素材測試。 |
| `docs/` | MkDocs 教學文件。 |

## 預覽文件

需要 Python、Doxygen 與以下指令：

```bash
pip install -r docs/requirements.txt
mkdocs serve
```

瀏覽器開啟 `http://127.0.0.1:8000/`。MkDoxy 會在建置時一併更新 API 文件。

## 測試

CI 使用 raylib stub 編譯並執行 `tests/`，也會確認 template 與兩個 Pacman 範例能以 C++17 編譯。實際圖形顯示仍應使用已安裝的 raylib 測試。

## 授權

Grid++ 採用 [MIT License](LICENSE)。raylib 使用 zlib/libpng License。
