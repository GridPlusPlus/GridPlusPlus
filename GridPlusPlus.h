/**
 * @file GridPlusPlus.h
 * @brief Grid++ 主標頭。
 *
 * 一般使用者只需要 include 此檔；其餘核心標頭僅為方便維護而拆分。
 * 此檔會組合遊戲引擎、網格物件與畫面覆蓋層。
 */
#ifndef GRID_PLUS_PLUS_GRID_PLUS_PLUS_H_
#define GRID_PLUS_PLUS_GRID_PLUS_PLUS_H_

#include "GridEngine.h"
#include "GridObject.h"
#include "Overlay.h"

namespace gridpp {

// 跨核心類別的 inline 實作放在主標頭最後。
// GridObject.h 只能前向宣告 GridEngine；到這裡兩個類別都已完整定義，
// 才能安全呼叫 GridEngine::DrawCell()，同時避免兩個標頭互相 include。
inline void GridObject::Render(GridEngine* engine) {
    engine->DrawCell(asset_name_, grid_x_, grid_y_, direction_, tint_);
}

}  // namespace gridpp

#endif  // GRID_PLUS_PLUS_GRID_PLUS_PLUS_H_
