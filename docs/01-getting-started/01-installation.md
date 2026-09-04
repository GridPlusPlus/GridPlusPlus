# 安裝 raylib

Grid++ 本身不需要安裝程序，但編譯器必須能找到 raylib 的標頭檔與函式庫。本節假設電腦已經有可用的 C++17 編譯器。

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

    確認標頭檔已經安裝：

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

## 分清楚安裝、編譯與開啟視窗

上面的檢查只能證明 raylib 的檔案已經放到預期位置，還不能證明 Grid++ 程式能夠編譯，更不能證明目前的桌面環境能顯示遊戲視窗。下一節會用同一份最小程式依序完成這兩項驗證：先確認編譯器能找到標頭檔並連結函式庫，再實際執行程式，確認圖形視窗能正常開啟。

如果程式已經成功編譯，執行時卻無法開啟視窗，應先檢查桌面顯示環境，而不是重新安裝 Grid++。這種情況在沒有圖形桌面的遠端 Linux，或尚未啟用圖形應用程式支援的 WSL 環境特別常見；先嘗試開啟其他圖形程式，便能區分問題究竟來自顯示環境，還是 Grid++ 程式本身。

安裝完成後，我們可以取得 Grid++，並用第一個程式完成上述兩層驗證。

[建立第一個 Grid++ 程式](02-hello-gridpp.md){ .md-button .md-button--primary }
