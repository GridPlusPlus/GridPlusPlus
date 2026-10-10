# 安裝 raylib

Grid++ 靠 **raylib** 開視窗、畫圖和讀鍵盤。這一步每台電腦只要做一次。

打開終端機，依照你的系統，把指令**一段一段**貼上執行。每段都要等它跑完、重新出現 `$` 或 `%`，再貼下一段。

=== "Windows（WSL）"

    在 Ubuntu 視窗中執行。

    **第 1 段：安裝編譯工具**（會要你輸入 Ubuntu 密碼，輸入時畫面不會顯示）

    ```bash
    sudo apt update
    sudo apt install -y git build-essential \
        libasound2-dev libx11-dev libxrandr-dev libxi-dev \
        libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev \
        libwayland-dev libxkbcommon-dev
    ```

    **第 2 段：下載並安裝 raylib**（大約需要 1–3 分鐘）

    ```bash
    cd ~
    git clone --depth 1 --branch 6.0 https://github.com/raysan5/raylib.git
    cd raylib/src
    make PLATFORM=PLATFORM_DESKTOP
    sudo make install
    cd ~
    ```

    **第 3 段：檢查是否成功**

    ```bash
    test -f /usr/local/include/raylib.h && echo "raylib installed"
    ```

=== "macOS"

    **第 1 段：安裝 raylib**

    ```bash
    brew install raylib pkg-config
    ```

    **第 2 段：檢查是否成功**

    ```bash
    pkg-config --modversion raylib && echo "raylib installed"
    ```

=== "Linux（Ubuntu）"

    指令和「Windows（WSL）」分頁完全相同，請切換到那個分頁照著做。其他 Linux 發行版的套件名稱不同，課程以 Ubuntu 為準。

如果最後一行看到下面這句話，就完成了 🎉

```text
raylib installed
```

??? info "這些指令在做什麼？（看不懂也沒關係）"

    - `sudo apt install ...`：從 Ubuntu 的軟體庫安裝 C++ 編譯器 `g++`、版本控制工具 `git`，以及 raylib 需要的圖形、音效套件。`sudo` 代表「用管理員權限執行」，所以要輸入密碼。
    - `git clone ...`：從網路下載 raylib 6.0 版的原始碼。
    - `make`：把 raylib 的原始碼編譯成函式庫；`sudo make install` 把它放到系統資料夾，之後所有程式都找得到。
    - macOS 的 `brew install` 會直接下載編譯好的 raylib，所以比較快。

## 常見問題

??? question "第 2 段出現 `fatal: destination path 'raylib' already exists`"

    代表之前已經下載過了。先刪掉舊的再重新執行第 2 段：

    ```bash
    rm -rf ~/raylib
    ```

??? question "沒有出現 `raylib installed`"

    往上捲，找找看有沒有 `error` 字樣的訊息。最常見的原因是第 1 段沒有跑完（例如密碼輸入錯誤）。重新執行第 1 段，再依序執行後面兩段。

[下一步：取得你的專案](03-get-project.md){ .md-button .md-button--primary }
