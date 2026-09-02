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

文件先以普通函式完成打地鼠，再深入說明 `GridEngine`、`GridObject`、Overlay、callback、碰撞、物件狀態與繪製。Pacman 專案接著組合迷宮、自訂物件、素材與遊戲狀態；最後整理生命週期、所有權和每幀順序。

章節使用編號資料夾，節使用資料夾內的編號檔名。API reference 由公開 header 中的 Doxygen 註解自動產生，與操作教學分開維護。

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
| `examples/` | 打地鼠、Pacman callback 版與完整 Pacman。 |
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

CI 使用 raylib stub 編譯並執行 `tests/`，也會確認 template、打地鼠里程碑與兩個 Pacman 範例能以 C++17 編譯。實際圖形顯示仍應使用已安裝的 raylib 測試。

## 授權

Grid++ 採用 [MIT License](LICENSE)。raylib 使用 zlib/libpng License。
