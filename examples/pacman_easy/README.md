# Pac-Man 入門版

這個版本只有一名玩家與一隻鬼，地圖直接寫在 `main.cpp`。程式只使用 Handler、普通函式與少量
全域變數，適合在完整版之前閱讀。

角色以 `game.addObject()` 建立，迷宮以 `game.addMaze()` 建立；碰撞函式收到兩個
`GridObject`，不會出現指標或 `->`。

```bash
g++ -std=c++17 main.cpp -I../.. -o game \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```

請在本資料夾執行，讓程式能找到 `pacman.db`。以上是 WSL／Linux 的指令；macOS
請把連結參數換成[第一個 Grid++ 程式](../../docs/01-getting-started/02-hello-gridpp.md)列出的對應版本，並保留 `-I../..`。
