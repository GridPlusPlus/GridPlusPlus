# 從素材名稱到素材包

遊戲規則需要一種穩定的方式描述外觀，否則程式一旦直接依賴圖片檔名、資料夾位置與 raylib texture，更換美術時便必須連帶修改移動與碰撞程式。Grid++ 因此讓物件只保存 `player`、`ghost` 或 `wall` 之類的素材名稱，Engine 再從已載入的素材包尋找對應圖片；規則知道角色「使用 player 外觀」，卻不必知道圖片如何保存或何時釋放。

## 先載入並使用既有素材包

素材包必須在物件開始繪製之前載入，最常見的順序是在建立 Engine 後呼叫 `LoadAssets()`，接著才用素材名稱生成物件。以下程式假設 `assets.db` 與執行中的 `game` 位於同一個目錄；相對路徑是以啟動程式時的目前目錄為基準，而不是以 `main.cpp` 所在位置為基準，因此從不同目錄執行時也必須相應調整路徑。

```cpp
int main() {
    gridpp::GridEngine game(10, 10, 48);
    game.LoadAssets("assets.db");
    game.Spawn("player", 4, 5, MovePlayer);
    game.Run();
}
```

`GridObject` 建構子與函式式 `Spawn()` 的第一個參數都是素材名稱，而 `set_asset_name()` 可以在執行期間切換名稱，因此同一個玩家可以在張嘴與閉嘴之間改變畫面，卻不必重新載入素材包或建立新物件。

```cpp
GridObject* player = game.Spawn("player_closed", 2, 3, MovePlayer);
player->set_asset_name("player_open");
```

Engine 會把圖片縮放到一格大小，再套用物件的 `direction` 與 `tint`；如果素材名稱是空字串、沒有載入素材包，或找不到相符圖片，畫面會以紅色方塊代替，讓座標、移動與碰撞仍可繼續測試。這個方塊是找不到素材時的警示，不是正式美術的一部分，所以看到它時應先檢查執行目錄、資料庫路徑與名稱拼字。

## 更換整組素材

再次呼叫 `LoadAssets()` 可以把目前素材換成另一個素材包，這適合在不改變規則的情況下切換主題或關卡美術。Engine 會先驗證並載入新內容，成功後才取代舊素材；若檔案無法開啟或格式無效，函式會丟出 `std::runtime_error`，原本已能使用的素材仍會保留。

```cpp
try {
    game.LoadAssets("level-two.db");
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
}
```

素材包只負責圖像，不保存物件位置、碰撞規則或遊戲狀態，因此多個物件可以共用同一張圖片，再各自設定方向、顏色與 z-index；`ClearObjects()` 也不會卸載素材，使重新建立關卡時不必重複載入。若遊戲只需要幾何圖形，則可完全省略 `LoadAssets()`，下一節會改用 `GridShapes.h` 建立不依賴外部圖片的畫面。

## 補充：素材包的內部格式

!!! info "需要自行製作素材包時再閱讀"

    Grid++ 素材包是一個 SQLite 資料庫，其中 `sprites` 資料表依序包含 `id`、`name`、`tags` 與 `image_data` 四個欄位；`image_data` 是 32×32 像素的原始 RGBA 資料，每個像素各用一個 byte 保存紅、綠、藍與透明度，所以每張圖片必須剛好包含 `32 × 32 × 4 = 4096` bytes。PNG 與 JPEG 仍是壓縮檔案格式，不能直接把檔案內容寫進這個欄位，而同一素材包內的名稱也應保持唯一。

    下列完整程式只使用 Python 標準庫，建立一張不透明紅色圖片並寫入 `assets.db`；它刻意使用純色資料，使範例從資料產生到資料庫寫入都能直接執行，不會把「先用其他工具轉換圖片」這個未說明步驟留給讀者猜測。

    ```python title="create_assets.py"
    import sqlite3

    width = 32
    height = 32
    image_bytes = bytes((220, 40, 40, 255)) * (width * height)

    if len(image_bytes) != 4096:
        raise ValueError("Asset image must contain exactly 4096 RGBA bytes")

    with sqlite3.connect("assets.db") as db:
        db.execute(
            """
            CREATE TABLE IF NOT EXISTS sprites (
                id INTEGER PRIMARY KEY,
                name TEXT,
                tags TEXT,
                image_data BLOB
            )
            """
        )
        db.execute(
            "INSERT INTO sprites (name, tags, image_data) VALUES (?, ?, ?)",
            ("red_mole", "mole,enemy", image_bytes),
        )
    ```

    `with` 區塊會在正常結束時提交變更並關閉連線，而 SQL 的 `?` 參數避免自行拼接資料。這一節說明的是 Grid++ 目前接受的內部格式；若要把真正的 PNG 圖片轉成素材包，仍需要先用能解碼圖片的工具取得 32×32 RGBA 像素，而不是把 PNG bytes 當成 `image_data`。

## Summary

- 程式以素材名稱引用圖片，Engine 負責載入、繪製與釋放素材。
- 先確認執行目錄與 `LoadAssets()` 路徑，再用 `Spawn()` 或 `set_asset_name()` 選擇圖片。
- 紅色方塊代表素材尚未找到，可供規則測試，但不代表載入成功。
- SQLite、RGBA 與 4096 bytes 屬於製作素材包時才需要理解的進階細節。
