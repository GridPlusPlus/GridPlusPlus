# Grid++ 遊戲範本

這是一個空白的 Grid++ 專案，用來做屬於你自己的遊戲。還沒用過 Grid++ 的話，建議先跟著文件的「從零做出 Pac-Man」教學做一次。

- Grid++ 文件：<https://gridplusplus.github.io/GridPlusPlus/>
- 取得專案、上傳作品的步驟：見文件第 1 章「開始使用」。

## 執行

在這個資料夾裡執行（Windows 請在 WSL 的 Ubuntu 中執行）：

WSL / Linux：

```bash
g++ -std=c++17 main.cpp -o game -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```

macOS：

```bash
g++ -std=c++17 main.cpp -o game $(pkg-config --cflags --libs raylib)
./game
```

## 檔案

| 檔案 | 用途 |
|---|---|
| `main.cpp` | 你的遊戲程式，從這裡開始寫。 |
| 其他 `.h` 檔 | Grid++ 函式庫，不需要修改。 |
