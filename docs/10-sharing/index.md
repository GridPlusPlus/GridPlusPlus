# 從本機成果到可重現的專案

第 1 章的[把作品存到 Gitea](../01-getting-started/05-save.md)只用三行指令就完成上傳，足以應付日常使用。本章說明這三行背後的流程，以及作品要分享給別人之前，還需要確認哪些事情。

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
git clone https://git.gridplusplus.ntuee.org/你的學號/你的專案.git game-check
cd game-check
git rev-parse HEAD
g++ -std=c++17 main.cpp -o game \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```

請先替換網址中的 `你的學號` 與 `你的專案`。`git rev-parse HEAD` 顯示的提交識別碼應與剛才推送的最新提交相同；macOS 使用第 1 章列出的對應連結參數。無論平台為何，都要從專案根目錄執行，讓 `loadAssets()` 與地圖檔的相對路徑能找到 `assets.db` 與 `map.txt`。

驗收不能停在視窗開啟，還要確認素材不是紅色替代方塊，而且遊戲的主要操作都能運作，例如 Pac-Man 的開始、移動、吃豆、勝敗、暫停與重新開始。只有這些輸入都能由遠端儲存庫重建，專案才算真正可分享。

## 同步上游 Grid++ 更新

你 fork 的專案（GridPlusPlus-Pacman 或 GridPlusPlus-Template）之後可能會收到 Grid++ 的更新，例如修正錯誤的標頭檔。Gitea 上你自己的專案頁面若顯示落後於原本的專案，會出現同步的按鈕；按下後，Gitea 會把新的提交合併進你的 fork。

網頁同步更新的是 Gitea 上的 fork，不會自動改變電腦中的檔案。完成網頁同步後，回到本機，先確認 `git status` 沒有尚未處理的修改，再取回新提交：

```bash
git status
git pull --ff-only
```

`--ff-only` 只在本機分支可以直接快轉時更新。若本機與 Gitea 各自產生不同提交，命令會停止而不擅自建立合併結果；這時先執行 `git push` 上傳本機的提交（若被拒絕，改執行 `git pull` 合併後再 `git push`）。

### 同步按鈕失敗時

如果你修改過的檔案剛好也在這次更新裡被修改（例如兩邊都改了 `main.cpp`），Gitea 無法自動決定要保留哪一邊，同步就會失敗。這時改在本機合併：

```bash
git pull https://git.gridplusplus.ntuee.org/GridPlusPlus/GridPlusPlus-Pacman.git main
```

使用範本的專案，請把網址換成 `GridPlusPlus-Template.git`。Git 會列出發生衝突的檔案；在 VS Code 打開它們，每個衝突處都可以選擇保留自己的版本、更新後的版本或兩者。存檔後執行：

```bash
git add -A
git commit -m "Merge Grid++ update"
git push
```

為了減少衝突，遊戲規則應盡量集中在自己的 `main.cpp` 與自訂檔案中，不要修改 Grid++ 的標頭檔。`main.cpp` 只有在 Grid++ 的起始程式改變時才會跟著更新，這種情況很少發生；若因此發生衝突，通常應保留自己的版本，再對照文件確認是否需要配合新版調整寫法。不確定時請先問助教，不要刪除本機歷史來換取表面上的同步成功。

## 分享前檢查

- `git status` 顯示沒有遺漏或意外加入的檔案。
- 最新提交包含來源、素材、地圖與必要設定，但不包含編譯輸出。
- 遠端分支已收到最新提交，而不是只在本機修改完成。
- 從另一個乾淨目錄重新複製後，可以依文件編譯並從指定目錄執行。
- 實際遊玩能載入素材與地圖，主要輸入、碰撞、勝敗及重新開始流程均符合預期。

這份檢查把「程式在作者電腦上跑過」提升為「專案能由已保存的輸入重新建立」，也使 Git 不只是備份工具，而成為驗證文件、來源與資源是否共同描述同一份作品的方法。
