// 第 1 步：節奏大師——按空白鍵，看看兩次之間隔了多久。
#include <string>

#include "GridPlusPlus.h"

using gridpp::GameEngine;
using gridpp::GridObject;
using gridpp::Overlay;

Overlay message;
double last_press = -1;  // 上一次按空白鍵的時間（秒）；-1 代表還沒按過

void Dance(GameEngine game, GridObject self) {
    if (!game.keyPressed(KEY_SPACE)) return;  // 這一幀沒按空白鍵，就什麼都不做

    self.setDirection(self.direction() + 1);  // 小精靈轉 90 度

    double now = game.time();
    if (last_press >= 0) {
        int gap = (now - last_press) * 1000;  // 換算成毫秒
        if (gap >= 950 && gap <= 1050) {
            message.setText(std::to_string(gap) + " ms  PERFECT!");
            message.setColor(GREEN);
        } else {
            message.setText(std::to_string(gap) + " ms");
            message.setColor(YELLOW);
        }
    }
    last_press = now;
}

int main() {
    GameEngine game(15, 11, 48);
    game.loadAssets("pacman.db");
    game.setBackgroundColor(BLACK);

    GridObject pacman = game.addObject("pacman", nullptr, Dance);
    pacman.setPosition(7, 5);

    message = game.addTextOverlay("Press SPACE!", 20, 20, 40, YELLOW);
    game.addTextOverlay("Goal: press exactly 1 second apart", 20, 480, 24, GRAY);

    game.run();
    return 0;
}
