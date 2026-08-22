/** @file GridPlusPlus.h
 *  @brief 引入 Grid++ 的核心 API。
 */
#ifndef GRID_PLUS_PLUS_GRID_PLUS_PLUS_H_
#define GRID_PLUS_PLUS_GRID_PLUS_PLUS_H_

#include "GridEngine.h"
#include "GridObject.h"
#include "Overlay.h"

namespace gridpp {

// 此處已具備 GridObject 與 GridEngine 的完整定義。
inline void GridObject::Render(GridEngine* engine) {
    engine->DrawCell(asset_name_, grid_x_, grid_y_, direction_, tint_);
}

}  // namespace gridpp

#endif  // GRID_PLUS_PLUS_GRID_PLUS_PLUS_H_
