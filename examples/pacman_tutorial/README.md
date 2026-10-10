# 從零做出 Pac-Man

跟著 Grid++ 文件的「從零做出 Pac-Man」，一步一步做出會追人的鬼、會吃豆子的小精靈。

## 執行

在這個資料夾裡執行（WSL / Linux）：

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
| `main.cpp` | 你要寫的程式。每一步都把文件裡的程式碼貼到這裡。 |
| `steps/` | 每一步的參考答案。卡住時可以對照看看。 |
| `pacman.db` | 小精靈、鬼、牆壁和豆子的圖片。 |
| 其他 `.h` 檔 | Grid++ 函式庫，不需要修改。 |

想直接執行某一步的參考答案，例如第 3 步：

```bash
g++ -std=c++17 -I. steps/step3.cpp -o game -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./game
```
