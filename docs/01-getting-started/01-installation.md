# 安裝 raylib

Grid++ 使用 raylib 來建立遊戲視窗、接收鍵盤與滑鼠輸入，以及繪製畫面，因此在開始寫 Grid++ 程式以前，需要先在電腦上準備好 raylib。

請選擇自己使用的開發環境，按照對應的步驟完成安裝即可。不同平台的安裝方式彼此獨立，不需要全部閱讀。

=== "Windows（WSL）"

````
如果你在 Windows 上使用 WSL，請在 Ubuntu 終端機中執行以下指令。後面的 Grid++ 教學也可以在同一個環境中完成。

先安裝編譯 raylib 所需的工具與系統套件：

```bash
sudo apt update
sudo apt install -y git build-essential \
    libasound2-dev libx11-dev libxrandr-dev libxi-dev \
    libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev \
    libwayland-dev libxkbcommon-dev
```

接著下載 raylib 6.0，編譯並安裝：

```bash
git clone --depth 1 --branch 6.0 https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
cd ../..
```

安裝完成後執行：

```bash
test -f /usr/local/include/raylib.h && echo "raylib installed"
```

如果最後看到：

```text
raylib installed
```

就可以繼續下一節。

!!! note "WSL 必須能顯示圖形視窗"

    Grid++ 是桌面圖形程式，因此 WSL 除了能編譯程式，也必須能開啟 Linux 圖形視窗。如果後面程式可以編譯，執行時卻沒有出現視窗，問題通常在 WSL 的圖形環境，而不是 Grid++ 本身。
````

=== "Ubuntu Linux"

````
先安裝編譯 raylib 所需的工具與系統套件：

```bash
sudo apt update
sudo apt install -y git build-essential \
    libasound2-dev libx11-dev libxrandr-dev libxi-dev \
    libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev \
    libwayland-dev libxkbcommon-dev
```

接著下載 raylib 6.0，編譯並安裝：

```bash
git clone --depth 1 --branch 6.0 https://github.com/raysan5/raylib.git
cd raylib/src
make PLATFORM=PLATFORM_DESKTOP
sudo make install
cd ../..
```

安裝完成後執行：

```bash
test -f /usr/local/include/raylib.h && echo "raylib installed"
```

如果最後看到：

```text
raylib installed
```

就可以繼續下一節。

其他 Linux 發行版也可以使用 raylib，但安裝系統套件的指令會有所不同；這份教學主要以 Ubuntu 為例。
````

=== "macOS"

````
如果尚未安裝 Homebrew，請先完成 Homebrew 的安裝，再執行：

```bash
brew install raylib pkg-config
```

接著確認 raylib 可以被找到：

```bash
pkg-config --modversion raylib
```

如果指令輸出 raylib 的版本號，就代表安裝完成。

也可以執行：

```bash
g++ --version
```

確認系統中有可用的 C++ 編譯器。macOS 上的 `g++` 通常實際使用 Apple Clang，這不影響後面的教學。
````

=== "Windows（MinGW-w64）"

````
如果你不使用 WSL，而是希望直接在 Windows 中編譯 `.exe`，可以使用 MinGW-w64。

先確認終端機中的編譯器是 MinGW-w64：

```bash
g++ --version
```

接著從 [raylib 6.0 Releases](https://github.com/raysan5/raylib/releases/tag/6.0) 下載 Windows 的 MinGW-w64 版本，解壓縮後保留其中的 `include` 與 `lib` 資料夾。

下一節建立 Grid++ 專案時，會把這兩個資料夾放進專案中，因此目前只需要完成下載與解壓縮。

!!! warning "不要使用舊版 MinGW"

    raylib 需要 MinGW-w64。如果你的環境使用的是早期的 mingw.org MinGW，請改用現代的 MinGW-w64 環境。
````

## 下一步

完成這一頁之後，我們還沒有真正執行 Grid++。下一節會建立一個只有 8×8 網格的最小程式，並用它確認三件事情：C++ 編譯器可以使用、raylib 可以正確連結，以及遊戲視窗可以開啟。

如果其中任何一步失敗，下一節會依照錯誤發生的位置判斷問題，而不需要在這裡先理解編譯與連結的細節。

[建立第一個 Grid++ 程式](02-hello-gridpp.md){ .md-button .md-button--primary }
