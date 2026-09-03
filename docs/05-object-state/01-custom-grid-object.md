# 自訂 GridObject

遊戲狀態是會隨執行過程改變，而且之後仍需要讀取的資料。分數屬於整局遊戲的狀態；位置是每個 GridObject 已經內建的狀態；敵人的生命值、移動間隔和追蹤目標則可能只屬於某一個 instance。

第 4 章使用的普通函式不會替每個物件建立一份區域資料；兩個物件使用同一個函式時，函式內容相同，其中讀寫的全域或 static 變數也只有一份，因此只要每個 instance 需要記住不同資訊，共用變數便無法直接表達資料究竟屬於誰。

C++ class 可以把資料與使用資料的行為放在一起。建立一個繼承 `GridObject` 的 class 後，每次建立 instance 都會得到自己的一組成員變數；覆寫的生命週期函式則可以直接讀寫這些變數。這種做法在 GridObject 的共通能力上增加特定遊戲需要的狀態，並繼續使用原有的 Grid++ API。

## 讓多隻地鼠保存自己的時間

單一地鼠可以使用全域 `next_move`，可是同時 spawn 三隻地鼠時，三個物件的 `UpdateFn` 都會讀寫同一個變數，第一隻更新時間後，另外兩隻也會被迫等待；將時間改成 `Mole` 的成員變數後，每個 instance 才會擁有獨立的出現週期。

```cpp
class Mole : public GridObject {
public:
    Mole(int x, int y, double interval)
        : GridObject("mole", x, y),
          interval_(interval),
          next_move_(0.0) {
        set_tag("mole");
    }

    void OnUpdate() override {
        if (GetTime() < next_move_) return;

        set_x(GetRandomValue(0, engine()->cols() - 1));
        set_y(GetRandomValue(0, engine()->rows() - 1));
        next_move_ = GetTime() + interval_;
    }

private:
    double interval_;
    double next_move_;
};
```

`interval_` 和 `next_move_` 都屬於單一 `Mole`，而建構子參數還能讓每隻地鼠使用不同速度，不需要為相同行為建立多個只因數值不同而換名字的函式。

```cpp
game.Spawn(new Mole(1, 1, 1.0));
game.Spawn(new Mole(3, 3, 0.8));
game.Spawn(new Mole(6, 5, 1.4));
```

這項修改沒有改變 Engine 看待物件的方式，因為 `class Mole : public GridObject` 表示 `Mole` 除了新增自己的資料與行為之外，仍然是一種 `GridObject`，所以座標、素材、顯示、碰撞與 z-index 等共通能力全部保留。真正改變的是資料的歸屬：`interval_` 和 `next_move_` 不再漂浮於物件之外，而是每次 `new Mole(...)` 都會隨著新地鼠產生一份，直到該物件由 Engine 移除為止。

## 建構子先建立物件本身

建構子負責建立物件本身，不應假設物件已經屬於某個引擎。此時 `engine()` 仍是 `nullptr`。需要讀取網格尺寸、生成其他物件或執行依賴引擎的初始化時，覆寫 `OnSpawn()`。

```cpp
class Enemy : public GridObject {
public:
    Enemy(int x, int y) : GridObject("enemy", x, y) {}

    void OnSpawn() override {
        set_x(engine()->cols() - 1);
    }
};
```

這項區分讓類別不會暗中依賴尚未存在的 Engine：建構子先完成物件自身的有效狀態，等 `Spawn()` 把它加入世界後，`OnSpawn()` 才處理必須查詢網格大小的工作。至於遊戲執行期間建立物件時，`OnSpawn()` 究竟在哪一幀發生，會留到第 8 章配合完整流程說明，以免生命週期規則分散在多個章節而互相重複。

`OnUpdate()` 只是自訂物件可以覆寫的生命週期方法之一；下一節會依照物件加入世界、持續更新、發生碰撞與畫上畫面的順序，說明其餘方法各自應該承擔什麼責任，而不在此重複列出零散規則。

## 建立與借用物件

自訂物件仍透過 `Spawn()` 加入引擎。`Spawn(GridObject*)` 的回傳型別是 `GridObject*`；需要呼叫衍生類別特有函式時，可先保留原本的衍生類別指標。

```cpp
Ghost* ghost = new Ghost(3, 4, RED);
game.Spawn(ghost);

// ghost 是借用指標，所有權已轉交給 game。
```

不要在 `Spawn()` 後自行釋放 `ghost`。如果只使用 `GridObject` 的共通操作，可以直接寫成 `GridObject* ghost = game.Spawn(new Ghost(...));`，降低程式對實際類別的依賴。

## 回頭理解 callback 與繼承

現在可以替第 4 章的機制補上一個常見名稱：把函式先交給系統，等指定事件發生時再由系統呼叫，這種函式稱為 callback，而 Grid++ 內部使用 `CallbackGridObject` 保存 `UpdateFn` 與 `CollideFn`。這個名稱描述的是呼叫方式，並不表示它比較低階或只能暫時使用；callback 與自訂類別使用相同的 Engine、碰撞與繪製機制，真正的選擇標準是資料究竟屬於整局遊戲、`GridObject` 已有狀態，還是某一個物件獨有的額外狀態。

平常使用函式式 `Spawn()` 時不必直接操作 `CallbackGridObject`，因為 Engine 會替我們建立並管理它；需要查閱內部建構介面時，再參考 [CallbackGridObject API](../api/classgridpp_1_1_callback_grid_object.md) 即可。

| 情況 | 建議方式 |
|---|---|
| 物件只需要位置、素材、tag 等內建狀態 | `UpdateFn`／`CollideFn` callback |
| 所有物件共用一份遊戲狀態 | `UpdateFn`／`CollideFn` callback |
| 每個 instance 需要不同的計時器或方向 | 自訂 `GridObject` |
| 行為需要多個互相配合的函式 | 自訂 `GridObject` |
| 需要自訂繪製 | 自訂 `GridObject` |

完整 Pacman 範例中的 `Ghost` 各自保存移動時間、顏色和行為模式，`Player` 則保存目前方向與下一個想轉向的方向，因此兩者都會採用繼承；豆子的碰撞若只需增加共用分數並移除自己，則仍可使用 `CollideFn`。同一個遊戲同時使用兩種方式並不矛盾，反而表示程式依照資料歸屬選擇了最直接的表達。
