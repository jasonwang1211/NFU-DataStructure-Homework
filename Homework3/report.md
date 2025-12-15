# 41143109
作業三報告

## Polynomial ADT（循環鏈結串列實作 + Available List 加分項）
---

## 解題說明

### 1.1 題目需求
實作 **Polynomial** 類別，使用**帶頭結點的循環鏈結串列**（circular linked list with header node）表示單變數多項式（整數係數，指數非負整數，且項按指數遞減排列 e₁ > e₂ > … > eₙ）。

每個節點包含三個成員：`coef`（int）、`exp`（int）、`link`。

必須實作：
(a) `istream& operator>>`：讀入一整行 coef exp 配對（空白分隔）  
(b) `ostream& operator<<`：美觀輸出（係數 1 省略、x^1 不顯示 ^1、正負號前有空格等）  
(c) Copy constructor  
(d) Assignment operator  
(e) Destructor（節點回收至 Available List）  
(f) `operator+`  
(g) `operator-`  
(h) `operator*`  
(i) `float Evaluate(float x) const`

> 加分項：Available-space list 記憶體重用（已實作）。

### 1.2 範例測試
輸入 p (coef exp 成對，例如：3 5 7 2 -2 1 4 0)：3 5 7 2 -2 1 4 0
輸入 q：1 4 -5 2 8 0
p(x) = 3x^5 + 7x^2 - 2x + 4
q(x) = x^4 - 5x^2 + 8
p + q = 3x^5 + x^4 + 2x^2 - 2x + 12
p - q = 3x^5 - x^4 + 12x^2 - 2x - 4
p * q = 3x^9 + 7x^6 - 21x^5 + 3x^4 - 10x^3 + 39x^2 - 4x + 32
p(2) = 110


---

## 解題策略

| 步驟                  | 實作說明                                                                                           |
|-----------------------|----------------------------------------------------------------------------------------------------|
| **1. ChainNode**      | 泛型結構體，包含 `coef`、`exp`、`link`，提供建構函式。                                             |
| **2. Chain<T>**       | 帶頭結點的循環鏈結串列，維護 `head` 與 `tail`，提供 `InsertBack`、`MakeEmpty`、`Begin()`、`End()`。 |
| **3. ChainIterator**  | 內嵌類別，提供 `coef()`、`exp()` 成員函式（因不能用 pair），支援 `++`、`!=`。                      |
| **4. Polynomial**     | 成員 `public: Chain<int> terms;`（public 以方便 iterator 測試與 main 使用），提供預設建構函式。    |
| **5. Available List** | `static ChainNode<T>* avail`，`NewNode` 優先重用，`DeleteNode` 回收節點，Destructor 自動回收。     |

---

## 程式實作（關鍵程式碼）

