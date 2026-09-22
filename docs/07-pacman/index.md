# 解析 Pac-Man

`examples/pacman_easy/` 與 `examples/pacman/` 都只使用 `GameEngine`、Handler、普通函式與全域變數。
入門版只有一隻鬼；完整版加入外部地圖、多隻鬼、開始／暫停／重玩按鈕及每實例狀態。

這個範例串起四個重點：

1. `Maze` 保存牆並提供移動前查詢。
2. `GridObject` 表示玩家、鬼與豆子。
3. `set/get` 讓共用 Update 的多隻鬼各自保存資料。
4. `Overlay` 顯示狀態並接受按鈕點擊。

完整可編譯版本以 `examples/pacman/main.cpp` 為準。
