# Pac-Man 範例

用 Grid++ 寫成的最小 Pac-Man 小遊戲，示範引擎的迷宮模組（`GridMaze`）、
遊戲物件（`GridObject`）與畫面覆蓋層（`Overlay`）如何組在一起。
這一頁逐步拆解專案附的 `examples/pacman/main.cpp`。

> 玩法：**方向鍵**移動——角色會持續朝目前方向走，按方向鍵且該方向不是牆時才轉向；
> **吃光所有豆子**獲勝，**被任何鬼魂抓到**失敗。

## 資料夾內容

| 檔案 | 說明 |
|---|---|
| `main.cpp` | 範例原始碼（完整可編譯版本）。 |
| `map.txt` | 關卡地圖（純整數，見下方格式）。 |
| `pacman.db` | 素材包（pacman / ghost / pellet / wall … 等圖檔的打包）。 |

`game`／`game.exe` 是編譯產生的檔案，不進版控；`pacman.db` 素材包則隨範例提供，執行前需放在本資料夾內。

## 編譯與執行

raylib 的安裝方式見[安裝與設定](../../docs/02-install.md)。Windows 建議使用 WSL，並依照 Linux 指令操作；
需要原生 `.exe` 時才使用 MinGW-w64。`-I../..` 讓編譯器找到專案根目錄的 headers。

=== "WSL / Linux"

    ```bash
    g++ -std=c++17 main.cpp -I../.. -o game \
        -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    ./game
    ```

=== "Windows / MinGW-w64"

    ```bash
    g++ -std=c++17 main.cpp -I../.. -o game.exe \
        -I../../raylib/include -L../../raylib/lib -lraylib -lgdi32 -lwinmm
    ./game.exe
    ```

=== "macOS"

    ```bash
    g++ -std=c++17 main.cpp -I../.. -o game $(pkg-config --cflags --libs raylib)
    ./game
    ```

!!! note "執行前確認"
    程式以相對路徑讀取 `map.txt` 與 `pacman.db`，所以要在 `examples/pacman/` 資料夾內執行。

## 地圖檔 map.txt

地圖用純整數描述，方便用 `ifstream >>` 逐一讀取（不必處理換行）：

```
21 19
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
1 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 1
...
```

- **第一行**：兩個整數 **列數 欄數**（rows cols）。
- **之後**：`列數 × 欄數` 個整數，每格一個值：

| 值 | 意義 |
|---|---|
| `1` | 牆壁 |
| `0` | 通道（放一顆豆子） |
| `2` | 小精靈（玩家）起點 |
| `3` | 鬼魂起點 |

!!! note "為什麼要先寫維度"
    用 `>>` 一個個讀整數會自動略過所有空白與換行，所以它「看不到換行＝換列」。
    在開頭先給列數、欄數，程式才知道地圖多大。改地圖大小時記得同步更新這兩個數字。

內附地圖是 **21×19** 的 Pac-Man 風格迷宮：外圈一圈迴路、對稱的內部牆塊、中央有一個鬼屋
（鬼從這裡出發）。走廊寬度恰好 1，在 32 像素的格子下視窗約 608×672，不會超出螢幕。

## 三個角色類別

每個角色都繼承 `GridObject`，只覆寫需要的生命週期函式。四個方向以 0~3 表示——
右、上、左、下，對應 `kDirectionX/kDirectionY` 偏移量與 `set_direction`。

### 豆子 Pellet

用引擎的「同格碰撞」機制：玩家踩上來時（靠 `tag() == "pacman"` 辨識），把自己標記成
被吃掉、剩餘數量減一；數量歸零即獲勝。被吃掉後覆寫 `Render` 不再畫。

```cpp
class Pellet : public GridObject {
public:
    Pellet(int x, int y) : GridObject("pellet", x, y) { set_tag("pellet"); }
    void OnCollide(GridObject* other) override {
        if (!eaten_ && other->tag() == "pacman") {
            eaten_ = true;
            if (--g_pellets_left <= 0) g_state = 2;    // 吃完 → 獲勝
        }
    }
    void Render(GridEngine* engine) override {
        if (!eaten_) GridObject::Render(engine);        // 被吃掉後就不畫
    }
private:
    bool eaten_ = false;
};
```

### 鬼魂 Ghost

每隔幾幀走一格（比玩家稍慢）。共四隻，靠 `set_tint` 把**同一張白色素材**染成
紅 / 粉 / 青 / 橘。選方向時先收集「不是牆、且不回頭」的方向（死路才允許回頭）：

- **貪心鬼（3 隻）**：挑離玩家**曼哈頓距離最小**的方向。
- **隨機鬼（1 隻）**：在合法方向裡隨機挑一個。
- 「不回頭」用 `d == (direction_ ^ 2)` 判斷——方向 0/1/2/3 中，互為相反者 XOR 2 相等。

```cpp
Ghost(int x, int y, Color color, bool is_random)
    : GridObject("ghost", x, y), random_(is_random) {
    set_tag("ghost");
    set_tint(color);              // ← 調色：白色素材染成這隻鬼的顏色
}
```

貪心挑方向的核心——走某方向後離玩家多遠（曼哈頓距離），挑最小的：

```cpp
int dist = ManhattanDistance(x() + kDirectionX[d], y() + kDirectionY[d],
                     g_player->x(), g_player->y());
if (dist < best) { best = dist; pick = d; }
```

