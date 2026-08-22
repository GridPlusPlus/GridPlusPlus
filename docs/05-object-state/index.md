# 物件狀態

打地鼠只有一隻地鼠，因此一個全域 `next_move` 就能記錄下次移動時間。若產生三隻地鼠，三個物件會呼叫同一個 `UpdateMole`，也會讀寫同一個 `next_move`。第一隻地鼠更新時間後，另外兩隻會看到相同數值，三隻地鼠無法擁有獨立的移動週期。

```cpp
double next_move = 0.0;

void UpdateMole(GridObject* self) {
    if (GetTime() < next_move) return;
    // 三隻地鼠都修改同一個 next_move。
    next_move = GetTime() + 1.0;
}
```

這個問題來自資料的歸屬。分數和遊戲結束時間屬於整局遊戲，可以共用；下一次移動時間屬於單一地鼠，每個 instance 都需要一份。用陣列和物件指標建立對照表雖然可行，卻會讓狀態遠離使用它的行為，新增或刪除物件時也必須同步維護。

C++ class 可以讓每個物件保存自己的成員變數。下一節會把地鼠重構成繼承 `GridObject` 的 `Mole`，讓每次 `new Mole(...)` 都建立獨立的計時器。完成這個重構後，Pacman 的玩家方向、鬼的移動計時器與豆子的已食用狀態都能用相同方式表示。
