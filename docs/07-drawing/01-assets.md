# 從素材名稱到素材包

遊戲規則需要一種穩定的方式描述外觀，否則程式一旦直接依賴圖片檔名、資料夾位置與圖形函式庫的紋理，更換美術時便必須連帶修改移動與碰撞程式。Grid++ 因此讓物件只保存 `pacman`、`ghost` 或 `wall` 之類的素材名稱，Engine 再從已載入的素材包尋找對應圖片；規則知道角色「使用 pacman 外觀」，卻不必知道圖片如何保存或何時釋放。

## 先載入並使用既有素材包

素材包必須在物件開始繪製之前載入，最常見的順序是在建立 Engine 後呼叫 `loadAssets()`，接著才用素材名稱建立物件。以下程式假設 `assets.db` 與執行中的 `game` 位於同一個目錄；相對路徑是以啟動程式時的目前目錄為基準，而不是以 `main.cpp` 所在位置為基準，因此從不同目錄執行時也必須相應調整路徑。

```cpp title="main.cpp 節錄：先載入素材再建立物件"
int main() {
    gridpp::GameEngine game(10, 10, 48);
    game.loadAssets("assets.db");

    gridpp::GridObject pacman = game.addObject("pacman", nullptr, MovePacman);
    pacman.setPosition(4, 5);

    game.run();
    return 0;
}
```

`addObject()` 的第一個參數就是素材名稱，而 `setImage()` 可以在執行期間切換名稱，因此同一個玩家可以在張嘴與閉嘴之間改變畫面，卻不必重新載入素材包或建立新物件。

```cpp title="物件更新函式節錄：切換素材名稱"
if (self.image() == "pacman_closed") {
    self.setImage("pacman_open");
} else {
    self.setImage("pacman_closed");
}
```

Engine 會把圖片縮放到一格大小，再套用物件的方向與顏色；如果素材名稱是空字串、沒有載入素材包，或找不到相符圖片，畫面會以紅色方塊代替，讓座標、移動與碰撞仍可繼續測試。這個方塊是找不到素材時的警示，不是正式美術的一部分，所以看到它時應先檢查執行目錄、資料庫路徑與名稱拼字。

## 圖片 Overlay

Overlay 也能顯示素材包中的圖片，例如畫面角落的生命圖示。`addOverlay()` 建立的圖片 Overlay 使用像素座標，一開始位於視窗左上角 `(0, 0)`，並以素材原本的 32×32 像素大小顯示，不會配合格子縮放：

```cpp title="main() 節錄：以圖片顯示生命數"
gridpp::Overlay heart = game.addOverlay("heart");
heart.setPosition(8, 8);
```

和網格物件一樣，`setImage()` 可以在執行期間更換圖片，找不到素材時會以 32×32 的紅色方塊代替。

## 更換整組素材

再次呼叫 `loadAssets()` 可以把目前素材換成另一個素材包，這適合在不改變規則的情況下切換主題或關卡美術。Engine 會先驗證並載入新內容，成功後才取代舊素材；若檔案無法開啟或格式無效，函式會丟出 `std::runtime_error`，原本已能使用的素材仍會保留。

```cpp title="載入錯誤處理節錄"
try {
    game.loadAssets("level-two.db");
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
}
```

素材包只負責圖像，不保存物件位置、碰撞規則或遊戲狀態，因此多個物件可以共用同一張圖片，再各自設定方向、顏色與 layer；`clearObjects()` 也不會卸載素材，使重新建立關卡時不必重複載入。若遊戲只需要幾何圖形，則可完全省略 `loadAssets()`，下一節會改用基本圖形建立不依賴外部圖片的畫面。

## 從 PNG 建立真正的素材包

專案提供 [`tools/create_asset_pack.py`](https://github.com/GridPlusPlus/GridPlusPlus/blob/main/tools/create_asset_pack.py)，可以把多張 32×32 PNG 轉成 Grid++ 使用的素材包。工具只使用 Python 標準函式庫，不需要另裝套件。執行時以 `素材名稱=圖片路徑` 的格式列出圖片；素材名稱就是之後傳給 `addObject()` 或 `setImage()` 的字串。

```bash
python3 tools/create_asset_pack.py assets.db \
    pacman=images/pacman.png \
    ghost=images/ghost.png \
    pellet=images/pellet.png
```

指令成功後會顯示寫入數量，並在目前目錄建立 `assets.db`。工具接受非交錯的 8-bit RGB 或 RGBA PNG，並將像素統一轉成 RGBA；不存在的檔案、重複名稱及不是 32×32 的圖片都會被拒絕。建立完成後，將 `game.loadAssets("assets.db")` 放在建立物件之前，再確認畫面不再出現紅色替代方塊。

!!! note "圖片尺寸"

    轉換工具刻意拒絕自動縮放，因為縮放像素圖可能造成邊緣模糊，而且作者應該明確決定裁切方式。請先在繪圖軟體中把圖片整理成 32×32 PNG；透明區域也會原樣保留。

## 補充：素材包的內部格式

!!! info "需要自行製作素材包時再閱讀"

    Grid++ 素材包是一個 SQLite 資料庫，其中 `sprites` 資料表依序包含 `id`、`name`、`tags` 與 `image_data` 四個欄位。`image_data` 是 32×32 像素的原始 RGBA 資料，每個像素各用一個 byte 保存紅、綠、藍與透明度，所以每張圖片必須剛好包含 `32 × 32 × 4 = 4096` bytes；大小不符的圖片會在載入時被略過並顯示警告。同一素材包內的名稱也應保持唯一，重複的名稱會在載入時顯示警告，並在繪製該素材時發生錯誤。

    下列程式只使用 Python 標準庫，建立一張不透明紅色圖片並寫入 `assets.db`。它的目的不是取代上面的 PNG 工具，而是讓想理解資料格式的讀者看見 4096 bytes 如何進入 SQLite。

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

    `with` 區塊會在正常結束時提交變更，而 SQL 的 `?` 參數避免自行拼接資料。PNG 是壓縮檔案，不能把它的檔案 bytes 直接當成 `image_data`；前一節的工具會先解碼 PNG，才將像素寫入資料庫。

## 本節小結

- 程式以素材名稱引用圖片，Engine 負責載入、繪製與釋放素材。
- 先確認執行目錄與 `loadAssets()` 路徑，再用 `addObject()` 或 `setImage()` 選擇圖片。
- 紅色方塊代表素材尚未找到，可供規則測試，但不代表載入成功。
- 圖片 Overlay 以 32×32 像素顯示在指定的像素座標。
- `tools/create_asset_pack.py` 會把具名的 32×32 PNG 轉成可載入的素材包。

[不需素材的基本圖形](02-shapes.md){ .md-button .md-button--primary }
