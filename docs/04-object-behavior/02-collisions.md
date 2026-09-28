# 從同格位置判斷互動

網格座標除了決定物件畫在哪裡，也提供了一種比像素邊界更容易理解的互動規則：玩家和豆子進入同一格時可以收集豆子，玩家和敵人進入同一格時則可能失去生命。Grid++ 會在每一幀完成物件更新之後比較座標，當兩個可見物件位於同一個有效格子時，才把它們交給各自的 Collide 函式處理；Engine 因而只回答「誰和誰相遇」，至於相遇代表加分、受傷或毫無影響，仍由遊戲程式決定。

Collide 函式會收到 `self` 與 `other` 兩個 Handler，其中 `self` 是正在處理事件的物件，`other` 則是和它同格的對象。由於單看座標無法知道對方在遊戲中扮演什麼角色，接下來還需要為物件補上可供規則判斷的身分。

## 在物件上記錄種類

每個 GridObject 都可以用 `set(key, value)` 保存額外資料，再用 `get(key, output)` 讀回來。這項能力第 5 章會完整介紹，現在先用它做一件簡單的事：為每個物件記錄一個 `"type"`，讓碰撞函式能區分玩家、豆子與敵人。以下程式先把玩家標記為 `player`，再讓豆子的 `CollectPellet` 只接受這種對象，因此其他物件即使經過豆子所在的格子，也不會誤觸得分規則。

```cpp title="main.cpp（碰撞函式放在 main() 前，建立物件放在 main() 內）"
int score = 0;

void CollectPellet(GameEngine, GridObject self, GridObject other) {
    std::string type;
    other.get("type", type);
    if (type != "player") return;

    ++score;
    self.remove();
}

// 放在 main() 內：
GridObject player = game.addObject("player", nullptr, MovePlayer);
player.setPosition(1, 1);
player.set("type", "player");

GridObject pellet = game.addObject("pellet", nullptr, nullptr, CollectPellet);
pellet.setPosition(3, 1);
pellet.set("type", "pellet");
```

`other.get("type", type)` 把對方保存的種類讀進字串 `type`；如果對方沒有設定過 `"type"`，`type` 會是空字串，同樣不等於 `"player"`。使用 `std::string` 時記得在檔案開頭加上 `#include <string>`。

玩家走到 `(3, 1)` 後，Engine 會把這次相遇交給豆子的 `CollectPellet`，函式確認對方的種類之後才增加分數，最後呼叫 `self.remove()` 讓豆子離開遊戲。移除會立即生效：豆子不再參與這一幀剩下的碰撞，也不會被畫出來，所有指向它的 Handler 都會失效。因此 `remove()` 通常是函式中最後一個操作，之後不應再透過 `self` 讀寫這顆豆子。這項規則如何保護同一幀中尚未完成的工作，會在[第 8 章的執行期間新增與刪除](../08-lifecycle/03-runtime-changes.md)統一說明。

## 同一場相遇可以有兩種反應

玩家碰到敵人時，玩家可能要扣除生命，敵人也可能要切換追逐模式，因此 Grid++ 會讓同一場相遇中的兩個物件各自處理自己的行為；對玩家而言 `self` 是玩家而 `other` 是敵人，輪到敵人時兩者則會交換。現階段只要知道雙方都可能收到通知，並讓每個函式專心修改自己負責的狀態即可，不需要依賴哪一方先執行。

```cpp title="碰撞函式示意：雙方各自處理反應"
void HitPlayer(GameEngine, GridObject self, GridObject other) {
    std::string type;
    other.get("type", type);
    if (type == "ghost") self.hide();
}

void TouchPlayer(GameEngine, GridObject self, GridObject other) {
    std::string type;
    other.get("type", type);
    if (type == "player") self.setColor(RED);
}
```

如果遊戲規則會在碰撞途中隱藏、移動或移除物件，後續通知是否仍會發生便與同一幀的處理順序有關；這不是剛開始撰寫碰撞規則時應該猜測的細節，因此第 8 章會配合完整的幀流程，說明建立順序、雙向通知與移除之間的關係。

## 哪些物件會參與判定

同格判定只處理目前確實存在於遊戲世界中的物件，因此物件必須同時符合下列條件：

- `visible()` 為 `true`，也就是沒有被 `hide()`。
- x 位於 `0` 到 `game.cols() - 1`。
- y 位於 `0` 到 `game.rows() - 1`。
- 尚未被 `remove()`。
- 已經開始運作；這一幀才建立的物件要到下一幀才會參與。

位於 `(-1, -1)` 或其他地圖外座標的物件仍然可以更新，卻不會被視為站在某個有效格子中；若目的只是暫時停用物件，使用 `hide()` 會同時停止它的繪製與碰撞，而且比用特殊座標暗示狀態更容易閱讀。反過來說，layer 只負責畫面前後層次，並不會讓畫在上方的物件取得碰撞優先權。

## 從相遇規則走向物件設計

到目前為止，一顆豆子只需要素材、座標、種類與一個 Collide 函式，使用普通函式便能把規則說清楚。然而，敵人若還要分別記住生命值、移動方向與下一次行動時間，所有敵人共用的函式與全域變數就無法表達資料屬於哪一個物件。碰撞機制已經回答物件如何互動；下一章將從三隻地鼠共用計時器的錯誤結果出發，處理每個物件如何保存自己的狀態。

## 本節小結

- 同格碰撞在物件更新完成後檢查，Engine 判斷相遇，遊戲程式決定相遇的意義。
- 用 `set("type", ...)` 記錄種類，Collide 函式再以 `get()` 辨認碰撞對象。
- 同一場相遇中的雙方可以各自處理反應，精確順序集中於第 8 章說明。
- 隱藏、越界、已移除或本幀才建立的物件不參與碰撞。

[讓每個物件保存自己的狀態](../05-object-state/index.md){ .md-button .md-button--primary }
