/** @file template.cpp
 *  @brief Grid++ 最小專案範本；完整教學見 docs/01-getting-started/02-hello-gridpp.md。
 */
#include "GridPlusPlus.h"

using gridpp::GameEngine;

int main() {
    GameEngine game(8, 8, 64);
    game.showGrid(true);
    game.run();
    return 0;
}
