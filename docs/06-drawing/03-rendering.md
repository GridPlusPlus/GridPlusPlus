# 外觀與繪製順序

`ObjectHandler` 保存物件外觀：

```cpp
player.setDirection(1);  // 逆時針旋轉 90 度
player.setColor(RED);
player.setLayer(10);
player.hide();
player.show();
```

方向會正規化至 0～3。`layer()` 較大的物件較晚畫，因此會覆蓋較小者；相同 layer 保持加入順序。
迷宮與一般物件共同遵守加入順序，Overlay 則永遠在網格內容完成後才繪製。

隱藏物件仍執行 Update，但不繪製也不參與碰撞。這適合暫停顯示後再次出現；若實體已不再需要，
使用 `remove()`。

`OverlayHandler` 也有 `show()`、`hide()` 與 `setColor()`。圖片 Overlay 的顏色是 tint；文字與按鈕
則用來設定文字顏色。
