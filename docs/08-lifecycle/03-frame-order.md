# 每幀執行順序

每一幀順序固定：

```text
1. 一般物件、Shapes、Maze Update
2. Overlay 與 Button Update；可見按鈕處理點擊
3. 可見一般物件與 Shapes 的同格碰撞
4. 依 layer 繪製一般物件、Shapes 與 Maze
5. 依加入順序繪製 Overlay
6. 套用延後移除與加入，執行新元素 Init
```

Update 修改的位置會影響同一幀的碰撞；修改 layer 也會影響同一幀繪製。碰撞不依賴 layer。

隱藏物件與 Overlay 仍 Update。隱藏物件不碰撞或繪製；隱藏 Button 不接受點擊。已排定移除者會被
後續階段跳過。

呼叫 `game.run()` 時，既有元素先按加入順序完成 Init，全部成功後第一幀才開始。Init 或 callback
丟出例外時，相關新實體會被移除，例外離開 `run()` 交給程式處理。
