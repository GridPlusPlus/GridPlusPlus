# 安裝與建立專案

Grid++ 使用 C++17，唯一的外部函式庫是 raylib。以下程序假設系統已有可用的 C++ 編譯器，並只安裝 raylib。

每個平台分頁包含完整的安裝與驗證程序。Windows 開發環境建議使用 WSL；需要產生原生 `.exe` 時使用 MinGW-w64。

=== "WSL"

    以下指令均在 WSL 的 Ubuntu 終端機執行。將專案放在 Linux 檔案系統（例如 `~/codes`），避免放在 `/mnt/c`。

    **1. 安裝 raylib 需要的圖形函式庫**

    ```bash
    sudo apt update
    sudo apt install -y git \
        libasound2-dev libx11-dev libxrandr-dev libxi-dev \
        libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev \
        libwayland-dev libxkbcommon-dev
    ```

    **2. 建置並安裝 raylib 6.0**

    ```bash
    git clone --depth 1 --branch 6.0 https://github.com/raysan5/raylib.git
    cd raylib/src
    make PLATFORM=PLATFORM_DESKTOP
    sudo make install
    cd ../..
    ```

    **3. 確認安裝**

    ```bash
    g++ --version
    test -f /usr/local/include/raylib.h && echo "raylib installed"
    ```

    最後一行必須顯示 `raylib installed`。

=== "Windows / MinGW-w64"

    raylib 官方推薦 W64Devkit。已配置完成的其他 MinGW-w64 環境也可使用。

    1. 從 [raylib 6.0 releases](https://github.com/raysan5/raylib/releases/tag/6.0) 下載 `raylib-6.0_win32_mingw-w64.zip`。
    2. 解壓縮後保留 `include` 與 `lib` 資料夾。下一頁會把它們放進 Grid++ 專案。
    3. 執行 `g++ --version`，確認終端機使用的是 MinGW-w64。

    !!! warning "不要使用舊版 MinGW"

        raylib 需要 MinGW-w64。舊的 mingw.org MinGW 無法使用。

=== "macOS"

    raylib 透過 Homebrew 安裝。若系統尚未安裝 Homebrew，請先依照 [Homebrew 官方首頁](https://brew.sh/) 完成安裝。

    ```bash
    brew install raylib pkg-config
    ```

    **確認安裝**

    ```bash
    g++ --version
    pkg-config --modversion raylib
    ```

    兩個指令都必須輸出版本資訊。

=== "Ubuntu Linux"

    Ubuntu 的官方套件庫目前沒有 raylib 6.0 開發套件，因此依照 raylib 官方 Wiki 從原始碼建置。

    **1. 安裝 raylib 需要的圖形函式庫**

    ```bash
    sudo apt update
    sudo apt install -y git \
        libasound2-dev libx11-dev libxrandr-dev libxi-dev \
        libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev \
        libwayland-dev libxkbcommon-dev
    ```

    **2. 建置並安裝 raylib 6.0**

    ```bash
    git clone --depth 1 --branch 6.0 https://github.com/raysan5/raylib.git
    cd raylib/src
    make PLATFORM=PLATFORM_DESKTOP
    sudo make install
    cd ../..
    ```

    **3. 確認安裝**

    ```bash
    g++ --version
    test -f /usr/local/include/raylib.h && echo "raylib installed"
    ```

    其他 Linux distribution 請參考 raylib 官方 Wiki 安裝對應的圖形依賴。

## 取得 Grid++

Clone Grid++ repository，並進入專案目錄。Grid++ 是 header-only library，不需要另外建置或安裝 library 本身；編譯遊戲時讓編譯器找到這些 header 即可。

```bash
git clone https://github.com/GridPlusPlus/GridPlusPlus.git
cd GridPlusPlus
```

若課程提供 GitHub Classroom 或 template repository，應使用課程提供的 URL。無法使用 Git 時，也可以從 GitHub 的 **Code → Download ZIP** 下載程式碼，但之後仍建議用 Git 保存自己的修改。

使用原生 Windows MinGW-w64 時，把 raylib 的 `include` 與 `lib` 目錄放進專案。WSL、Linux 和 macOS 已把 raylib 安裝到系統路徑，不需要這兩個目錄。

```text
GridPlusPlus/
├── raylib/
│   ├── include/
│   └── lib/
├── GridPlusPlus.h
├── GridEngine.h
├── GridObject.h
├── Overlay.h
└── template.cpp
```

## 編譯起始程式

Repository 內的 `template.cpp` 建立 10×10 網格，並放入一個可用方向鍵移動的物件。使用下列對應平台的指令編譯。

=== "WSL / Linux"

    ```bash
    g++ -std=c++17 template.cpp -o game \
        -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    ```

=== "Windows / MinGW-w64"

    ```bash
    g++ -std=c++17 template.cpp -o game.exe \
        -Iraylib/include -Lraylib/lib -lraylib -lgdi32 -lwinmm
    ```

=== "macOS"

    ```bash
    g++ -std=c++17 template.cpp -o game $(pkg-config --cflags --libs raylib)
    ```

`-std=c++17` 指定 Grid++ 使用的 C++ 語言版本。`template.cpp` 是輸入來源，`-o` 後方是輸出檔名，其餘參數用來尋找並連結 raylib。macOS 的 `pkg-config` 會輸出 Homebrew raylib 所需的 include 與 library 參數。

## 執行與驗證

WSL、Linux 和 macOS 執行 `./game`；原生 Windows MinGW-w64 執行 `./game.exe`。程式應開啟 10×10 的視窗，中央顯示一個紅色方塊。方向鍵每次將方塊移動一格，邊界檢查會阻止它離開視窗。

紅色方塊不是載入錯誤。起始程式沒有素材包，Grid++ 會用紅色方塊替代不存在的素材，讓網格、輸入和移動可以先完成驗證。

若編譯器回報找不到 `raylib.h`，表示 raylib header 不在 include path；若連結階段出現 `undefined reference`，表示 raylib library 或平台 library 沒有正確連結。先重新執行本頁的安裝驗證，再比對編譯指令，不需要修改 Grid++ header。

**驗證結果**

- [ ] 編譯指令沒有錯誤。
- [ ] 遊戲視窗可以開啟與關閉。
- [ ] 四個方向鍵都能移動方塊。
- [ ] 方塊不會移出網格。

[GridEngine](03-grid-engine.md){ .md-button .md-button--primary }
