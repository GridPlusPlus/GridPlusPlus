# GridObject

遊戲世界由許多具有位置和行為的實體組成。玩家會移動，敵人會追蹤，道具會在碰撞後消失；即使它們外觀和規則不同，都需要回答相同的基本問題：目前位於哪一格、應該畫成什麼、是否可見，以及和其他物件同格時要做什麼。

`GridObject` 是 Grid++ 對這類遊戲實體的共同描述，也是玩家、敵人、道具和其他網格內容共用的基底。GridObject 把所有物件都會使用的資料和生命週期放在同一個介面中，Engine 因此可以用一致方式更新、碰撞和繪製不同類型的內容。

## 資料與行為

一個 GridObject 包含兩類資訊。「資料」描述物件現在的狀態，例如 x、y、素材、方向、顏色與顯示層級；「行為」描述狀態何時改變，例如按鍵後移動、碰到玩家後消失，或每隔一段時間切換素材。

GridObject 已經提供所有物件共通的資料。行為則有兩種提供方式：將普通函式交給 `CallbackGridObject`，或建立衍生 class 覆寫生命週期函式。無論選擇哪一種，Engine 看到的都是 GridObject，因此兩種物件可以同時存在於同一個遊戲。

## 物件與 Engine 的關係

只在 C++ 中建立一個物件，並不會讓它自動出現在遊戲裡。物件必須透過 `GridEngine::Spawn()` 加入某個 Engine，才能進入該 Engine 的更新、碰撞與繪製流程。Spawn 也會把物件的所有權交給 Engine，使資源清理只有一個明確負責者。

下列程式建立一個位於 `(3, 2)` 的物件，並保留 Engine 回傳的借用指標。

```cpp
GridObject* player = game.Spawn("player", 3, 2, nullptr);
```

這個多載會建立 `CallbackGridObject`，但回傳型別是它的基底類別 `GridObject*`。呼叫端只需使用所有網格物件共通的 API，不必依賴實際類別。

## 將物件加入打地鼠

目前的 8×8 Engine 還沒有任何遊戲內容。加入一個地鼠物件只需要素材名稱與網格位置；第三個參數暫時傳入 `nullptr`，表示物件目前沒有每幀更新函式。

```cpp
int main() {
    GridEngine game(8, 8, 64);
    game.set_background_color(BEIGE);
    game.set_show_grid(true);

    GridObject* mole = game.Spawn("mole", 3, 4, nullptr);
    mole->set_tag("mole");

    game.Run();
}
```

程式尚未載入名為 `mole` 的素材，因此畫面會在第 3 欄、第 4 列顯示紅色 fallback 方塊。這個結果已足以驗證 `Spawn()`、座標與繪製；素材可以在功能正確後再加入。

## 位置與移動

`x()` 和 `y()` 讀取目前位置；`set_x()` 與 `set_y()` 設定單一座標；`Move(dx, dy)` 以相對位移修改兩個座標。Grid++ 不會自動阻止物件離開地圖，移動規則應在遊戲程式中根據 `engine()->cols()`、`engine()->rows()` 或 `GridMaze::IsWall()` 判斷。

```cpp
player->set_x(4);
player->set_y(5);
player->Move(-1, 0);  // 移動至 (3, 5)

int column = player->x();
int row = player->y();
```

位於地圖外的物件仍會執行 `OnUpdate()` 和繪製，但不參與碰撞。若素材繪製位置也在視窗外，raylib 會自然裁掉看不見的部分。將物件移至 `(-1, -1)` 可以讓它離開畫面，但需要暫停碰撞與繪製時，應使用 `set_visible(false)` 表達意圖。

## 素材與外觀

`asset_name()` 指定 `Render()` 使用的素材。`set_asset_name()` 可在遊戲執行期間切換素材。素材名稱不存在或尚未載入素材包時，預設繪製會顯示紅色方塊，讓程式在沒有素材的情況下仍可測試。

```cpp
player->set_asset_name("player_open");
player->set_direction(1);
player->set_tint(YELLOW);
```

`direction` 以 90 度為單位旋轉素材，數值會正規化到 0～3。方向增加時，素材逆時針旋轉。`tint` 使用 raylib 的 `Color`；`WHITE` 保留原始素材顏色，其餘顏色會與素材混合。

`tag` 是由遊戲自行定義的文字標記，通常用於碰撞時辨識物件類型。它不會自動改變繪製或碰撞規則。

```cpp
player->set_tag("player");

if (other->tag() == "pellet") {
    // 處理玩家吃到豆子。
}
```

## 顯示狀態

`set_visible(false)` 隱藏物件。隱藏的物件不會繪製，也不會參與碰撞，但仍會每幀執行 `OnUpdate()`。這項行為適合需要暫時消失、之後再次出現的物件。

```cpp
mole->set_visible(false);

// 之後可以重新使用同一物件。
mole->set_x(6);
mole->set_y(1);
mole->set_visible(true);
```

若物件之後不會再使用，呼叫 `game.Destroy(object)` 釋放它。不要在 `set_visible(false)` 後自行 `delete` 指標；物件仍由引擎持有。

## 繪製順序

`z_index` 控制 `GridObject` 的繪製層級。數值較小的物件先畫，數值較大的物件後畫，因此較大的值顯示在上方。預設值是 0，也可以使用負數。

```cpp
floor->set_z_index(-10);
pellet->set_z_index(0);
player->set_z_index(10);
```

相同 z-index 保留 spawn 順序，後 spawn 的物件會較晚繪製。z-index 只影響畫面，不改變更新或碰撞 callback 的執行順序。Overlay 永遠繪製在所有 `GridObject` 上方。

## 所有權

`Spawn(GridObject*)` 與 `AddOverlay(Overlay*)` 接受 raw pointer，是為了讓入門程式保持直接；所有權會在呼叫成功後轉交給引擎。只能傳入使用 `new` 建立、尚未交給其他引擎的物件。

```cpp
// 正確：引擎接管 new 建立的物件。
GridObject* object = game.Spawn(new GridObject("box", 1, 1));

// 錯誤：區域變數不是由引擎配置，之後不可由引擎 delete。
GridObject local("box", 1, 1);
game.Spawn(&local);
```

同一指標不可 spawn 兩次，也不可同時交給兩個引擎。`Spawn()` 回傳的指標只在物件仍存在時有效；呼叫 `Destroy()`、`ClearObjects()` 或讓引擎結束生命週期後，不可再次讀取該指標。

完整成員列表見 [GridObject API](../api/classgridpp_1_1_grid_object.md)。
