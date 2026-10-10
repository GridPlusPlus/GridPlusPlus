# 第 2 步：小精靈出發

節奏練夠了，該讓小精靈出門了！這一步用方向鍵控制牠，按一下走一格，而且嘴巴會朝著前進的方向。

![第 2 步的畫面：黑色畫面中，小精靈嘴巴朝下](../images/tutorial/step2.png)

## 完整程式

這一步不再需要節奏遊戲，所以程式反而變短了。

```cpp title="main.cpp" linenums="1" hl_lines="7-24 31"
--8<-- "examples/pacman_tutorial/steps/step2.cpp"
```

--8<-- "docs_includes/run-command.md"

## 程式怎麼運作

### 網格座標

Grid++ 把畫面切成一格一格的。這個遊戲有 15 欄、11 列，左上角是 `(0, 0)`，**x 往右變大，y 往下變大**。

```cpp
pacman.setPosition(7, 5);  // 第 8 欄、第 6 列，剛好在正中央
```

### 移動

```cpp
self.move(1, 0);  // x 加 1，往右走一格
```

`move(dx, dy)` 讓物件從現在的位置移動 `dx` 欄、`dy` 列。往上是 `move(0, -1)`，因為 y 往上是變小。

`MovePacman` 和上一步的 `Dance` 一樣，每一幀都會被執行，所以只要檢查四個方向鍵有沒有被按下就好。

## 試試看

- 一直往右走，小精靈會跑出畫面嗎？（會！下一步會用牆壁把牠擋住。）
- 讓按一次走兩格。
- 把 `keyPressed` 改成 `keyDown`，按住不放會發生什麼事？為什麼小精靈一下子就跑不見了？（提示：一秒有 60 幀。）

--8<-- "docs_includes/stuck.md"

??? info "想知道更多？"

    網格座標和像素座標的差別，請看[理解核心模型](../03-core-model/index.md)。

[第 3 步：迷宮](03-maze.md){ .md-button .md-button--primary }