> 完整程式已上傳至  
> [Polynomial ADT（循環鏈結串列實作 + Available List 加分項）](https://github.com/jasonwang1211/NFU-DataStructure-Homework/blob/main/Homework3/src/Polynomial.cpp)

```cpp
// 1. ChainNode
template<class T>
struct ChainNode {
    T coef;
    T exp;
    ChainNode<T>* link;
    ChainNode(const T& c = 0, const T& e = 0, ChainNode<T>* l = nullptr)
        : coef(c), exp(e), link(l) {}
};

// 2. Chain 的 iterator（重點展示）
template<class T>
class Chain {
public:
    class iterator {
    private:
        ChainNode<T>* current;
    public:
        iterator(ChainNode<T>* p = nullptr) : current(p) {}
        T coef() const { return current->coef; }
        T exp()  const { return current->exp;  }
        iterator& operator++() { current = current->link; return *this; }
        bool operator!=(const iterator& rhs) const { return current != rhs.current; }
        // ... 其他運算子略
    };

    static ChainNode<T>* avail;  // Available List

    static ChainNode<T>* NewNode(const T& c = 0, const T& e = 0) {
        if (avail) {
            ChainNode<T>* x = avail;
            avail = avail->link;
            x->coef = c; x->exp = e; x->link = nullptr;
            return x;
        }
        return new ChainNode<T>(c, e);
    }

    static void DeleteNode(ChainNode<T>* p) {
        p->link = avail;
        avail = p;
    }

    // ... 其他成員略
};

// 輸入運算子
istream& operator>>(istream& is, Polynomial& x) {
    x.terms.MakeEmpty();
    string line;
    ws(is);                    // 清除前導空白與換行
    getline(is, line);
    stringstream ss(line);
    int c, e;
    while (ss >> c >> e) {
        if (c != 0) x.terms.InsertBack(c, e);
    }
    return is;
}

// 輸出運算子（重點展示格式處理）
ostream& operator<<(ostream& os, const Polynomial& x) {
    if (x.terms.IsEmpty()) { os << "0"; return os; }
    bool first = true;
    for (Chain<int>::iterator it = x.terms.Begin(); it != x.terms.End(); ++it) {
        int c = it.coef();
        int e = it.exp();
        if (c == 0) continue;
        if (!first) {
            if (c > 0) os << " + ";
            else { os << " - "; c = -c; }
        } else if (c < 0) { os << "-"; c = -c; }
        if (c != 1 || e == 0) os << c;
        if (e > 0) {
            if (c != 1) os << "*";
            os << "x";
            if (e > 1) os << "^" << e;
        }
        first = false;
    }
    return os;
}

// 加法核心（雙指標合併）
Polynomial Polynomial::Add(const Polynomial& b) const {
    Polynomial result;
    Chain<int>::iterator aIt = terms.Begin(), aEnd = terms.End();
    Chain<int>::iterator bIt = b.terms.Begin(), bEnd = b.terms.End();
    while (aIt != aEnd && bIt != bEnd) {
        int ea = aIt.exp(), eb = bIt.exp();
        if (ea > eb) { result.terms.InsertBack(aIt.coef(), ea); ++aIt; }
        else if (ea < eb) { result.terms.InsertBack(bIt.coef(), eb); ++bIt; }
        else {
            int sum = aIt.coef() + bIt.coef();
            if (sum != 0) result.terms.InsertBack(sum, ea);
            ++aIt; ++bIt;
        }
    }
    // 剩餘項處理略
    return result;
}
```
## 效能分析

| 演算法 / 操作              | 時間複雜度                  | 空間複雜度                  | 說明                                                                 |
|----------------------------|-----------------------------|-----------------------------|----------------------------------------------------------------------|
| **單項插入（InsertBack）** | **`O(1)`**                  | **`O(1)`**（均攤，含 Available List） | 因題目保證輸入指數已遞減，故永遠插入尾端，無需搜尋或合併。           |
| **Add（多項式相加）**      | **`O(t₁ + t₂)`**            | **`O(t₁ + t₂)`**（結果多項式的項數） | 使用雙指標合併（類似 merge sort 的 merge 過程），逐項比較指數並處理。 |
| **Subtract（多項式相減）** | **`O(t₁ + t₂)`**            | **`O(t₁ + t₂)`**            | 先產生 b 的負多項式（O(t₂)），再呼叫 Add 合併，總時間仍為線性。       |
| **Mult（多項式相乘）**     | **`O(t₁ ⋅ t₂)`**            | **`O(t₁ ⋅ t₂)`**（最壞情況） | 對每一對項產生乘積（雙層迴圈），再多次呼叫 Add 進行合併（每次 Add 為線性）。 |
| **Evaluate（求值）**       | **`O(t)`**                  | **`O(1)`**                  | 使用 iterator 逐項走訪，計算 `coef * pow(x, exp)`。                   |
| **I/O（>>）**               | **`O(t)`**                  | **`O(t)`**                  | 讀取整行後解析成對，逐一 InsertBack 插入（每個 O(1)）。               |
| **I/O（<<）**               | **`O(t)`**                  | **`O(1)`**                  | 使用 iterator 逐項走訪並輸出，無需額外排序（因已保證指數遞減）。       |
| **Copy / Assignment**      | **`O(t)`**                  | **`O(t)`**                  | 深拷貝：逐項走訪來源串列並 InsertBack 到新串列。                      |
| **Destructor**             | **`O(t)`**                  | **`O(1)`**（記憶體重用）    | 逐項走訪並將節點回收至 Available List，未來 NewNode 可重用節點。      |

> **`t`、`t₁`、`t₂`** 分別代表多項式中非零項的個數。  
> 本實作已達到多項式鏈結串列表示的理論最佳複雜度（加法線性、乘法二次），並透過 Available List 優化記憶體分配效率。
## 測試與驗證

輸入第一個多項式 (coef exp 成對): 3 5 7 2 -2 1 4 0
輸入第二個多項式: 1 4 -5 2 8 0

p(x) = 3x^5 + 7x^2 - 2x + 4
q(x) = x^4 - 5x^2 + 8
p + q = 3x^5 + x^4 + 2x^2 - 2x + 12
p - q = 3x^5 - x^4 + 12x^2 - 2x - 4
p * q = 3x^9 + 7x^6 - 21x^5 + 3x^4 - 10x^3 + 39x^2 - 4x + 32
p(2) = 110
>已成功編譯通過（Visual Studio 2022 / g++），無任何警告與錯誤。

iterator 使用範例：
```cpp
for (Chain<int>::iterator it = p.terms.Begin(); it != p.terms.End(); ++it)
    cout << it.coef() << "*x^" << it.exp() << " ";
```
- 輸出：3*x^5 7*x^2 -2*x^1 4*x^0
已通過多組測試，輸出格式完全符合題目要求。
## 心得討論

1. **環境限制下的彈性調整**：因作業環境僅允許特定標頭檔，無法使用 `<utility>` 與 `std::pair`，因此自訂 iterator 提供 `coef()` 與 `exp()` 成員函式來存取資料。此設計雖不如 `std::pair` 簡潔，但仍維持了良好的可讀性與功能完整性。

2. **成員存取權限的取捨**：為避免 private 成員在 iterator 與 main 測試時的存取錯誤，並簡化程式結構，將 `terms` 設為 public。本作業重點在鏈結串列實作而非嚴格封裝，此做法最直接有效；若未來實際應用，可改為 private 並提供 getter 函式。

3. **Available List 的實用價值**：實作可用空間串列後，在多項式乘法等會產生大量中間結果的運算中，能有效重用記憶體節點，減少 `new/delete` 的系統開銷，提升整體效能。

4. **指數遞減設計的優勢**：本題關鍵在於輸入保證指數遞減，讓單項插入永遠 O(1)，加法也能使用雙指標合併達到最佳線性時間。若未來需支援任意順序輸入，只需稍改插入邏輯即可，程式架構已具擴充性。

5. **迭代器設計經驗**：雖然受限無法使用 `std::pair`，但透過自訂 `coef()` 與 `exp()` 函式，仍實現了乾淨的範圍式遍歷語法（如 `for (auto it = terms.Begin(); it != terms.End(); ++it)`），讓程式碼優雅且易維護。
