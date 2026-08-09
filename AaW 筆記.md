# AaW 筆記

## 使用者需要做什麼、用到哪些函數
- 安裝、專案 template
- 跟 git 和部署 demo 有關的東西
- 素材包
- 介紹這個函式庫的三大元素：遊戲引擎物件、可以動的物件、疊在上面的UI
- 衍生的東西：素材包、迷宮模組
- GeneralGridObject -> 用 C 傳入 function pointer 的物件
    - 介紹用這個弄的遊戲
    - 範例1: 沒用到迷宮
- 繼承 BaseGridObject 來進行更細的遊戲設計
    - 範例2: pacman

========================
核心方向很清楚：這不該只是「函式庫文件」，而應該是一條刻意設計的學習路徑：

> 先用學生已知的 C 寫法做出遊戲 → 遇到狀態管理問題 → 引出 class、繼承、封裝與多型 → 最後完成 Pac-Man。

目前 [`mkdocs.yml`](/Users/aaronwu/codes/GridPlusPlusOrg/GridPlusPlus/mkdocs.yml:39) 是按功能分類的百科；[`getting-started.md`](/Users/aaronwu/codes/GridPlusPlusOrg/GridPlusPlus/docs/getting-started.md:58) 安裝完直接跳到完整 Pac-Man，接著又直接出現 class，跟預期教學坡道相反。

## 1. 命名先定稿

我不建議 `GeneralGridObject` 與 `BaseGridObject`：

| 現在／候選 | 建議 | 原因 |
|---|---|---|
| `GridEngine` | 保留 | 名稱清楚，就是遊戲引擎 |
| `GridObject` | 保留 | 已經表示「網格世界裡的物件」；不必加 `Base` |
| `EasyObject` | `CallbackGridObject` | 說明它真正的特色：用 callback 決定行為 |
| `GeneralGridObject` | 不使用 | `General` 聽起來反而像功能最完整的版本 |
| `BaseGridObject` | 不使用 | `Base` 描述繼承位置，不描述物件用途 |
| `UIElement` | 保留 | 清楚、也是常見術語 |
| `GridMaze` | 目前保留 | 若未來支援地板、陷阱等，再改成 `GridMap` |
| `onCollide` | `onCollision` | 英文事件名稱更自然 |
| `render` | `draw` | 與 `UIElement::draw()`、raylib 語彙一致 |
| `loadAssets` | `loadAssetPack` | 明確表示載入的是一個素材包 |
| `getCols` | `getColumns` | 教學 API 不需要省這幾個字 |
| `gridSize` | `cellSize` | 實際含義是單格的像素大小 |

建議在重寫文件前完成這次 breaking rename。現在專案還小，這是最後一次能低成本統一語彙的時間。

## 2. 建議的完整課程順序

