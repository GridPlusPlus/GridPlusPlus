# 使用 raylib 讀取輸入與時間

Grid++ 負責管理網格世界、物件與每幀流程，鍵盤、滑鼠和時間則直接交給底層的 raylib。只要程式已經引入 `GridPlusPlus.h`，就能使用本節介紹的 raylib 函式，不必再加入其他標頭；它們通常寫在 `UpdateFn` 中，讓 Engine 每幀詢問一次目前的輸入與時間，再依結果修改遊戲狀態。

這裡不會列出 raylib 的所有功能，而是先處理接下來幾章會反覆遇到的四項需求：辨認按鍵是剛按下還是仍被按住、把滑鼠位置換成網格座標、判斷一段時間是否已經經過，以及取得有效範圍內的隨機整數。理解這些差異後，後面的範例便不必把函式名稱當成需要照抄的咒語，而能從遊戲規則判斷該使用哪一個。

## 按一次與持續按住

鍵盤輸入最常見的差別，在於遊戲要回應「按下去的那一刻」，還是按鍵維持按住的整段時間。`IsKeyPressed(KEY_R)` 只會在 R 鍵由放開變成按下的那一幀回傳 `true`，很適合重新開始、確認選項或一次移動一格；即使玩家稍微按住按鍵，這項操作也只會執行一次。

`IsKeyDown(KEY_RIGHT)` 則會在右方向鍵保持按住的每一幀回傳 `true`，因此適合持續移動，或像第 7 章的 Pacman 一樣持續記住玩家想轉向的方向。`KEY_R`、`KEY_RIGHT`、`KEY_UP` 等名稱是 raylib 提供的按鍵常數，換成不同常數便能檢查不同按鍵。

```cpp
if (IsKeyPressed(KEY_R)) {
    ResetGame(mole);
}

if (IsKeyDown(KEY_RIGHT)) {
    wanted_direction = 0;
}
```

選擇函式時，不要只看操作使用哪一顆鍵，而要先決定規則允許它觸發幾次。重新開始若使用 `IsKeyDown()`，玩家按住 R 的幾幀內就可能反覆重設；連續移動若使用 `IsKeyPressed()`，角色則只會在按下瞬間收到一次輸入。

## 從滑鼠像素找到網格

`GetMousePosition()` 會回傳滑鼠在視窗中的像素座標，結果保存為含有 `x`、`y` 的 `Vector2`；`IsMouseButtonPressed(MOUSE_BUTTON_LEFT)` 則用來判斷滑鼠左鍵是否剛被按下。兩者合在一起，可以知道玩家這一幀是否點了視窗中的某個位置，但還不能直接與 GridObject 的網格座標比較。

若每格寬高都是 `grid_size` 像素，只要將滑鼠的 x、y 各自除以格子大小，就能得到滑鼠所在的欄與列。例如像素位置 `(220, 150)` 在 64 像素的格子中會落於網格 `(3, 2)`，因為整數除法會捨去不足一格的餘數。

```cpp
const Vector2 mouse = GetMousePosition();
const int grid_size = mole->engine()->grid_size();
const int mouse_x = static_cast<int>(mouse.x) / grid_size;
const int mouse_y = static_cast<int>(mouse.y) / grid_size;

if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
    mouse_x == mole->x() && mouse_y == mole->y()) {
    ++score;
}
```

這段判斷先確認左鍵剛被按下，再比較滑鼠與地鼠是否位於同一格，因此按住滑鼠不會在每一幀重複得分。若遊戲還允許物件出現在網格以外，或視窗周圍另有介面區域，換算後也應先用 `cols()` 與 `rows()` 檢查座標是否仍在有效範圍內。

## 判斷經過時間

遊戲不應用「已經更新幾幀」猜測經過多久，因為不同電腦與不同負載下，每秒實際完成的幀數可能不同。raylib 提供兩種常用的時間讀法，其中 `GetTime()` 回傳視窗建立後經過的秒數，適合保存下一次行動的絕對時刻：

```cpp
double next_move = 0.0;

if (GetTime() >= next_move) {
    MoveMole(mole);
    next_move = GetTime() + 1.0;
}
```

第一次執行時，`next_move` 為 0，所以地鼠會立刻移動；移動後再把下一次時刻設為目前時間加一秒，接下來的更新便會等待這個時刻到來。倒數計時也可以保存結束時刻，再以「結束時刻減去 `GetTime()`」算出剩餘秒數。

另一種函式 `GetFrameTime()` 回傳上一幀花費的秒數，適合把每幀經過的一小段時間累加起來，例如控制平滑移動或持續變化的動畫：

```cpp
elapsed += GetFrameTime();
```

兩者並不是不同精確度的計時器，而是兩種描述規則的方式。若問題是「現在是否已到下一次移動的時刻」，使用 `GetTime()` 直接比較最清楚；若問題是「這一幀應該前進多少」，便使用 `GetFrameTime()` 取得這一幀實際經過的時間。

## 取得範圍內的隨機整數

`GetRandomValue(min, max)` 會從指定範圍隨機取出一個整數，而且最小值與最大值都可能出現。8 欄網格的 x 座標從 0 到 7，因此最大值必須寫成 `cols() - 1`；若直接把 `cols()` 當成上限，物件偶爾就會得到網格外的座標。

```cpp
mole->set_x(GetRandomValue(0, mole->engine()->cols() - 1));
mole->set_y(GetRandomValue(0, mole->engine()->rows() - 1));
```

同一原則也適用於陣列索引與其他零起算的範圍：先確認有效值的首尾，再把那兩個值交給 `GetRandomValue()`，不要因為把上限誤認成數量而多出一格。

## 讓檢查發生在每幀更新中

鍵盤與滑鼠函式描述的是某一幀的輸入狀態，時間判斷也必須隨遊戲推進而反覆檢查，所以這些程式通常放在 `UpdateFn`，之後改用自訂物件時則放在 `OnUpdate()`。`main()` 仍負責建立 Engine、加入物件並完成初始設定；呼叫 `Run()` 後，Engine 才會在視窗開啟期間反覆執行更新，而 `Run()` 後面的程式要等到視窗關閉才會繼續。

接下來的打地鼠會把這些函式放進同一個 `UpdateFn`：以 `GetTime()` 安排地鼠換位，以 `GetRandomValue()` 選擇新座標，以 `GetMousePosition()` 和滑鼠按鍵判斷是否命中，最後再用鍵盤輸入重設遊戲。每個函式只回答一個問題，遊戲規則則由程式決定如何組合這些答案。

[製作打地鼠](../02-whack-a-mole/index.md){ .md-button .md-button--primary }
