// =============================================================================
//  examples/pacman_easy —— Pac-Man「入門版」（示範 callback Spawn）
//
//  刻意寫得像 C：沒有自訂 class、沒有繼承，只用「全域變數 + 一般函式」。
//  角色的行為靠傳給 Spawn 的函式決定，不需要自訂 class。
//  因為只有一個玩家、一隻鬼，它們的狀態放全域變數就夠了。
//
//  玩法：方向鍵移動，吃光所有豆子獲勝，碰到鬼魂失敗。（結束後畫面會定格）
//
//  編譯（在本資料夾內，raylib 已安裝；其他平台見 docs/getting-started.md）：
//    g++ -std=c++17 main.cpp -I../.. -o game -lraylib -lopengl32 -lgdi32 -lwinmm
//  執行前確保本資料夾有 pacman.db（素材，沿用 examples/pacman 的那份）。
// =============================================================================
#include <cstdio>

#include "GridMaze.h"
#include "GridPlusPlus.h"

using gridpp::GridEngine;
using gridpp::GridMaze;
using gridpp::GridObject;

namespace {

constexpr int kWidth = 11;
constexpr int kHeight = 9;

// 地圖：1=牆  0=豆子  2=玩家起點  3=鬼魂起點
constexpr int kMap[kHeight][kWidth] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}, {1, 0, 0, 0, 0, 3, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}, {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

// 遊戲用的全域變數（只有一個玩家、一隻鬼，所以放全域就好）
int g_state = 0;                 // 0=遊戲中  1=獲勝  2=失敗
int g_pellets = 0;               // 還剩幾顆豆子
int g_ghost_timer = 0;           // 鬼的移動計時器
GridMaze* g_maze = nullptr;      // 用來查某一格是不是牆
GridObject* g_player = nullptr;  // 讓鬼知道玩家在哪

// 這一格是不是牆？（玩家和鬼都用它來判斷能不能走）
int IsWall(int x, int y) { return g_maze->IsWall(x, y); }

// 玩家：方向鍵按一下走一格，前面是牆就不走
void MovePlayer(GridObject* self) {
    if (g_state != 0) return;
    const int x = self->x();
    const int y = self->y();
    if (IsKeyPressed(KEY_RIGHT) && !IsWall(x + 1, y)) self->Move(1, 0);
    if (IsKeyPressed(KEY_LEFT) && !IsWall(x - 1, y)) self->Move(-1, 0);
    if (IsKeyPressed(KEY_DOWN) && !IsWall(x, y + 1)) self->Move(0, 1);
    if (IsKeyPressed(KEY_UP) && !IsWall(x, y - 1)) self->Move(0, -1);
}

// 玩家撞到東西：撞到鬼就失敗
void HitPlayer(GridObject* self, GridObject* other) {
    (void)self;
    if (other->tag() == "ghost") {
        g_state = 2;
        SetWindowTitle("GAME OVER");
        std::printf("被鬼抓到了，失敗！\n");
    }
}

// 豆子被玩家吃到：隱藏並停止碰撞，剩餘數量 -1；吃光就贏
void EatPellet(GridObject* self, GridObject* other) {
    if (other->tag() == "player") {
        self->set_visible(false);
        --g_pellets;
        if (g_pellets <= 0) {
            g_state = 1;
            SetWindowTitle("YOU WIN!");
            std::printf("豆子吃光了，獲勝！\n");
        }
    }
}

// 鬼：每隔幾幀往玩家的方向走一格（會避開牆）
void MoveGhost(GridObject* self) {
    if (g_state != 0) return;
    if (++g_ghost_timer < 15) return;
    g_ghost_timer = 0;

    const int ghost_x = self->x();
    const int ghost_y = self->y();
    const int player_x = g_player->x();
    const int player_y = g_player->y();

    // 先往玩家的左右方向靠近，走不動再試上下
    if (player_x > ghost_x && !IsWall(ghost_x + 1, ghost_y)) {
        self->Move(1, 0);
    } else if (player_x < ghost_x && !IsWall(ghost_x - 1, ghost_y)) {
        self->Move(-1, 0);
    } else if (player_y > ghost_y && !IsWall(ghost_x, ghost_y + 1)) {
        self->Move(0, 1);
    } else if (player_y < ghost_y && !IsWall(ghost_x, ghost_y - 1)) {
        self->Move(0, -1);
    }
}

}  // namespace

int main() {
    GridEngine game(kWidth, kHeight, 40);
    game.LoadAssets("pacman.db");
    game.set_background_color(BLACK);

    // 建立迷宮：所有牆都用同一張圖（最簡單的畫法）
    g_maze = new GridMaze(kWidth, kHeight);
    g_maze->SetWallAsset("wall_cross");
    game.Spawn(g_maze);

    // 照著 kMap 擺好牆、豆子、玩家、鬼
    for (int y = 0; y < kHeight; ++y) {
        for (int x = 0; x < kWidth; ++x) {
            const int tile = kMap[y][x];
            if (tile == 1) {
                g_maze->SetWall(x, y, true);
            } else if (tile == 0) {
                game.Spawn("pellet", x, y, nullptr, EatPellet);
                ++g_pellets;
            } else if (tile == 2) {
                g_player = game.Spawn("pacman", x, y, MovePlayer, HitPlayer);
                g_player->set_tag("player");
            } else if (tile == 3) {
                GridObject* ghost = game.Spawn("ghost", x, y, MoveGhost);
                ghost->set_tag("ghost");
                ghost->set_tint(RED);
            }
        }
    }

    SetWindowTitle("Pac-Man (Easy) - arrow keys");
    game.Run();
    return 0;
}
