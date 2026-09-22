# 製作打地鼠

這個範例把目前的 API 組成第一個完整遊戲。完成版位於 `examples/whack_a_mole/main.cpp`。

## 1. 建立會移動的地鼠

```cpp
double next_move = 0.0;

void UpdateMole(gridpp::Game game, gridpp::ObjectHandler mole) {
    if (game.time() < next_move) return;

    mole.setPosition(
        game.random(0, game.cols() - 1),
        game.random(0, game.rows() - 1)
    );
    next_move = game.time() + 1.0;
}
```

在 `main()` 註冊這個函式：

```cpp
gridpp::Game game(8, 8, 64);
game.showGrid(true);
game.addObject("mole", nullptr, UpdateMole);
```

## 2. 顯示與更新分數

```cpp
int score = 0;
gridpp::OverlayHandler score_label;

score_label = game.addTextOverlay("Score: 0", 12, 12, 24);
```

在 Update 中把滑鼠像素換成格子；點中地鼠就更新文字：

```cpp
const int mouse_x = game.mouseX() / game.gridSize();
const int mouse_y = game.mouseY() / game.gridSize();
if (game.mousePressed(MOUSE_BUTTON_LEFT) && mouse_x == mole.x() && mouse_y == mole.y()) {
    ++score;
    score_label.setText("Score: " + std::to_string(score));
    next_move = 0.0;
}
```

## 3. 用 Init 開始一局

Init 在 `run()` 之後、第一幀之前執行，適合設定依賴遊戲時間的初值：

```cpp
void InitMole(gridpp::Game game, gridpp::ObjectHandler mole) {
    score = 0;
    end_time = game.time() + 30.0;
    mole.show();
}

game.addObject("mole", InitMole, UpdateMole);
game.run();
```

時間歸零時呼叫 `mole.hide()`；按 R 時可直接呼叫重設函式並 `show()`。整個流程沒有指標，
所有實體都由 `Game` 管理。
