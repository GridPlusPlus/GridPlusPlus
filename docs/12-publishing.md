# 使用 Git 保存專案

每完成一個可執行階段，使用 Git 建立一筆 commit。先檢查目前變更，再加入本次需要保存的來源檔、素材與地圖。

```bash
git status
git add main.cpp assets.db map.txt
git commit -m "Add player movement"
```

`git status` 應在 `git add` 前後各執行一次。第一次確認工作目錄包含哪些變更，第二次確認即將寫入 commit 的範圍。不要使用 `git add .` 代替檢查，否則容易同時加入編譯輸出或無關檔案。

## 忽略編譯輸出

`game`、`game.exe` 和其他編譯產物可由來源程式重新建立，不應加入 repository。將各平台的輸出名稱加入 `.gitignore`：

```gitignore
game
game.exe
*.o
```

Header、`.cpp`、素材包、地圖與建置設定則是重現遊戲所需的輸入，應納入版本控制。提交前可使用 `git diff --cached` 檢查實際內容。

## 推送至遠端 repository

建立 commit 後，將目前 branch 推送到已設定的 remote：

```bash
git push
```

第一次推送新 branch 時，Git 可能要求設定 upstream。依終端機顯示的 branch 名稱執行 `git push -u origin <branch>`。推送只會傳送 commit，不會包含尚未 commit 的工作目錄變更。

部署 Web demo 需要額外的 WebAssembly 建置與託管 workflow。這項流程不是 Grid++ 核心 API 的一部分，應使用課程或專案提供的部署設定；不要直接把桌面版 `game` 執行檔上傳為網頁。
