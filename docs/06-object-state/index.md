# 讓每個物件保存自己的狀態

第 5 章已經讓普通函式處理更新與碰撞，但這種寫法還沒有回答一個問題：三隻地鼠若各有不同的移動速度，下一次移動時間應該存在哪裡？把它留在全域 `next_move` 中，三隻地鼠就會讀寫同一個數值，第一隻更新時間後，另外兩隻也被迫一起等待。

```cpp title="錯誤示意：三隻地鼠共用同一個 next_move"
double next_move = 0.0;

void UpdateMole(GameEngine game, GridObject self) {
    if (game.time() < next_move) return;
    // 三隻地鼠都修改同一個 next_move。
    next_move = game.time() + 1.0;
}
```

Pac-Man 教學第 5 步的挑戰「再加一隻鬼」也會遇到同一個問題：`ghost_direction` 與 `ghost_next_move` 是全域變數，第二隻鬼若共用同一個 `ChasePacman`，兩隻鬼就會搶著修改同一份方向和時間。

問題不在函式內容，而在資料歸屬：分數與遊戲結束時間屬於整局，可以共用；下一次移動時間屬於單一地鼠，每個物件都需要一份。用陣列替每隻地鼠各開一格雖然可行，卻會讓狀態遠離使用它的行為，新增或刪除物件時也必須同步維護陣列。

Grid++ 的做法是讓每個物件自己帶著一個小小的「資料袋」：用 `self.set("nextMove", ...)` 把數值存進目前這隻地鼠，下次輪到它時再用 `self.get("nextMove", ...)` 取回。三隻地鼠共用同一個 Update 函式，但因為 `self` 每次指向不同的物件，讀寫的也就是各自的資料。本章先用這個方式改寫地鼠，再說明 Init 如何準備每個物件的初始狀態，以及 Handler 被複製時會發生什麼事。

## 完成本章後

- 能說明為何三隻地鼠不能共用同一個 `next_move`。
- 能用 `set()` 與 `get()` 讓每個物件保存自己的計時器、方向或其他資料。
- 能依資料屬於整局還是單一物件，選擇全域變數或物件自己的狀態。
- 能分辨複製 Handler 與 `deepCopy()` 的差別。

[用 set 與 get 保存個別狀態](01-set-and-get.md){ .md-button .md-button--primary }
