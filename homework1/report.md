# 41143109

作業一
## problem1 Ackermann's Function
## 解題說明

### 問題一：Ackermann 函式
Ackermann 函式是著名的 非原始遞迴（non‑primitive‑recursive）函式，其數學定義為
```
A(m, n) = {
    n + 1                     , m = 0
    A(m‑1, 1)                 , m > 0 且 n = 0
    A(m‑1, A(m, n‑1))         , m > 0 且 n > 0
}

```
- 特性:由於增長速度極快，只接受小範圍測試（如 m≤3、n≤4）才不會因堆疊或記憶體耗盡而失敗。
- 本作業要求:
1. 實作 遞迴版（直接對照數學定義）。
2. 實作 迭代版，以手寫 堆疊 模擬遞迴，避免系統呼叫堆疊的深度限制。

### 解題策略

1. 遞迴實作:直接映射定義，最簡潔的解法。
2. 迭代實作（手寫堆疊）:避免系統呼叫堆疊深度限制，讓程式自行管理深度。

## 程式實作

以下為主要程式碼：
1. 遞迴版
```cpp
#include <iostream>
using namespace std;

   
long long ackermannRec(int m, int n)
{
    if (m == 0) return n + 1;
    if (n == 0) return ackermannRec(m - 1, 1);
    return ackermannRec(m - 1,
        ackermannRec(m, n - 1));
}
```
2. 迭代版 – 手寫堆疊模擬遞迴
```cpp
#include <iostream>
using namespace std;


long long ackermannItr(int m, int n)
{
    const int MAX_STK = 100000;         
    int *stk = new int[MAX_STK];        
    int top = -1;                        

    while (true)
    {
        if (m == 0)                      // 基本情形 A(0,n) = n+1
        {
            n = n + 1;
            if (top < 0) break;          
            m = stk[top--];              // 彈回上一層的 m
        }
        else if (n == 0)                 // A(m,0) = A(m-1,1)
        {
            n = 1;
            m = m - 1;
        }
        else                             // A(m,n) = A(m-1, A(m,n-1))
        {
            stk[++top] = m - 1;           // 把外層的 m-1 暫存
            n = n - 1;                    // 先算內層 A(m,n-1)
            // m 保持不變，下一輪迴圈會再次判斷
        }
    }

    delete[] stk;
    return n;                            // 最終的 n 即 A(m0,n0)
}

```
## 效能分析


時間複雜度:當 m = 0 時， $O(1)$ 。  
當 m = 1 時， $O(n)$ 。  
當 m = 2 時，  $O(2n + 3)$ 。  
當 m = 3 時， $O(2^{n+3})$ 。  
當 m = 4 時， $O(2^{2^{n}})$ 。  
這正是 Ackermann 的遞迴定義。每一次展開都會產生 一個 直接的遞迴呼叫與 一個 內層的 A(m,n‑1)。
因此遞迴樹的節點數 恰好等於 A(m,n) 本身的值（每個節點代表一次基本的 +1 操作）。

因此整體時間複雜度可表為 $T(m,n)$ = $O( A(m,n) )$    

空間複雜度:遞迴版空間複雜度為  $O(m + n)$ 。非遞迴版空間複雜度為 $O(m + n)$ 。

## 測試與驗證

### 測試案例

| 測試案例 | 輸入參數 (m, n) | 預期輸出 | 實際輸出 |
|----------|-----------|---------|-----------|
| 測試一   | (2,3)     | 9        | 9        |
| 測試二   | (3,2)     | 29       | 29       |
| 測試三   | (3,4)     | 125      | 125      |
| 測試四   | (4,1)     | 溢位     | 程式自行報錯     |

### 編譯與執行指令（Visual Studio）

```shell
$ cl /EHsc /std:c++17 ackermann.cpp /Fe:ackermann.exe
$ .\ackermann.exe
請輸入 m 與 n（以空白分隔）: 3 6
遞迴版  A(3,6) = 509
迭代版  A(3,6) = 509

```

### 結論

1. 功能正確
- 程式能正確計算 Ackermann 函式 A(m,n)，無論是 遞迴版 還是 非遞迴（手寫堆疊）版，在所有測試資料中兩者輸出完全一致。
2. 測試覆蓋
- 基本情形：m = 0 → $A(0,n)$ = n+1
- 遞迴呼叫：m > 0 且 n = 0 → $A(m,0)$ = $A(m‑1,1)$
- 深層遞迴：m > 0 且 n > 0 → $A(m,n)$ = $A(m‑1, A(m,n‑1))$
- 以上三種情況皆在程式中得到正確處理
 
3. 效能與限制
- Ackermann 的增長速度 遠超多項式、指數，屬於超指數級。
- 空間上界為 O(m+n)，兩種實作皆相同；手寫堆疊版允許自行設定 MAX_STK，可在需要時調整上限。
- 當 m ≥ 4 時，即使是手寫堆疊版也會因為 呼叫次數天文 而耗盡記憶體或執行時間（如 $A(4,1)$ ≈ 2^65536），所以本作業只在 小範圍（m ≤ 3、n ≤ 6）內測試。

## 申論及開發報告

### 研究目的與實作方式

本次作業的核心是 Ackermann 函式 $A(m,n)$，其遞迴定義在題目中已說明。
我依序實作了兩個版本：

1. 遞迴版:以最直接的方式呈現 Ackermann 的數學定義，程式碼最短，便於教學。
   當 $n$ 等於 1 或 0 時，直接返回結果，結束遞迴。

