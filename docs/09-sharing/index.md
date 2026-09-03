# 從本機成果到可重現的專案

一個遊戲能在自己的電腦上執行，還不代表別人取得專案後也能得到相同結果，因為真正需要保存的不只有 `main.cpp`，還包括 headers、地圖、素材包與建置設定；相對地，由編譯產生的執行檔與中間檔可以重新建立，把它們一併保存反而會讓版本紀錄混入大量無法閱讀的變更。本章會沿著 Git 的實際資料流依序處理工作目錄、暫存區、commit 與遠端 repository，最後再用一份乾淨複本驗證專案是否真的可以分享。

## 先決定哪些檔案構成遊戲

Header、`.cpp`、素材包、地圖與建置設定都是重現遊戲所需的輸入，應納入版本控制；`game`、`game.exe`、`.o` 等編譯產物則應寫入 `.gitignore`，使 Git 從一開始就不把它們列為待保存內容。

```gitignore
game
game.exe
*.o
```

設定完成後先執行 `git status`，此時看到的是工作目錄中尚未選入下一筆 commit 的變更；若清單仍出現編譯產物，應先修正 `.gitignore`，而不是直接把整個目錄加入暫存區。

## 從工作目錄建立一筆 commit

`git add` 不等於永久保存，它只是把指定檔案目前的內容放入暫存區，準備成為下一筆 commit，因此應明確列出這個階段真正完成的來源、素材與地圖，再次執行 `git status` 確認「Changes to be committed」的範圍，並用 `git diff --cached` 閱讀即將寫入紀錄的文字變更。

```bash
git status
git add main.cpp assets.db map.txt .gitignore
git status
git diff --cached
git commit -m "Add playable Pacman level"
```

刻意列出檔名雖然比 `git add .` 多寫幾個字，卻能避免把測試檔、個人筆記或其他尚未完成的修改一起送進 commit。commit 完成後再執行一次 `git status`，理想結果是工作目錄乾淨；如果仍有變更，應判斷它們屬於下一個工作階段，還是剛才漏掉的必要檔案，而不是只因 commit 已成功便忽略它們。

## 把本機 commit 推送到遠端

遠端 repository 保存的是 commit，不會包含工作目錄中尚未 commit 的內容，因此推送前應先確認目前所在 branch 與狀態，再執行 `git push`。第一次推送新 branch 時，Git 可能要求設定 upstream，此時依終端機顯示的實際 branch 名稱執行 `git push -u origin <branch>`；設定完成後，後續的 `git push` 才知道要把目前 branch 送往哪個遠端分支。

```bash
git status
git branch --show-current
git push
```

推送成功只證明遠端收到了 commit，還沒有證明另一台電腦能建立遊戲，因此真正的分享驗收應從乾淨資料夾重新 clone，按照文件中的指令編譯，並從正確的工作目錄執行。以 Pacman 為例，除了視窗能開啟之外，還要確認 `map.txt` 與 `pacman.db` 均能由相對路徑找到，角色素材不是紅色 fallback 方塊，而且開始、移動、吃豆、勝敗、暫停與重新開始都能運作；只有這些輸入都能由 repository 重建，專案才算完整。

## 同步上游 Grid++ 更新

在 GitHub fork 首頁按下 **Sync fork**，更新的是 GitHub 上的遠端 fork，並不會自動改變電腦中已經 clone 的工作目錄；完成網頁同步後，仍需回到本機，先確認 `git status` 沒有尚未處理的修改，再從自己的遠端分支取回新 commit。

```bash
git status
git pull --ff-only
```

`--ff-only` 只在本機 branch 可以直接快轉時更新，若本機與遠端各自產生了不同 commit，命令會停止而不是擅自建立合併結果，讓作者先看清兩邊差異再決定如何處理。若曾直接修改 Grid++ headers，同步時更容易形成衝突，因此遊戲規則應盡量集中在自己的 `main.cpp` 與自訂檔案中；發生衝突時先停止、閱讀 Git 指出的檔案並確認要保留的內容，不應以刪除本機歷史的方式換取表面上的同步成功。

## 分享前檢查

- `git status` 顯示沒有遺漏或意外加入的檔案。
- 最新 commit 包含來源、素材、地圖與必要設定，但不包含編譯輸出。
- 遠端 branch 已收到最新 commit，而不是只在本機修改完成。
- 從另一個乾淨目錄 clone 後，可以依文件編譯並從指定目錄執行。
- 實際遊玩能載入素材與地圖，主要輸入、碰撞、勝敗及重新開始流程均符合預期。

這份檢查把「程式在作者電腦上跑過」提升為「專案能由已保存的輸入重新建立」，也使 Git 不只是備份工具，而成為驗證文件、來源與資源是否共同描述同一份作品的方法。
