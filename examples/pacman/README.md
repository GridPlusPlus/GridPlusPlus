# Pac-Man 完整範例

這個範例示範只用 `GameEngine`、`GridObject`、`Maze`、`Overlay` 與普通函式完成 Pac-Man。
沒有自訂角色 class、繼承、指標或 `new`。

功能包含外部 `map.txt`、自動拼接牆面、四隻各有獨立狀態的鬼、方向緩衝、豆子碰撞、開始、暫停、
勝敗與重玩。各角色額外資料透過 `set/get` 保存。

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

地圖第一行是 `rows cols`，後方每格為：`0` 豆子、`1` 牆、`2` 玩家、`3` 鬼。
