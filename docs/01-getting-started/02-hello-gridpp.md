# 第一個 Grid++ 程式

我們先建立一個只有網格的程式。這個版本沒有玩家、敵人或遊戲規則，但它能驗證編譯器、raylib 與 Grid++ 已經正確連接。

## 取得專案範本

前往 [GridPlusPlus-Template](https://github.com/GridPlusPlus/GridPlusPlus-Template)，按 **Fork** 建立自己的遊戲專案，再從自己 fork 的 **Code** 選單複製網址並執行：

```bash
git clone <你的-fork-網址>
cd <專案資料夾>
```

`<...>` 表示要換成自己的內容，不要連同角括號輸入。後續指令都在包含 `main.cpp` 與 Grid++ headers 的專案根目錄執行。執行 `ls`（Windows 可用 `dir`）時，應能看到 `main.cpp` 與 `GridPlusPlus.h`。

專案範本包含 `main.cpp` 與全部 Grid++ headers。原生 Windows MinGW-w64 還需要把上一節下載的 raylib `include` 與 `lib` 放進專案；下圖只列出和這項設定直接相關的檔案：

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

`GridEngine` 是 Grid++ 中管理整個遊戲的核心類別，可以先把它理解成「遊戲世界的控制者」。它負責建立視窗、保存之後加入的遊戲物件，並持續安排更新與繪製。這裡建立的 `game` 是一個 GridEngine instance，代表這個程式唯一的遊戲世界。

這個程式依序經過四個階段：

1. `#include "GridPlusPlus.h"` 取得 Grid++ 的核心類別。
2. `GridEngine game(8, 8, 64)` 建立 8 欄、8 列、每格 64 像素的世界，因此視窗大小是 512×512 像素。
3. `set_show_grid(true)` 在進入遊戲前完成顯示設定。
4. `Run()` 啟動持續更新與繪製的主迴圈，直到視窗被關閉。

目前這個世界還沒有玩家、敵人或牆壁，只有 Engine 畫出的參考格線。`set_show_grid(true)` 只改變畫面外觀，方便我們看出每一格的位置；它不會替遊戲建立地圖，也不會阻止物件跨越格線。這些遊戲內容與移動規則會在後面的章節由程式加入。

呼叫 `Run()` 可以看成遊戲從「準備」進入「運行」的分界。在它之前，程式先設定 Engine 並加入初始內容；進入 `Run()` 後，Engine 便持續更新狀態與重畫畫面。桌面版的 `Run()` 會一直執行到玩家關閉視窗，之後 `main()` 才繼續往下並結束。

`game` 是 `main()` 中的區域變數。當 `main()` 結束時，它會離開作用域，Engine 的解構函式便釋放遊戲物件、圖片與視窗資源。這裡只需要先掌握「建立 → 設定 → 執行 → 清理」；下一章會解釋 Engine 如何管理物件、碰撞與繪製。

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

在進入下一章前，可以修改建構子的三個數字並重新執行：前兩個數字改變欄、列數，第三個數字改變每格的像素大小。只要能預測視窗與格線如何改變，就已理解這個最小程式。

[理解 Grid++ 核心模型](../03-core-model/index.md){ .md-button .md-button--primary }
