# 解析 Pac-Man

`examples/pacman_easy/` 與 `examples/pacman/` 都只使用 `Game`、Handler、普通函式與全域變數。
入門版只有一隻鬼；完整版加入外部地圖、多隻鬼、開始／暫停／重玩按鈕及每實例狀態。

這個範例串起四個重點：

1. `MazeHandler` 保存牆並提供移動前查詢。
2. `ObjectHandler` 表示玩家、鬼與豆子。
3. `set/get` 讓共用 Update 的多隻鬼各自保存資料。
4. `OverlayHandler` 顯示狀態並接受按鈕點擊。

完整可編譯版本以 `examples/pacman/main.cpp` 為準。
