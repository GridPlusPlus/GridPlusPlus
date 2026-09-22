# 第一個 Grid++ 程式

建立 `main.cpp`：

```cpp
#include "GridPlusPlus.h"

using gridpp::GameEngine;

int main() {
    GameEngine game(8, 8, 64);
    game.showGrid(true);
    game.run();
    return 0;
}
```

`GameEngine game(8, 8, 64)` 建立 8 欄、8 列、每格 64 像素的遊戲。`showGrid(true)` 顯示參考格線；
格線只影響畫面，不會自動建立牆。`run()` 會先執行所有 Init 函式，再開始逐幀更新、碰撞與繪製，
直到視窗關閉。

專案根目錄的 `template.cpp` 就是這份最小程式，可直接複製後開始修改。

!!! note
    宣告順序很重要：`GameEngine` 必須在它的 Handler 仍會被使用時保持存在。一般把 `GameEngine` 放在
    `main()` 開頭並在最後呼叫 `run()` 即可。
