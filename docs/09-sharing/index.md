# 從本機成果到可重現的專案

遊戲能在自己的電腦上執行，不代表別人取得專案後也能得到相同結果。真正需要保存的不只有 `main.cpp`，還包括標頭檔、地圖、素材包與建置設定；由編譯產生的執行檔與中間檔則可以重新建立，不應混入版本紀錄。本章沿著 Git 的資料流依序處理工作目錄、暫存區、版本提交（commit）與遠端儲存庫，最後再用乾淨複本驗證專案。

## 先決定哪些檔案構成遊戲

標頭檔、`.cpp`、素材包、地圖與建置設定都是重現遊戲所需的輸入，應納入版本控制。`game`、`game.exe`、`.o` 等編譯產物則應寫入 `.gitignore`，使 Git 從一開始就不把它們列為待保存內容。

```gitignore title=".gitignore"
game
game.exe
*.o
```

設定完成後先執行 `git status`，此時看到的是工作目錄中尚未選入下一筆 commit 的變更。若清單仍出現編譯產物，應先修正 `.gitignore`，而不是直接把整個目錄加入暫存區。

## 從工作目錄建立一筆版本提交

`git add` 不等於永久保存，它只是把指定檔案目前的內容放入暫存區，準備成為下一筆版本提交。應明確列出這個階段真正完成的來源、素材與地圖，再次執行 `git status` 確認「Changes to be committed」的範圍，並用 `git diff --cached` 閱讀即將寫入紀錄的文字變更。

```bash
git status
git add main.cpp assets.db map.txt .gitignore
git status
git diff --cached
git commit -m "Add playable Pacman level"
```

刻意列出檔名，可以避免把測試檔、個人筆記或尚未完成的修改一起送進版本提交。提交完成後再執行一次 `git status`，理想結果是工作目錄乾淨；如果仍有變更，應判斷它們屬於下一個工作階段，還是剛才漏掉的必要檔案。

## 把本機提交推送到遠端

遠端儲存庫保存的是版本提交，不會包含工作目錄中尚未提交的內容，因此推送前應先確認目前分支與狀態，再執行 `git push`。第一次推送新分支時，Git 可能要求設定上游分支；此時依終端機顯示的實際分支名稱執行它建議的 `git push -u origin ...`，後續的 `git push` 才知道要送往哪個遠端分支。

```bash
git status
git branch --show-current
git push
```

## 從乾淨複本驗證

推送成功只證明遠端收到了版本提交，還沒有證明另一台電腦能建立遊戲。真正的分享驗收要在原專案之外建立一個全新的資料夾，從遠端重新複製，再依第 1 章的同一套平台指令編譯；`game-check` 必須是尚不存在的名稱，以免混入舊檔案。

```bash
cd ..
git clone https://github.com/YOUR_ACCOUNT/YOUR_GAME.git game-check
cd game-check
git rev-parse HEAD
cd examples/pacman
g++ -std=c++17 main.cpp -I../.. -o game \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```

請先替換網址中的 `YOUR_ACCOUNT` 與 `YOUR_GAME`。`git rev-parse HEAD` 顯示的提交識別碼應與剛才推送的最新提交相同；macOS 與 MinGW-w64 使用第 1 章列出的對應連結參數。無論平台為何，都要從 `examples/pacman/` 執行，讓相對路徑能找到 `map.txt` 與 `pacman.db`。

驗收不能停在視窗開啟，還要確認素材不是紅色替代方塊，而且開始、移動、吃豆、勝敗、暫停與重新開始都能運作。只有這些輸入都能由遠端儲存庫重建，專案才算真正可分享。

## 同步上游 Grid++ 更新

在 GitHub fork 首頁按下 **Sync fork**，更新的是 GitHub 上的遠端 fork，不會自動改變電腦中已經複製的工作目錄。完成網頁同步後，仍需回到本機；先確認 `git status` 沒有尚未處理的修改，再從自己的遠端分支取回新提交。

```bash
git status
git pull --ff-only
```

`--ff-only` 只在本機分支可以直接快轉時更新。若本機與遠端各自產生不同提交，命令會停止而不擅自建立合併結果，讓作者先看清兩邊差異。直接修改 Grid++ 標頭檔會提高衝突機會，因此遊戲規則應盡量集中在自己的 `main.cpp` 與自訂檔案中；發生衝突時先閱讀 Git 指出的檔案並確認要保留的內容，不應刪除本機歷史來換取表面上的同步成功。

## 分享前檢查

- `git status` 顯示沒有遺漏或意外加入的檔案。
- 最新提交包含來源、素材、地圖與必要設定，但不包含編譯輸出。
- 遠端分支已收到最新提交，而不是只在本機修改完成。
- 從另一個乾淨目錄重新複製後，可以依文件編譯並從指定目錄執行。
- 實際遊玩能載入素材與地圖，主要輸入、碰撞、勝敗及重新開始流程均符合預期。

這份檢查把「程式在作者電腦上跑過」提升為「專案能由已保存的輸入重新建立」，也使 Git 不只是備份工具，而成為驗證文件、來源與資源是否共同描述同一份作品的方法。
