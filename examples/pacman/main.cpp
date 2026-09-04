// =============================================================================
//  examples/pacman —— Grid++ 範例：Pac-Man 小遊戲
//
//  編譯指令、map.txt 格式、遊戲流程與邏輯總覽見同資料夾的 README.md；
//  對應教學見 docs/07-pacman/。
// =============================================================================
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include "GridMaze.h"  // 用 -I../.. 指到專案根；會自動帶進核心 GridPlusPlus.h
#include "LevelMap.h"

using gridpp::Button;
using gridpp::GridEngine;
using gridpp::GridMaze;
using gridpp::GridObject;
using gridpp::Overlay;
using pacman_example::LevelMap;
using pacman_example::LoadLevelMap;

namespace {

// 地圖啟動時完整驗證一次，再用固定陣列內容建立與重建關卡。

// 四個方向，對應 set_direction 的 0~3：右、上、左、下
constexpr int kDirectionX[4] = {1, 0, -1, 0};
constexpr int kDirectionY[4] = {0, -1, 0, 1};

enum class GameState {
    kStart,
    kPlaying,
    kWon,
    kLost,
};

GameState g_state = GameState::kStart;
int g_pellets_left = 0;          // 還剩幾顆豆子
bool g_paused = false;           // 是否暫停（由暫停按鈕切換）
GridMaze* g_maze = nullptr;      // 讓角色查詢牆壁
GridObject* g_player = nullptr;  // 讓鬼魂知道玩家在哪
GridEngine* g_game = nullptr;    // 讓「重新開始」按鈕能重建世界
LevelMap g_level;

// 曼哈頓距離。
int ManhattanDistance(int from_x, int from_y, int to_x, int to_y) {
    return std::abs(from_x - to_x) + std::abs(from_y - to_y);
}

// 豆子：被小精靈吃到就消失
class Pellet : public GridObject {
public:
    Pellet(int x, int y) : GridObject("pellet", x, y) { set_tag("pellet"); }

    void OnCollide(GridObject* other) override {
        if (!eaten_ && other->tag() == "pacman") {
            eaten_ = true;
            if (--g_pellets_left <= 0) g_state = GameState::kWon;
        }
    }
    void Render(GridEngine* engine) override {
        if (!eaten_) GridObject::Render(engine);
    }

private:
    bool eaten_ = false;
};

// 鬼魂：貪心追逐（挑離玩家最近、且不回頭的方向）或隨機。各自有不同顏色(tint)。
class Ghost : public GridObject {
public:
    Ghost(int x, int y, Color color, bool is_random) : GridObject("ghost", x, y), random_(is_random) {
        set_tag("ghost");
        set_tint(color);  // 同一張白色素材染成不同顏色
    }

    void OnUpdate() override {
        if (g_state != GameState::kPlaying || g_paused || g_player == nullptr) return;
        if (++timer_ < 12) return;  // 移動速度（比玩家稍慢）
        timer_ = 0;

        int options[4];
        int option_count = 0;  // 非牆、且不回頭
        int any[4];
        int any_count = 0;  // 非牆（死路時才用，允許回頭）
        for (int direction = 0; direction < 4; ++direction) {
            if (g_maze->IsWall(x() + kDirectionX[direction], y() + kDirectionY[direction])) continue;
            any[any_count++] = direction;
            if (direction == (direction_ ^ 2)) continue;  // 與目前方向相反 → 回頭，先跳過
            options[option_count++] = direction;
        }
        int* pool = option_count > 0 ? options : any;
        const int pool_count = option_count > 0 ? option_count : any_count;
        if (pool_count == 0) return;

        int pick = pool[0];
        if (random_) {
            pick = pool[GetRandomValue(0, pool_count - 1)];  // 隨機鬼
        } else {                                             // 貪心鬼：挑離玩家曼哈頓距離最小的
            int best = 1 << 30;
            for (int i = 0; i < pool_count; ++i) {
                const int direction = pool[i];
                const int distance = ManhattanDistance(x() + kDirectionX[direction], y() + kDirectionY[direction],
                                                       g_player->x(), g_player->y());
                if (distance < best) {
                    best = distance;
                    pick = direction;
                }
            }
        }
        direction_ = pick;
        Move(kDirectionX[direction_], kDirectionY[direction_]);
    }

private:
    bool random_;
    int timer_ = 0;
    int direction_ = 0;
};

// 小精靈（玩家）：持續朝目前方向走；按方向鍵且該方向不是牆才轉向。
class Pacman : public GridObject {
public:
    Pacman(int x, int y) : GridObject("pacman", x, y) { set_tag("pacman"); }

