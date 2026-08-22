# 第一個 Grid++ 程式

我們先建立一個只有網格的程式。這個版本沒有玩家、敵人或遊戲規則，但它能驗證編譯器、raylib 與 Grid++ 已經正確連接。

## 取得專案範本

從課程提供的 Grid++ template repository 建立遊戲專案，再將專案 clone 或下載到電腦。後續指令都在包含 `main.cpp` 與 Grid++ headers 的專案根目錄執行。

template repository 會包含 `main.cpp` 與全部 Grid++ headers。原生 Windows MinGW-w64 還需要把上一節下載的 raylib `include` 與 `lib` 放進專案；下圖只列出和這項設定直接相關的檔案：

```text
your-game/
|-- raylib/
|   |-- include/
|   `-- lib/
|-- GridPlusPlus.h
|-- GridEngine.h
|-- GridObject.h
|-- Overlay.h
`-- main.cpp
```

WSL、Linux 與 macOS 已把 raylib 安裝到系統路徑，不需要專案內的 `raylib/` 資料夾。

## 建立遊戲世界

打開 `main.cpp`。內容如下：

```cpp title="main.cpp"
#include "GridPlusPlus.h"

using gridpp::GridEngine;

int main() {
    GridEngine game(8, 8, 64);
    game.set_show_grid(true);
    game.Run();
    return 0;
}
```

`#include "GridPlusPlus.h"` 讓程式可以使用 Grid++ 的核心類別。`GridEngine game(8, 8, 64)` 建立 8 欄、8 列的遊戲世界，每格寬高都是 64 像素，所以視窗大小是 512×512 像素。

`set_show_grid(true)` 顯示格線。格線只協助我們辨認座標，不會建立牆壁或限制移動。`Run()` 啟動遊戲主迴圈，程式會停留在這一行，直到視窗被關閉。

這裡只需要先理解 Engine 代表整個遊戲世界。第 3 章會再說明它如何管理物件、碰撞、繪製與資源。

## 編譯

在遊戲專案根目錄執行對應平台的指令：

=== "WSL / Linux"

    ```bash
    g++ -std=c++17 main.cpp -o game \
        -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    ```

=== "macOS"

    ```bash
    g++ -std=c++17 main.cpp -o game $(pkg-config --cflags --libs raylib)
    ```

=== "Windows / MinGW-w64"

    ```bash
    g++ -std=c++17 main.cpp -o game.exe \
        -Iraylib/include -Lraylib/lib -lraylib -lgdi32 -lwinmm
    ```

`-std=c++17` 指定 Grid++ 使用的 C++ 版本。`-o game` 設定輸出檔名，後面的參數負責連結 raylib 與平台圖形函式庫。

## 執行

=== "WSL / Linux / macOS"

    ```bash
    ./game
    ```

=== "Windows / MinGW-w64"

    ```bash
    ./game.exe
    ```

視窗中應該出現 8×8 的空白網格。關閉視窗後，`Run()` 回傳，`main()` 結束，Engine 會釋放資源並關閉 raylib。

如果編譯器回報找不到 `raylib.h`，請回到安裝章確認 header 路徑。如果連結階段出現 `undefined reference`，請確認編譯指令包含對應平台的 raylib 與系統 library。

現在我們已經有一個可執行的遊戲世界。下一章會在這個網格上加入第一個物件，並把它變成完整的打地鼠。

[製作打地鼠](../02-whack-a-mole/index.md){ .md-button .md-button--primary }
