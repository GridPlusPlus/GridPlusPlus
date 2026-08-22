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

## 素材包格式

素材包是標準的 SQLite 資料庫檔案，例如 Pacman 範例使用的 `examples/pacman/pacman.db`。Grid++ 內含一個唯讀的 SQLite 讀取器，會直接解析資料庫並把圖片建立成 raylib 的 `Texture2D`。執行遊戲時不需要另外安裝或連結 SQLite；建立或修改素材包時，則可以使用 Python 標準庫提供的 `sqlite3`。

資料庫必須包含一張名為 `sprites` 的資料表，並依下列順序定義四個欄位。Grid++ 依欄位位置讀取資料，因此建立資料表時不可調換欄位順序。

| 欄位 | 型別 | 內容 |
| --- | --- | --- |
| `id` | `INTEGER PRIMARY KEY` | 每筆素材的主鍵。 |
| `name` | `TEXT` | 程式用來取得素材的名稱，例如 `player`。 |
| `tags` | `TEXT` | 以逗號分隔的分類標籤；目前不影響載入或繪製。 |
| `image_data` | `BLOB` | 32×32 像素、依 R、G、B、A 排列的原始位元組。 |

每個像素包含紅、綠、藍與透明度四個位元組，因此一張素材必須剛好包含 `32 × 32 × 4 = 4096` bytes。`image_data` 保存的是解碼後的原始像素；PNG 或 JPEG 的檔案內容仍包含壓縮格式，不能直接寫入這個欄位。

名稱是程式與圖片之間的介面。資料庫本身允許重複名稱，但 Grid++ 載入時會輸出警告，取得重複名稱時則會丟出例外。每個素材名稱應在同一個素材包中保持唯一。

## 建立素材包

以下程式使用 Python 標準庫建立 `assets.db`，並加入一張名為 `red_mole` 的紅色素材。將程式存成 `create_assets.py` 後執行 `python3 create_assets.py`。

```python title="create_assets.py"
import sqlite3

WIDTH = 32
HEIGHT = 32
RGBA_CHANNELS = 4

# 每個像素依序包含 R、G、B、A。這裡建立不透明的紅色圖片。
image_bytes = bytes((220, 40, 40, 255)) * (WIDTH * HEIGHT)

if len(image_bytes) != WIDTH * HEIGHT * RGBA_CHANNELS:
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

`sqlite3.connect()` 會在檔案不存在時建立資料庫；`with` 區塊正常結束時會提交變更並關閉連線。SQL 使用 `?` 參數傳入名稱、標籤與二進位資料，避免自行組合 SQL 字串。

若圖片已由其他工具轉換成 32×32 RGBA 原始資料，可以先加入 `from pathlib import Path`，再用 `Path("sprite.rgba").read_bytes()` 取代範例中的純色 `image_bytes`。寫入前仍應檢查長度。Grid++ 遇到長度不等於 4096 bytes 的素材時會輸出警告並略過該筆資料。

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

素材名稱必須唯一。重複名稱會在載入時輸出警告，之後嘗試取得該名稱時丟出例外，因為引擎無法判斷應使用哪張圖片。尺寸或格式不符的資料會被略過並輸出警告。

## 素材與物件的關係

素材包只負責圖像，不保存物件位置、碰撞規則或遊戲狀態。同一素材可由多個 `GridObject` 共用，每個物件仍可設定不同的方向、顏色與 z-index。`ClearObjects()` 不會卸載素材，因此重新建立關卡時不必重複呼叫 `LoadAssets()`。

不需要圖片的遊戲可以省略 `LoadAssets()`，改用 `GridShapes.h` 直接呼叫 raylib 繪製圖形。
