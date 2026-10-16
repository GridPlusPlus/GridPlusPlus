# 取得你的專案

課程的程式都放在 **Gitea** 這個網站上。這一步要在 Gitea 上拿到一份屬於你的 Pac-Man 專案，再把它下載到電腦。

## 1. 登入 Gitea

1. 打開 <https://git.gridplusplus.ntuee.org>。
2. 帳號是你的**學號**，初始密碼由助教提供。
3. 第一次登入會要求你設定新密碼，請設定一組自己記得住的密碼。

!!! info "Gitea 上的作品只有修課同學看得到"

    沒有登入的人什麼都看不到。

## 2. Fork 一份 Pac-Man 專案

1. 打開 <https://git.gridplusplus.ntuee.org/GridPlusPlus/GridPlusPlus-Pacman>。
2. 按右上角的 **Fork**，下一頁直接按確認的按鈕。

完成後，網址會變成 `.../你的學號/GridPlusPlus-Pacman`。

??? info "Fork 是什麼？"

    Fork 會把整個專案複製一份到你的帳號底下。之後你修改的都是自己那一份，不會影響到別人，也不會改到原本的專案。

## 3. 設定 Git（只要做一次）

**Git** 是負責下載、保存程式的工具。打開終端機，貼上下面三行，**記得先把 `你的學號` 換成自己的學號**：

```bash
git config --global user.name "你的學號"
git config --global user.email "你的學號@ntu.edu.tw"
git config --global credential.helper store
```

這三行執行後不會有任何輸出，這是正常的。

??? info "這三行在做什麼？"

    前兩行告訴 Git「存檔的人是誰」，之後每次存檔都會記下你的學號和學校信箱。email 只用來標示作者，Git 不會寄信給你。

    第三行讓 Git 記住你的 Gitea 密碼，之後就不用每次輸入。密碼會存在電腦的 `~/.git-credentials` 檔案裡，所以請不要在公用電腦上執行這一行。

## 4. 把專案下載到電腦

同樣先把 `你的學號` 換掉，再貼到終端機：

```bash
cd ~
git clone https://git.gridplusplus.ntuee.org/你的學號/GridPlusPlus-Pacman.git
```

!!! tip "不想自己改網址？"

    在你 fork 出來的專案頁面上，有一個顯示 HTTPS 網址的框框，旁邊有複製按鈕。在終端機打 `git clone ` （後面有一個空格），再貼上複製的網址就可以了。

按下 ++enter++ 後，終端機會先問你的帳號，畫面會停在這一行等你輸入：

```text
Username for 'https://git.gridplusplus.ntuee.org':
```

輸入**學號**，按 ++enter++。接著它會問密碼：

```text
Password for 'https://你的學號@git.gridplusplus.ntuee.org':
```

輸入 **Gitea 密碼**，按 ++enter++。打密碼時畫面上**什麼都不會出現**（連 `*` 都沒有），這是正常的，照常打完按 ++enter++ 就好。

!!! note "只有第一次會問"

    因為第 3 步設定了 `credential.helper store`，輸入一次之後 Git 就會記住，之後 `git push`、`git pull` 都不會再問。

最後確認一下，執行：

```bash
ls ~/GridPlusPlus-Pacman
```

看到 `main.cpp`、`pacman.db`、`GridPlusPlus.h` 等檔案，就代表下載成功了。

## 常見問題

??? question "出現 `Authentication failed`"

    學號或密碼輸入錯誤。重新執行一次 `git clone` 那一行，仔細輸入即可。

??? question "出現 `Repository not found`"

    確認網址裡的學號是不是自己的，以及第 2 步的 Fork 有沒有完成。

??? question "出現 `destination path 'GridPlusPlus-Pacman' already exists`"

    代表之前已經下載過了，不需要再下載一次，直接進行下一步。

[下一步：執行第一個程式](04-first-run.md){ .md-button .md-button--primary }
