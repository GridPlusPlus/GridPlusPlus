# 安裝 raylib

Grid++ 本身不需要安裝程序，但編譯器必須能找到 raylib 的 header 與 library。本節假設電腦已經有可用的 C++17 編譯器。

Windows 使用者建議在 WSL 中開發。WSL 和 Linux 使用相同的工具與編譯指令，也比較接近課程與 CI 的環境。只有需要產生原生 Windows `.exe` 時，才需要使用 MinGW-w64。

以下分頁是同一項工作的不同平台做法，只需閱讀你使用的平台。這一節只準備 raylib；下一節會以實際編譯 Grid++ 程式作為完整驗證。

=== "WSL"

    以下指令都在 WSL 的 Ubuntu 終端機中執行。專案建議放在 Linux 檔案系統，例如 `~/codes`。

    安裝 raylib 需要的建置工具和圖形函式庫：

    ```bash
    sudo apt update
    sudo apt install -y git build-essential \
        libasound2-dev libx11-dev libxrandr-dev libxi-dev \
        libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev \
        libwayland-dev libxkbcommon-dev
    ```

    從官方 6.0 tag 建置並安裝 raylib：

    ```bash
    git clone --depth 1 --branch 6.0 https://github.com/raysan5/raylib.git
    cd raylib/src
    make PLATFORM=PLATFORM_DESKTOP
    sudo make install
    cd ../..
    ```

    確認 header 已經安裝：

    ```bash
    test -f /usr/local/include/raylib.h && echo "raylib installed"
    ```

    最後一行應顯示 `raylib installed`。

=== "Ubuntu Linux"

    Ubuntu 使用和 WSL 相同的建置方式：

    ```bash
    sudo apt update
    sudo apt install -y git build-essential \
        libasound2-dev libx11-dev libxrandr-dev libxi-dev \
        libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev \
        libwayland-dev libxkbcommon-dev

    git clone --depth 1 --branch 6.0 https://github.com/raysan5/raylib.git
    cd raylib/src
    make PLATFORM=PLATFORM_DESKTOP
    sudo make install
    cd ../..
    ```

    其他 Linux distribution 需要改用對應的套件管理器安裝圖形依賴。raylib 的建置步驟維持相同。

=== "macOS"

    使用 Homebrew 安裝 raylib 與 `pkg-config`：

    ```bash
    brew install raylib pkg-config
    ```

    確認安裝結果：

    ```bash
    g++ --version
    pkg-config --modversion raylib
    ```

    macOS 的 `g++` 通常是 Apple Clang。文件中的指令仍使用 `g++`，`pkg-config` 會補上 Homebrew raylib 需要的編譯與連結參數。

=== "Windows / MinGW-w64"

    raylib 官方推薦使用 W64Devkit。已設定完成的其他 MinGW-w64 環境也可以使用。

    1. 從 [raylib 6.0 releases](https://github.com/raysan5/raylib/releases/tag/6.0) 下載 `raylib-6.0_win32_mingw-w64.zip`。
    2. 解壓縮並保留 `include` 與 `lib` 資料夾。下一節會把它們放在遊戲專案的 `raylib/` 資料夾中。
    3. 執行 `g++ --version`，確認終端機使用 MinGW-w64。

    !!! warning "MinGW-w64"

        raylib 需要 MinGW-w64。舊的 mingw.org MinGW 無法使用。

安裝完成後，我們可以取得 Grid++ 並編譯第一個程式。

[建立第一個 Grid++ 程式](02-hello-gridpp.md){ .md-button .md-button--primary }
