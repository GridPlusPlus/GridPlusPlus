在專案資料夾的終端機編譯並執行（按 ++up++ 鍵可以叫出上次的指令）：

=== "Windows（WSL）/ Linux"

    ```bash
    g++ -std=c++17 main.cpp -o game -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 && ./game
    ```

=== "macOS"

    ```bash
    g++ -std=c++17 main.cpp -o game $(pkg-config --cflags --libs raylib) && ./game
    ```