Microsoft Learn 把課程組成有順序的 learning path，每個 module 再拆成短 units、練習與 knowledge check；Google Codelab 也要求先說明 prerequisites、what you’ll build，再逐步完成一個具體成果。[Microsoft Learn 結構](https://learn.microsoft.com/en-ie/training/support/learn-content-types)、[Google Codelab 寫作指南](https://developers.google.com/blockly/guides/contribute/samples/write_a_codelab)。

### 模組 0：開始之前

1. 你會做出什麼
   - 放可玩的最終 Pac-Man Web Demo。
   - 列出先備知識：變數、函式、陣列、struct、指標。
   - 說明尚未要求 class。

2. 安裝與取得專案
   - 安裝編譯器與 raylib。
   - 從 GitHub Classroom／template repository clone。
   - 編譯一個已完成的空白視窗。
   - 執行成功後做第一次 commit。

Git 只教這一課真的會用到的 `clone`、`status`、`add`、`commit`、`push`，不要先塞完整 Git 教科書。

### 模組 1：用 C 寫出第一個遊戲

3. 第一個網格世界
   - 建立 `GridEngine`。
   - 調整欄、列、格子大小。
   - `run()`。
   - 執行後看見空白網格。

4. 認識三大元素
   - `GridEngine`：管理視窗與遊戲循環。
   - `GridObject`：存在網格中的遊戲物件。
   - `UIElement`：使用像素座標、疊在遊戲上方。
   - 這一章只建立心智模型，不列完整 API 表。

5. Callback 物件
   - 引入 `CallbackGridObject`。
   - 寫 `movePlayer(GridObject* self)`。
   - 把 function pointer 傳進建構子。
   - 解釋 `self` 與日後 `this` 的關係。

6. 鍵盤與邊界
   - `IsKeyPressed`。
   - `getX/getY/move`。
   - `getEngine()`。
   - 每一步修改後立即編譯執行。

7. 碰撞與 tag
   - 建立一個金幣。
   - 玩家碰到金幣後，金幣移到隨機位置。
   - 使用 `onCollision`、`self`、`other`、`getTag()`。

8. UI 與分數
   - 加入 `Label`。
   - 撿到金幣後更新分數。
   - 完成第一個遊戲「Coin Collector」。

這就是你要的「沒有迷宮的範例 1」。它能在很少程式碼內用到三大元素、function pointer、碰撞和 UI，而且不需要每實例自訂狀態。

### 模組 2：為什麼需要物件導向

9. 故意讓程式遇到問題
   - 加入三個敵人。
   - 每個敵人需要自己的移動計時器。
   - 示範全域變數與 `static` 為什麼互相干擾。
   - 不用 `void*` 解決，讓問題自然留下。

10. 第一個 class
   - 把 `moveEnemy(self)` 搬進 `Enemy::onUpdate()`。
   - `Enemy : public GridObject`。
   - `override`。
   - `self->move()` 變成 `move()`。

11. 建構子與基底建構子
   - 將素材名稱、座標傳給 `GridObject`。
   - 建立多個 `Enemy`。
   - 說明同一個 class 可以建立很多 object。

12. 成員變數與封裝
   - `timer` 成為 `Enemy` 的 private 成員。
   - 每個敵人現在各自有一份。
   - 比較全域變數、區域變數、成員變數的生命週期。

13. 虛擬函式與多型
   - 不同物件都放進引擎。
   - 引擎只持有 `GridObject*`，卻能執行不同 `onUpdate()`。
   - 這時再正式命名「多型」，學生已經看過它運作。

### 模組 3：擴充遊戲世界

14. 素材與素材包
   - 先使用現成素材包。
   - 素材名稱、旋轉、tint。
   - 再教如何建立自己的素材包。

15. 文字地圖與迷宮
   - 用 `ifstream` 讀地圖。
   - `GridMaze`、`setWall()`、`isWall()`。
   - 牆壁自動拼接。
   - 這一章也能複習二維陣列與檔案輸入。

### 模組 4：Pac-Man 整合專題

不要維持目前一篇 228 行的大拆解，而是分成：

16. 載入地圖與建立迷宮  
17. 建立 Pellet 與碰撞  
18. 建立 Pacman 與方向緩衝  
19. 建立多隻 Ghost 與每實例狀態  
20. 加入分數、開始、暫停與結束畫面  
21. 重新開始與物件生命週期  
22. 小挑戰：新鬼魂行為、第二關或特殊豆子  

### 模組 5：Git 與部署

23. 整理 commit 與 push  
24. GitHub Actions 編譯 WebAssembly  
25. 部署 GitHub Pages  
26. 分享遊戲網址  

專案已經有 Emscripten 主循環與文件部署 workflow，可以讓 Actions 同時產生 `site/demos/...`，在教材頁面用 iframe 嵌入可玩的版本。不需要自己做線上 C++ 編輯器。

## 3. 每一課固定使用同一格式

每課控制在 15–30 分鐘：

1. 這一課會完成什麼。
2. 先備知識。
3. 可玩的結果或截圖。
4. 從哪份 starter code 開始。
5. 3–6 個編號操作步驟。
6. 每個步驟後執行並顯示預期結果。
7. 解釋剛才實際用到的新觀念。
8. 兩題知識檢查。
9. 一個小挑戰與可展開的答案。
10. 下一課會遇到什麼問題。

Google 的程序文件也建議使用明確的編號步驟、提供最短的推薦路徑，而不是一次列出很多選項；可複製的程式碼應該直接可執行，不要充滿 `...`。[程序寫作指南](https://developers.google.com/style/procedures)、[程式碼範例指南](https://developers.google.com/style/code-samples)。

MkDocs Material 現有的 copy button、tabs、admonition 已經足夠。再加：

- 每課開頭的 WebAssembly demo。
- 「預期畫面」截圖或短 GIF。
- task list 顯示進度。
- 可展開的提示與答案。
- Windows／macOS／Linux 指令 tabs。

## 4. 為什麼現在的文件看起來「太 AI」

主要不是文筆，而是文件類型混在一起：

- 教學、概念說明、API reference、內部原理同時出現在主導覽。
- 幾乎每頁都有「特色、注意、重點觀念、API 一覽」，但學生沒有一直動手。
- 說明完整成品，而不是逐步修改程式。
- 同一份 Pac-Man 文件在 [`docs/tutorial/pacman.md`](/Users/aaronwu/codes/GridPlusPlusOrg/GridPlusPlus/docs/tutorial/pacman.md:1) 和 [`examples/pacman/README.md`](/Users/aaronwu/codes/GridPlusPlusOrg/GridPlusPlus/examples/pacman/README.md:1) 幾乎逐字重複。
- 太常使用粗體、破折號、「你只要」、以及總結式段落。
- SQLite B-tree 內部原理對學生不是主要學習路徑，應移到「貢獻者文件」。

保留一份 canonical tutorial；範例資料夾的 README 只留下玩法、編譯指令與教學連結。

## 5. 實作改善優先順序

### 必須先修，否則會妨礙教材

1. 正式的物件移除 API

目前豆子靠移到 `(-1,-1)` 假裝消失，[實際上仍會更新、繪製並參與碰撞](/Users/aaronwu/codes/GridPlusPlusOrg/GridPlusPlus/examples/pacman_easy/main.cpp:66)。應加入安全的 deferred `destroy(object)`，在一幀結束後才真正刪除。

2. 明確的所有權與清理

[`GridEngine` destructor](/Users/aaronwu/codes/GridPlusPlusOrg/GridPlusPlus/GridPlusPlus.h:183) 只關閉視窗，沒有刪除 objects、UI 或 GPU textures。`spawn(new ...)` 看起來代表 engine 接管所有權，就應該完整清理並寫進 API 文件。

3. 可真正使用的素材流程

[`assets.md`](/Users/aaronwu/codes/GridPlusPlusOrg/GridPlusPlus/docs/guide/assets.md:31) 中的 `image_bytes` 沒有產生方式，學生照做無法完成素材包。

最簡方案：

- 初期提供 `loadSprite(name, pngPath)`，直接使用 raylib 已有的 PNG 載入。
- 後期部署章才介紹把多張圖打成 asset pack。
- 若保留 packer，就必須提供一支真的可執行的工具；目前 [`.gitignore`](/Users/aaronwu/codes/GridPlusPlusOrg/GridPlusPlus/.gitignore:21) 甚至忽略所有 `.py`，會擋住 Python 素材工具。

4. 無素材的彩色格子

讓空素材名稱直接畫 `tint` 色塊；只有「指定了不存在的素材名稱」才畫紅色錯誤方塊。如此第一個遊戲不需要先理解素材包，也能區分玩家、金幣與敵人。

5. 輸入錯誤要有初學者看得懂的訊息

Pac-Man 直接讀 `map.txt`，沒有檢查檔案是否存在或內容是否完整，[讀取失敗時 rows/cols 可能未初始化](/Users/aaronwu/codes/GridPlusPlusOrg/GridPlusPlus/examples/pacman/main.cpp:211)。這會產生很難理解的錯誤，應在邊界明確報錯。

6. 修正 reference 與實作不一致

API 文件列出了不存在的 [`setWallAsset(mask, name)`](/Users/aaronwu/codes/GridPlusPlusOrg/GridPlusPlus/docs/reference/api.md:127)。重寫前先讓 reference 可由公開 header 核對。

### 第二順位

- `GridMaze` 對超過 64×64 的尺寸目前靜默截斷；對教材應明確報錯。
- 將 `gridX/gridY/engine` 改為 private，範例統一使用 getter；目前 [`template.cpp`](/Users/aaronwu/codes/GridPlusPlusOrg/GridPlusPlus/template.cpp:18) 直接使用 protected `engine`，與封裝教學衝突。
- CI 除了部署文件，也應編譯測試、編譯所有範例並執行 `mkdocs build --strict`；目前 workflow 只做普通 [`mkdocs build`](/Users/aaronwu/codes/GridPlusPlusOrg/GridPlusPlus/.github/workflows/docs.yml:22)。
- 為物件移除、碰撞與迷宮連通加入少量測試。

### 目前不要加

- `void* userdata`
- ECS
- scene manager
- pathfinding
- 物理系統
- 自製線上程式碼編輯器
- 萬用 property bag

它們不直接服務「從 C 過渡到 OOP」這條課程主線。

最合理的執行順序是：先定 API 名稱 → 修物件生命週期與素材流程 → 做 Coin Collector → 用它重寫前半段課程 → 最後把 Pac-Man 拆成逐步專題。SQLite test 已在本機以 C++11 編譯並通過；目前環境沒有 MkDocs 與 raylib，因此這次未完成文件與圖形範例的本機建置。