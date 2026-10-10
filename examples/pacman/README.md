# Pac-Man 完整範例

這是「從零做出 Pac-Man」教學版的完整版，寫法和教學版相同，只用 `GameEngine`、`GridObject`、`Maze`、
`Overlay` 與普通函式，沒有自訂角色 class、繼承、指標或 `new`。

教學版之外，多了外部 `map.txt`、持續移動與方向緩衝、四隻各有獨立狀態的鬼，以及開始、暫停、勝敗與重玩。
各角色的方向與下一次移動時間透過 `set/get` 存在自己身上。

## 編譯

在本資料夾執行；`map.txt` 與 `pacman.db` 必須留在工作目錄。

```bash
g++ -std=c++17 main.cpp -I../.. -o game \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```

以上是 WSL／Linux 的指令；macOS 請把連結參數換成
[執行第一個程式](../../docs/01-getting-started/04-first-run.md)列出的對應版本，並保留 `-I../..`。
逐段解說見教學文件第 8 章〈解析 Pac-Man〉。

`map.txt` 和教學版的地圖使用相同符號，每行是一列：`#` 牆、`.` 豆子、`P` 小精靈起點、`G` 鬼的起點。
