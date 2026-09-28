# 第一個 Grid++ 程式

我們先建立一個只有網格的程式。這個版本沒有玩家、敵人或遊戲規則，但可以用來確認編譯器、raylib 與 Grid++ 都已設定完成，程式也能正常編譯、連結與執行。

## 取得專案範本

前往 [GridPlusPlus-Template](https://github.com/GridPlusPlus/GridPlusPlus-Template)，按 **Fork** 建立自己的遊戲專案。接著從自己 fork 的 **Code** 選單複製網址並執行：

```bash
git clone https://github.com/YOUR_ACCOUNT/YOUR_GAME.git
cd YOUR_GAME
```

請把 `YOUR_ACCOUNT` 與 `YOUR_GAME` 換成自己 fork 的帳號和專案名稱。後續指令都在包含 `main.cpp` 與 Grid++ 標頭檔的專案根目錄執行；執行 `ls`（Windows 可用 `dir`）時，應能看到 `main.cpp` 與 `GridPlusPlus.h`。

專案範本包含 `main.cpp` 與全部 Grid++ 標頭檔。原生 Windows MinGW-w64 還需要把上一節下載的 raylib `include` 與 `lib` 放進專案；下面只列出和這項設定直接相關的檔案：

```text
your-game/
|-- raylib/
|   |-- include/
|   `-- lib/
|-- GridPlusPlus.h
|-- GameEngine.h
|-- GridObject.h
|-- Overlay.h
|-- ...（其他 Grid++ 標頭檔）
`-- main.cpp
```

WSL、Linux 與 macOS 已把 raylib 安裝到系統路徑，不需要專案內的 `raylib/` 資料夾。

程式只需要引入 `GridPlusPlus.h`，其他標頭檔會由它自動引入；但它們仍必須和 `main.cpp` 放在同一個資料夾，編譯器才找得到。

## 建立遊戲世界

打開 `main.cpp`。內容如下：

```cpp title="main.cpp"
#include "GridPlusPlus.h"

using gridpp::GameEngine;

int main() {
    GameEngine game(8, 8, 64);
    game.showGrid(true);
    game.run();
    return 0;
}
```

`GameEngine` 是 Grid++ 中管理整個遊戲的核心類別，可以先把它理解成「遊戲世界的控制者」。它負責建立視窗、保存之後加入的遊戲內容，並持續安排更新與繪製。這裡建立的 `game` 是一個 `GameEngine` 變數，代表這個程式中的遊戲世界。

先逐行看這個程式做了什麼：

1. `#include "GridPlusPlus.h"` 引入 Grid++ 的標頭檔，讓程式可以使用其中定義的類別與函式。
2. `using gridpp::GameEngine;` 讓後面可以直接寫 `GameEngine`，而不必每次都寫完整名稱 `gridpp::GameEngine`。Grid++ 的名稱都放在 `gridpp` 命名空間（namespace）中，避免和你自己取的名稱衝突。
3. `GameEngine game(8, 8, 64)` 建立 8 欄、8 列、每格 64 像素的世界，因此視窗大小是 512×512 像素。
4. `game.showGrid(true)` 要求 Engine 顯示格線。
5. `game.run()` 啟動持續更新與繪製的主迴圈，直到視窗被關閉。

目前這個世界還沒有玩家、敵人或牆壁，只有 Engine 畫出的參考格線。`showGrid(true)` 只改變畫面外觀，方便我們看出每一格的位置；它不會替遊戲建立地圖，也不會阻止物件跨越格線。這些遊戲內容與移動規則會在後面的章節由程式加入。

呼叫 `run()` 可以看成遊戲從「準備」進入「運行」的分界。在這個範例中，`run()` 前只有 Engine 本身與格線設定；之後的程式還會在這個階段載入素材、建立物件並加入遊戲的初始內容。進入 `run()` 後，Engine 便持續更新狀態與重畫畫面，桌面版會一直執行到玩家關閉視窗，之後 `main()` 才繼續往下並結束。

Grid++ 專案根目錄的 [`template.cpp`](https://github.com/GridPlusPlus/GridPlusPlus/blob/main/template.cpp) 就是這份最小程式；專案範本中的 `main.cpp` 也是由它同步而來。

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

`-std=c++17` 指定 Grid++ 使用的 C++ 版本。`-o game` 設定輸出檔名，後面的參數負責連結 raylib 與平台圖形函式庫。之後每一章的程式都使用同一組指令編譯；只有要編譯其他資料夾中的檔案時（例如 `examples/` 內的範例），才需要另外加上 `-I` 告訴編譯器 Grid++ 標頭檔的位置。

## 執行

=== "WSL / Linux / macOS"

    ```bash
    ./game
    ```

=== "Windows / MinGW-w64"

    ```bash
    ./game.exe
    ```

視窗中應該出現 8×8 的空白網格，其座標從左上角 `(0, 0)` 開始，x 向右、y 向下增加。

<figure markdown="span">
  ![8×8 網格座標，左上角為零零，三二位置以藍色標示](../images/grid-coordinates.svg)
  <figcaption>格線只協助辨認座標；圖中的藍色格位於 x=3、y=2。</figcaption>
</figure>

關閉視窗後，`run()` 回傳，`main()` 隨後結束。`game` 是 `main()` 中的區域變數，它離開作用域時，Engine 會自動釋放遊戲內容、圖片與視窗資源；程式不需要、也不應該自己做任何清理。

## 遇到錯誤時

錯誤訊息出現的時機，可以告訴我們問題位在哪一個步驟：

| 現象 | 通常的原因 | 處理方式 |
| --- | --- | --- |
| `fatal error: raylib.h: No such file or directory` | 編譯器找不到 raylib 標頭檔 | 回到[安裝章](01-installation.md)確認 raylib 已安裝；MinGW-w64 請確認 `raylib/include` 的位置 |
| `fatal error: GridPlusPlus.h: No such file or directory` | 目前目錄不是專案根目錄 | 用 `ls`／`dir` 確認 `GridPlusPlus.h` 與 `main.cpp` 在同一層 |
| `undefined reference to ...` | 編譯成功，但連結階段找不到 raylib | 確認指令包含對應平台的 raylib 與系統函式庫參數 |
| 可以編譯，執行時卻沒有視窗 | 圖形環境無法開啟視窗 | WSL 請確認能顯示 Linux 圖形程式 |

## 動手試試看

在進入下一章前，修改建構子的三個數字並重新編譯執行：前兩個數字改變欄、列數，第三個數字改變每格的像素大小。每次執行前，先預測視窗會變成多大、格線會有幾條，再用畫面驗證。只要能預測結果，就已理解這個最小程式。

如果把數字改成 `GameEngine game(200, 200, 64)`，程式會在啟動時丟出例外並結束，因為視窗寬度 `200 × 64 = 12800` 像素超過 Grid++ 允許的 8192 像素上限；三個數字也都必須大於 0。

[理解 Grid++ 核心模型](../02-core-model/index.md){ .md-button .md-button--primary }
