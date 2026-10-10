# 準備電腦

寫程式時，我們會用到兩個工具：

- **終端機**：一個可以輸入指令的視窗。你會在這裡把程式「編譯」成遊戲，再執行它。
- **VS Code**：寫程式用的編輯器，就像寫程式專用的 Word。

請選擇你的電腦系統：

=== "Windows"

    Windows 上我們使用 **WSL**，它會在 Windows 裡面裝一個小小的 Linux（Ubuntu）。課程的所有指令都在這個 Ubuntu 裡執行。

    **1. 安裝 WSL**

    1. 按下鍵盤的 ++windows++ 鍵，輸入 `PowerShell`。
    2. 在「Windows PowerShell」上按右鍵，選「**以系統管理員身分執行**」，出現詢問時按「是」。
    3. 把下面這行指令複製，在藍色視窗中按右鍵貼上，再按 ++enter++：

        ```powershell
        wsl --install
        ```

    4. 等它跑完，**重新開機**。

    **2. 設定 Ubuntu**

    重新開機後，會自動跳出一個 Ubuntu 視窗（沒有的話，按 ++windows++ 鍵搜尋 `Ubuntu` 打開它）。它會請你設定：

    - **使用者名稱**：用英文小寫，例如你的名字縮寫。
    - **密碼**：輸入時畫面上**什麼都不會出現**，這是正常的，打完按 ++enter++ 就好。

    !!! warning "請記住這組密碼"

        之後安裝東西時，Ubuntu 會再跟你要這組密碼。

    看到類似下面這行，就代表 Ubuntu 準備好了。**這個視窗就是你的終端機。**

    ```text
    yourname@DESKTOP-XXXX:~$
    ```

    **3. 安裝 VS Code**

    1. 到 [VS Code 官網](https://code.visualstudio.com/) 下載並安裝 Windows 版。
    2. 打開 VS Code，按左側的「延伸模組」圖示（四個小方塊），搜尋 **WSL**，安裝 Microsoft 出的那一個。

=== "macOS"

    **1. 打開終端機**

    按 ++cmd+space++，輸入 `終端機`（或 `Terminal`），按 ++enter++。**這個視窗就是你的終端機。**

    **2. 安裝開發工具**

    把下面的指令貼到終端機（++cmd+v++），按 ++enter++，在跳出的視窗中按「安裝」：

    ```bash
    xcode-select --install
    ```

    如果出現 `already installed`，代表已經裝好了，直接進行下一步。

    **3. 安裝 Homebrew**

    Homebrew 是用來安裝程式工具的小幫手。貼上這行指令：

    ```bash
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    ```

    過程中它會要你按 ++enter++，並輸入**電腦的開機密碼**（輸入時畫面不會顯示，這是正常的）。

    裝完後，畫面最後會出現 `Next steps`。如果你的 Mac 是 Apple 晶片（M1、M2 等），再貼上這兩行：

    ```bash
    echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
    eval "$(/opt/homebrew/bin/brew shellenv)"
    ```

    **4. 安裝 VS Code**

    1. 到 [VS Code 官網](https://code.visualstudio.com/) 下載 Mac 版，把它拖進「應用程式」資料夾。
    2. 打開 VS Code，按 ++cmd+shift+p++，輸入 `shell command`，選「**Shell Command: Install 'code' command in PATH**」。

## 小技巧：在終端機裡貼上指令

這份文件裡灰色框框中的指令，都可以按右上角的複製按鈕，再貼到終端機執行。

| | 貼上 | 指令跑完的樣子 |
|---|---|---|
| Windows（Ubuntu 視窗） | 按滑鼠右鍵，或 ++ctrl+shift+v++ | 最後一行又出現 `$` |
| macOS | ++cmd+v++ | 最後一行又出現 `%` |

指令還在跑的時候，請耐心等它出現 `$` 或 `%`，再貼下一個指令。

## 常見問題

??? question "Windows：輸入 `wsl --install` 後，只出現一大堆說明文字"

    代表電腦已經有 WSL，但還沒裝 Ubuntu。改成執行：

    ```powershell
    wsl --install -d Ubuntu
    ```

??? question "Windows：出現 0x80370102 或提到「虛擬化」的錯誤"

    電腦的虛擬化功能沒有打開，需要到 BIOS 設定裡開啟。每台電腦的做法不同，請找助教幫忙。

??? question "Windows：忘記 Ubuntu 的密碼"

    在 PowerShell 執行 `wsl -u root`，再執行 `passwd 你的使用者名稱` 就能重設。

[下一步：安裝 raylib](02-install-raylib.md){ .md-button .md-button--primary }
