# Overlay

Overlay 使用像素座標，永遠畫在網格內容上方。圖片、文字與按鈕都回傳同一種 `Overlay`：

```cpp
auto logo = game.addOverlay("logo");
logo.setPosition(20, 20);

auto score = game.addTextOverlay("Score: 0", 12, 12, 24, BLACK);
score.setText("Score: 10");

auto restart = game.addButton("Restart", 120, 180, 120, 40, RestartGame);
```

Overlay 共用 `setPosition()`、`move()`、`show()`、`hide()`、`remove()` 與狀態儲存功能。
圖片使用 `setImage()`；文字和按鈕使用 `setText()`。在錯誤種類上呼叫內容專用函式會清楚報錯。

`setInitFunction()` 與 `setUpdateFunction()` 適用所有 Overlay；`setClickFunction()` 只適用按鈕。
隱藏的按鈕仍可更新顯示條件，但不會接受點擊。