    void OnUpdate() override {
        if (g_state != GameState::kPlaying || g_paused) return;
        if (IsKeyDown(KEY_RIGHT)) wanted_direction_ = 0;  // 每幀記下想要的方向（緩衝）
        if (IsKeyDown(KEY_UP)) wanted_direction_ = 1;
        if (IsKeyDown(KEY_LEFT)) wanted_direction_ = 2;
        if (IsKeyDown(KEY_DOWN)) wanted_direction_ = 3;

        if (++timer_ < 8) return;  // 移動速度
        timer_ = 0;

        if (wanted_direction_ >= 0 &&
            !g_maze->IsWall(x() + kDirectionX[wanted_direction_], y() + kDirectionY[wanted_direction_])) {
            direction_ = wanted_direction_;  // 想要的方向合法 → 轉過去
        }
        if (direction_ >= 0 && !g_maze->IsWall(x() + kDirectionX[direction_], y() + kDirectionY[direction_])) {
            Move(kDirectionX[direction_], kDirectionY[direction_]);
            set_direction(direction_);  // 旋轉素材朝向移動方向
        }
    }
    void OnCollide(GridObject* other) override {
        if (other->tag() == "ghost") g_state = GameState::kLost;
    }

private:
    int timer_ = 0;
    int direction_ = -1;  // -1 = 還沒開始移動
    int wanted_direction_ = -1;
};

// 依已驗證的地圖佈置一局：清掉上一局 → 重建迷宮、豆子、角色。
void BuildLevel(GridEngine& game, const LevelMap& level) {
    game.ClearObjects();
    g_pellets_left = 0;
    g_paused = false;
    g_maze = nullptr;
    g_player = nullptr;

    GridMaze* maze = new GridMaze(level.cols, level.rows);
    // 自動拼接：給 6 種基本牆形狀，其餘方向引擎會旋轉素材湊出來。
    maze->SetWallTiles("wall_iso", "wall_end", "wall_straight", "wall_corner", "wall_tee", "wall_cross");
    game.Spawn(maze);  // 先放迷宮（畫最底層）
    g_maze = maze;

    const Color colors[4] = {RED, PINK, SKYBLUE, ORANGE};  // 四隻鬼的顏色
    int ghost_count = 0;
    Pacman* player = nullptr;
    for (int y = 0; y < level.rows; ++y) {
        for (int x = 0; x < level.cols; ++x) {
            const int tile = level.tiles[y][x];
            if (tile == 1) {
                maze->SetWall(x, y, true);
            } else if (tile == 0) {
                game.Spawn(new Pellet(x, y));
                ++g_pellets_left;
            } else if (tile == 2) {
                player = new Pacman(x, y);
            } else if (tile == 3) {
                game.Spawn(new Ghost(x, y, colors[ghost_count % 4], ghost_count == 3));
                ++ghost_count;
            }
        }
    }
    if (player != nullptr) {
        g_player = player;
        game.Spawn(player);  // 玩家最後生成（畫最上層）
    }
}

// 分數與各畫面的訊息（畫面覆蓋層，跳脫網格、用像素座標）
class ScoreOverlay : public Overlay {
public:
    void Draw() override {
        if (g_state == GameState::kPlaying) {
            DrawText(TextFormat("Pellets: %d", g_pellets_left), 8, 8, 20, YELLOW);
            if (g_paused) DrawBigText("PAUSED", ORANGE);
            return;
        }
        // 非遊戲中：壓暗背景，畫大字（按鈕由各自的類別畫在這之上）
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.6f));
        if (g_state == GameState::kStart)
            DrawBigText("PAC-MAN", YELLOW);
        else if (g_state == GameState::kWon)
            DrawBigText("YOU WIN!", GREEN);
        else if (g_state == GameState::kLost)
            DrawBigText("GAME OVER", RED);
    }

private:
    static void DrawBigText(const char* message, Color color) {
        constexpr int kFontSize = 40;
        const int text_width = MeasureText(message, kFontSize);
        DrawText(message, GetScreenWidth() / 2 - text_width / 2, GetScreenHeight() / 2 - 70, kFontSize, color);
    }
};

// 各按鈕只在「對的階段」顯示與作用：覆寫 OnClick（行為）與 OnUpdate/Draw（階段判斷）。
class StartButton : public Button {
public:
    StartButton(int x, int y, int width, int height) : Button("Start", x, y, width, height) {}
    void OnClick() override { g_state = GameState::kPlaying; }
    void OnUpdate() override {
        if (g_state == GameState::kStart) Button::OnUpdate();
    }
    void Draw() override {
        if (g_state == GameState::kStart) Button::Draw();
    }
};

class RestartButton : public Button {
public:
    RestartButton(int x, int y, int width, int height) : Button("Restart", x, y, width, height) {}
    // 重來一局。
    void OnClick() override {
        BuildLevel(*g_game, g_level);
        g_state = GameState::kPlaying;
    }
    void OnUpdate() override {
        if (g_state == GameState::kWon || g_state == GameState::kLost) Button::OnUpdate();
    }
    void Draw() override {
        if (g_state == GameState::kWon || g_state == GameState::kLost) Button::Draw();
    }
};

class PauseButton : public Button {
public:
    PauseButton(int x, int y, int width, int height) : Button("Pause", x, y, width, height) {}
    void OnClick() override { g_paused = !g_paused; }
    void OnUpdate() override {
        if (g_state == GameState::kPlaying) Button::OnUpdate();
    }
    void Draw() override {
        if (g_state == GameState::kPlaying) Button::Draw();
    }
};

}  // namespace

int main() {
    try {
        g_level = LoadLevelMap("map.txt");

        GridEngine game(g_level.cols, g_level.rows, 32);
        g_game = &game;
        game.LoadAssets("pacman.db");
        game.set_background_color(BLACK);  // Pac-Man 經典黑底（格線預設已關）

        BuildLevel(game, g_level);  // 先建好一局
        g_state = GameState::kStart;

        // 畫面覆蓋層（ScoreOverlay 先加，按鈕畫在它之上）
        constexpr int kButtonWidth = 120;
        constexpr int kButtonHeight = 40;
        const int button_x = g_level.cols * 32 / 2 - kButtonWidth / 2;
        const int button_y = g_level.rows * 32 / 2;
        game.AddOverlay(new ScoreOverlay());
        game.AddOverlay(new StartButton(button_x, button_y, kButtonWidth, kButtonHeight));
        game.AddOverlay(new RestartButton(button_x, button_y, kButtonWidth, kButtonHeight));
        game.AddOverlay(new PauseButton(g_level.cols * 32 - 88, 6, 82, 24));

        game.Run();
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
