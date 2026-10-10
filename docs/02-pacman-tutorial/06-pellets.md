# 第 6 步：吃豆子

最後一塊拼圖！地圖上的每個 `.` 都變成一顆豆子。吃一顆加 10 分，**全部吃光就贏了**；被鬼抓到還是會輸。

![第 6 步的畫面：迷宮裡擺滿豆子，小精靈吃掉了幾顆，左上角顯示 Score: 40](../images/tutorial/step6.png)

## 完整程式

```cpp title="main.cpp" linenums="1" hl_lines="3 32-34 48-64 142-148 161"
--8<-- "examples/pacman_tutorial/steps/step6.cpp"
```

--8<-- "docs_includes/run-command.md"

你能拿到幾分？吃得完嗎？

## 程式怎麼運作

### 地圖上的點變成豆子

```cpp
} else if (kMap[y][x] == '.') {
    GridObject pellet = game.addObject("pellet", nullptr, nullptr, EatPellet);
    pellet.setPosition(x, y);
    pellets_left++;
}
```

第 3 步就畫在地圖上的 `.`，現在終於派上用場了。每遇到一個 `.` 就放一顆豆子，同時用 `pellets_left` 數總共有幾顆。小精靈的起點是 `P`，所以那一格沒有豆子。

### 吃掉豆子

豆子也有自己的碰撞函式 `EatPellet`。小精靈走到豆子上時：

1. 先確認撞到豆子的是小精靈。鬼經過的話不算數。
2. `self.remove()` 把這顆豆子從遊戲中拿掉。
3. 加分，並更新左上角的分數。
4. 剩下的豆子少一顆；變成 0 就呼叫 `Win()`。

`Win()` 和 `GameOver()` 幾乎一樣，只是顯示 YOU WIN!。兩者都把 `game_over` 設成 `true`，讓所有角色停下來。

## 🎉 完成了！

你剛剛從零做出了一個完整的 Pac-Man。回頭看看，你學會了：

- [x] 每一幀執行的 Update 函式
- [x] 讀取鍵盤，用 `game.time()` 計算時間
- [x] 用陣列畫地圖、判斷牆壁
- [x] 碰撞函式
- [x] 讓鬼自己做決定
- [x] 分數與勝負

別忘了[上傳到 Gitea](../01-getting-started/05-save.md)，把你的成果存起來！

## 試試看

- 在地圖上加入「大力丸」（例如用 `o` 表示）：吃到後 5 秒內換小精靈追鬼，抓到鬼可以加 200 分。
- 被抓到不會馬上輸：給小精靈 3 條命，被抓到就回到起點。
- 吃完豆子後進入第二關：換一張新地圖，鬼也變得更快。
- 畫一張完全屬於你的地圖，讓同學來挑戰！

## 接下來

- **做自己的遊戲**：Gitea 上的 [GridPlusPlus-Template](https://git.gridplusplus.ntuee.org/GridPlusPlus/GridPlusPlus-Template) 是一個空白的起點。用[取得你的專案](../01-getting-started/03-get-project.md)教過的方法 fork 它，把網址裡的 `GridPlusPlus-Pacman` 換成 `GridPlusPlus-Template`，就能開始做一個全新的遊戲。
- **深入了解 Grid++**：後面的章節會更完整地介紹 Grid++ 的設計，還有另一個小遊戲「打地鼠」，以及有開始畫面、暫停和四隻鬼的完整版 Pac-Man。想知道某個功能怎麼用的時候，再回來翻就可以了。

[深入了解 Grid++](../03-core-model/index.md){ .md-button .md-button--primary }