### 小精靈 Pacman

移動規則做得像真的 Pac-Man：**持續朝目前方向前進；按下方向鍵、且那個方向不是牆時才轉向**。
作法是每幀把玩家「想要」的方向記在 `wanted_direction_`，到了移動時機才決定能不能轉：

```cpp
void OnUpdate() override {
    if (g_state != 1 || g_paused) return;
    if (IsKeyDown(KEY_RIGHT)) wanted_direction_ = 0;   // 每幀記下想要的方向（緩衝）
    if (IsKeyDown(KEY_UP))    wanted_direction_ = 1;
    if (IsKeyDown(KEY_LEFT))  wanted_direction_ = 2;
    if (IsKeyDown(KEY_DOWN))  wanted_direction_ = 3;

    if (++timer_ < 8) return;              // 每 8 幀走一格（移動速度）
    timer_ = 0;

    if (wanted_direction_ >= 0 && !g_maze->IsWall(x() + kDirectionX[wanted_direction_], y() + kDirectionY[wanted_direction_]))
        direction_ = wanted_direction_;                       // 想要的方向合法 → 轉過去
    if (direction_ >= 0 && !g_maze->IsWall(x() + kDirectionX[direction_], y() + kDirectionY[direction_])) {
        Move(kDirectionX[direction_], kDirectionY[direction_]);
        set_direction(direction_);                // ← 旋轉素材，讓嘴巴朝向移動方向
    }
}
void OnCollide(GridObject* other) override {
    if (other->tag() == "ghost") g_state = 3;   // 被抓到 → 失敗
}
```

把「想要的方向」緩衝起來，是 Pac-Man 手感的關鍵：你可以提早按方向，
角色一到能轉的路口就會轉過去。

## 遊戲流程與畫面

整個遊戲用一個全域 `g_state` 表示目前在哪個階段：

| `g_state` | 階段 | 畫面 |
|---|---|---|
| `0` | 開始畫面 | 壓暗 + 標題「PAC-MAN」+ **Start** 按鈕 |
| `1` | 遊戲中 | 左上角分數；右上角 **Pause** 按鈕（可暫停） |
| `2` | 獲勝 | 壓暗 +「YOU WIN!」+ **Restart** 按鈕 |
| `3` | 失敗 | 壓暗 +「GAME OVER」+ **Restart** 按鈕 |

角色只在 `g_state == 1` 時移動，所以開始畫面與結束畫面都會自動「凍結」。
各按鈕也只在對的階段顯示與作用（見 [Overlay](../../docs/05-overlay.md)）。

**重新開始**靠兩件事：引擎的 `ClearObjects()`（刪掉上一局的所有物件）＋
把佈置流程抽成 `BuildLevel(game, level)`，開始與重來都使用啟動時已驗證的地圖。

```cpp
void BuildLevel(GridEngine& game, const LevelMap& level) {
    game.ClearObjects();          // 清掉上一局
    g_pellets_left = 0;
    // ...用已驗證的 level 建立迷宮、生成豆子/鬼魂/玩家...
}
```

## 組裝流程（main）

`main()` 只做高層次的事：

1. 完整讀取並驗證 `map.txt`，再開引擎、載素材，用 `set_background_color(BLACK)` 設成經典黑底
   （網格線預設已關閉）。
2. `BuildLevel(game, level)` 佈置第一局，把 `g_state` 設成 `0`（停在開始畫面）。
3. `AddOverlay` 加上分數、Start、Restart、Pause。
4. `game.Run()`。

其中 `BuildLevel` 內部：建立 `GridMaze(cols, rows)`、用 `SetWallTiles` 給 6 種基本牆形狀
（其餘方向靠旋轉自動拼接出全部 16 種連通），再逐格使用已驗證的整數——`1` 用 `SetWall` 設牆、
`0` 生成豆子、`2` 記住玩家、`3` 生成鬼魂。

!!! note "為什麼不用 vector？"
    地圖用固定 64 × 64 陣列暫存，先驗證完整內容再建立物件，不需要 `vector`。
    迷宮仍使用 `GridMaze(cols,rows)` + `SetWall` 逐格設定。

!!! tip "重點觀念"
    - **碰撞分兩種**：豆子用引擎的「同格碰撞」（`OnCollide`）；牆壁用迷宮的 `IsWall()` 主動查詢。
    - **用 tag 分辨對象**：`OnCollide` 拿到的是 `GridObject*`，靠 `tag()` 判斷是豆子還是鬼魂。
    - **生成順序＝繪製順序**：迷宮先 `Spawn`（最底層），玩家最後 `Spawn`（最上層）。
    - **重複利用素材**：小精靈靠 `set_direction` 旋轉一張圖朝向四個方向；四隻鬼靠 `set_tint`
      把同一張白色素材染成不同顏色；牆壁也靠旋轉，用 6 種基本形狀拼出全部 16 種連通。
    - **覆蓋層與遊戲分開**：分數與訊息放在 `ScoreOverlay : Overlay`、按鈕是 `Button` 的子類，
      各自只在「對的階段」顯示與作用，透過 `AddOverlay` 加入、畫在網格之上。詳見 [Overlay](../../docs/05-overlay.md)。