2. 非遞迴版:利用手寫堆疊取代系統呼叫堆疊，解決 Stack overflow 的問題，讓程式在較深的遞迴深度下仍能正確執行。

兩種實作在 功能上完全等價，皆能正確計算 A(m,n) 並在所有測試資料中產生一致的輸出。
   


## 測試覆蓋與驗證
   
| 項目 | 說明 |
|-------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------|
| 時間複雜度   | Ackermann 的增長速度 遠超任何多項式或指數，理論上屬於 超指數級 (Θ(A(m,n)))。不管是遞迴版還是手寫堆疊版，都必須執行與 A(m,n) 等量的基本運算，無法再降低時間上界。     |
| 空間複雜度   | 兩個版本的最大堆疊深度均為 $O(m + n)$（即遞迴樹的最大高度）。遞迴版依賴系統呼叫堆疊，非遞迴版則自行以陣列管理此深度，兩者在 小範圍 (m ≤ 3、n ≤ 6) 內實際佔用的記憶體僅為數十個 int，可視為常數。     |
| 實務限制     | 當 m ≥ 4 時，即使採用手寫堆疊，也會因 呼叫次數天文（例如 A(4,1) ≈ 2^65536）而耗盡記憶體或執行時間。故本作業只在 m ≤ 3、n ≤ 6 的小範圍內測試，已足以驗證演算法正確性。     |


## 開發心得

1. 遞迴 vs. 手寫堆疊
- 遞迴版最易寫、最能表現概念；手寫堆疊則讓我體會「把遞迴抽象成資料結構」的轉換，學會自行管理堆疊深度。
2. 未來可改進
- 若要處理更大的 m、n，可加入 記憶化（Memoization） 或 動態矩陣，減少重複子問題的計算次數。
- 亦可將手寫堆疊的容量改為動態調整（使用 std::vector），避免固定 MAX_STK 可能的上限問題。

### 結語

本次作業成功展示了 Ackermann 函式 在 遞迴 與 手寫堆疊 兩種不同實作方式下的等價性與正確性，並透過 功能測試、效能分析、限制說明 完整闡述了演算法的特性。
雖然 Ackermann 本身在實務上僅作為 遞迴概念的極限範例，但透過本次實作，我更加熟悉了 遞迴 → 堆疊的思考轉換，以及管理 程式記憶體 與 呼叫深度 的技巧


作業二
## Power‑Set 生成
## 解題說明

### 問題二：Power Set 生成

給定一個大小為 n 的集合 S = {s₀ , s₁ , … , sₙ₋₁}，要求列舉出 S 的全部子集合（即 Power‑Set）。

```
S = {a, b, c}
Power‑Set(S) = { {}, {a}, {b}, {c},
                {a,b}, {a,c}, {b,c},
                {a,b,c} }
```
遞迴實作：以「是否選擇」的 0/1 決策方式（Decrease‑and‑Conquer）寫出 generatePowerSet。

### 解題策略

| 步驟 | 目的 |實作方式|
|-------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------|
| 決策樹建構   | 每個元素 sᵢ 必須決定 「不選」(0) 或 「選」(1)。     |在遞迴 generatePowerSet(arr, used, n, idx) 中，idx 表示當前正在決策第 idx 個元素。|
| 0/1 記錄   | 用布林陣列 used[i] 保存本輪的選擇。     |used[i]=false → 不取； used[i]=true → 取。|
| 葉節點輸出     | 當所有 n 個決策完成 (idx == n) 時，把 used 轉成子集合字串印出。     |printSubset 逐一檢查 used[i]，把被選的 arr[i] 放入 { … }。|
| 葉節點輸出     | 兩條分支（不選 / 選）在每層遞迴都執行，形成深度為 n、寬度為 2 的完整二叉遞迴樹，葉子數正好是 2ⁿ。   |只要遞迴呼叫一次 generatePowerSet(arr, used, n, 0) 即可。|

## 程式實作
Power‑Set 

```cpp
#include <iostream>
#include <string>
using namespace std;

constexpr int MAX_N = 64;          

void printSubset(const string arr[], const bool used[], int n)
{
    cout << "{ ";
    bool first = true;
    for (int i = 0; i < n; ++i) {
        if (used[i]) {
            if (!first) cout << ", ";
            cout << arr[i];
            first = false;
        }
    }
    cout << " }" << endl;
}

void generatePowerSet(const string arr[], bool used[], int n, int idx)
{
    if (idx == n) {                 
        printSubset(arr, used, n);
        return;
    }

    /* 不選第 idx 個元素 */
    used[idx] = false;
    generatePowerSet(arr, used, n, idx + 1);

    /* 選第 idx 個元素 */
    used[idx] = true;
    generatePowerSet(arr, used, n, idx + 1);
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cout << "請輸入集合的元素個數 (0~" << MAX_N << "): ";
    if (!(cin >> n) || n < 0 || n > MAX_N) {
        cerr << "錯誤：請輸入合法的非負整數且不超過上限。" << endl;
        return -1;
    }

    string arr[MAX_N];
    bool   used[MAX_N] = { false };    

    cout << "請依序輸入 " << n << " 個元素（以空白或換行分隔）:" << '\n';
    for (int i = 0; i < n; ++i) {
        if (!(cin >> arr[i])) {
            cerr << "讀取元素失敗。" << endl;
            return -1;
        }
    }

    cout << "\n=== Power Set (遞迴 0/1 決策) ===\n";
    generatePowerSet(arr, used, n, 0);
    return 0;
}
```
## 效能分析

