# 每幀執行順序

每一幀順序固定：

```text
1. GridObject 與 Maze Update
2. 可見 GridObject 的同格碰撞
3. Overlay Update；可見按鈕處理點擊
4. 依 layer 繪製 GridObject 與 Maze
5. 依加入順序繪製 Overlay
6. 依序執行本幀新增元素的 Init
```

Update 修改的位置會影響同一幀的碰撞；修改 layer 也會影響同一幀繪製。碰撞不依賴 layer。

隱藏物件與 Overlay 仍 Update。隱藏物件不碰撞或繪製；隱藏 Button 不接受點擊。已排定移除者會被
後續階段跳過。

呼叫 `game.run()` 時，既有世界內容先 Init，Overlay 隨後 Init，全部完成後第一幀才開始。Init 又建立
內容時會分成下一批，仍依世界內容、Overlay 的順序初始化。
