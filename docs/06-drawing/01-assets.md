# 素材與素材包

遊戲規則需要一種穩定的方式描述物件外觀。若程式直接依賴每張圖片的檔名、資料夾和 raylib texture，替換美術或分享專案時就必須同時修改許多程式碼。Grid++ 將「物件使用哪個外觀」表示成素材名稱，例如 `player`、`ghost` 或 `wall`。

素材是可供物件繪製的圖片；素材名稱是程式引用圖片的識別字。素材包則把一組相關素材整理成單一檔案。遊戲只保存名稱，Engine 負責從已載入的素材包找到圖片、建立 GPU texture，並在結束時釋放資源。

這個分離表示遊戲規則不需要知道圖片存在哪裡。玩家物件使用 `player` 素材，但移動與碰撞仍由程式決定；換一份包含同名素材的素材包，可以改變畫面而不改變規則。

Grid++ 素材包實際上是一個包含 32×32 RGBA 圖片的資料庫檔案。它需在物件開始繪製前由 `GridEngine::LoadAssets()` 載入。

```cpp
int main() {
    gridpp::GridEngine game(10, 10, 48);
    game.LoadAssets("assets.db");
    game.Spawn("player", 4, 5, MovePlayer);
    game.Run();
}
```

相對路徑以執行程式時的目前目錄為基準。若從專案根目錄執行 `./game`，`assets.db` 也應位於專案根目錄，或在程式中提供正確的相對路徑。

## 指定素材

`GridObject` 建構子或函式式 `Spawn()` 的第一個參數是素材名稱。引擎繪製物件時，會在目前素材包中尋找相同名稱的圖片。

```cpp
GridObject* player = game.Spawn("player", 2, 3, MovePlayer);
player->set_asset_name("player_open");
```

`set_asset_name()` 只修改名稱，不會重新載入素材包。這適合用於角色動畫或狀態切換，例如在 `player_open` 與 `player_closed` 之間交替。素材會縮放到一格的大小，再套用物件的 `direction` 與 `tint`。

空字串或不存在的素材名稱會以紅色方塊顯示。這項 fallback 讓位置、移動與碰撞可以在素材完成前測試，但不應用來判斷正式素材是否正確載入。

## 載入與取代

再次呼叫 `LoadAssets()` 會在新素材全部驗證並載入成功後，取代舊素材。若新檔案無法開啟或格式無效，函式會丟出 `std::runtime_error`，原本已載入的素材仍然保留。

```cpp
try {
    game.LoadAssets("level-two.db");
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
}
```

素材名稱必須唯一。重複名稱會在載入時輸出警告，之後嘗試取得該名稱時丟出例外，因為引擎無法判斷應使用哪張圖片。不是 32×32 RGBA 的資料會被略過並輸出警告。

## 素材與物件的關係

素材包只負責圖像，不保存物件位置、碰撞規則或遊戲狀態。同一素材可由多個 `GridObject` 共用，每個物件仍可設定不同的方向、顏色與 z-index。`ClearObjects()` 不會卸載素材，因此重新建立關卡時不必重複呼叫 `LoadAssets()`。

不需要圖片的遊戲可以省略 `LoadAssets()`，改用 `GridShapes.h` 直接呼叫 raylib 繪製圖形。
