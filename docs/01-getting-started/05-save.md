# 把作品存到 Gitea

在 VS Code 按存檔，只會存在你自己的電腦裡。把程式**上傳到 Gitea**，換了電腦也拿得回來，助教也看得到你的進度。

## 上傳

在專案資料夾的終端機貼上（`"Change hello text"` 可以換成任何描述這次修改的文字）：

```bash
git add -A
git commit -m "Change hello text"
git push
```

接著打開 Gitea 上你自己的專案頁面，重新整理。點開 `main.cpp`，看得到剛才的修改就成功了。

!!! tip "養成好習慣"

    每完成一個步驟、或是準備休息的時候，就上傳一次。上傳的次數再多都沒關係。

??? info "這三行在做什麼？"

    Git 會幫專案記錄一個個「存檔點」：

    1. `git add -A`：把所有修改過的檔案放進這次的存檔。
    2. `git commit -m "..."`：建立一個存檔點，引號裡是這次修改的說明。
    3. `git push`：把電腦上的存檔點上傳到 Gitea。

    之後就算程式改壞了，也可以回到以前的任何一個存檔點。

## 常見問題

??? question "`git commit` 出現 `nothing to commit`"

    代表從上次上傳到現在，沒有任何檔案被修改。確認 VS Code 裡有沒有存檔（分頁標題上的圓點代表還沒存）。

??? question "`git commit` 出現 `Please tell me who you are`"

    還沒設定 Git。請回到[取得你的專案](03-get-project.md)做第 3 步。

??? question "`git push` 出現 `rejected`"

    Gitea 上有電腦裡沒有的新存檔，常見於你在另一台電腦上傳過。先執行 `git pull`，再執行一次 `git push`。

## 準備好了！

到這裡，你已經有：

- [x] 可以寫 C++ 的電腦
- [x] 會動的 Grid++ 程式
- [x] 存在 Gitea 上的專案

下一章，我們要讓這隻小精靈動起來，一路做出完整的 Pac-Man！

[從零做出 Pac-Man](../02-pacman-tutorial/index.md){ .md-button .md-button--primary }
