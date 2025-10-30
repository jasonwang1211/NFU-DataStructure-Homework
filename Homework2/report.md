# 41143109
 
 作業二 
## Polynomial ADT
## 解題說明

### 1.1 題目需求
實作 **Polynomial** 類別，使其能：

* 表示任意次多項式（每項為 `<係數, 指數>`，指數為非負整數）。
* 支援 **加法**、**乘法**、**求值** (`Eval`)。
* 提供 **`>>`、`<<`** 運算子完成 I/O（輸入格式為「項數」+「每行 exponent coefficient」）。

### 1.2 範例
輸入 (p)： 3 2 5.0 0 -1.2 5 3.4
輸入 (q)： 2 1 2.0 3 -1.0

*`p(x) = 5x² - 1.2 + 3.4x⁵`*  
*`q(x) = 2x - x³`*  

p(x) + q(x) = 3.4x⁵ + 5x² - x³ + 2x - 1.2 p(x) * q(x) = -5x⁵ + 3.4x⁸ - 2.4x³ + 6.8x⁶ - 0.24x


---

## 解題策略

| 步驟 | 實作說明 |
|------|----------|
| **資料結構** | `Term` 保存 `float coef`、`int exp`（private），`Polynomial` 內以 `Term* termArray`、`int capacity`、`int terms` 動態管理非零項。 |
| **Friend** | `Term` 宣告 `friend class Polynomial;`，讓 `Polynomial` 能直接存取 `coef` 与 `exp`（解決了編譯錯誤）。 |
| **插入/合併** | `insertTerm`：若已有相同 `exp`，係數相加；若結果為 0，刪除該項。 |
| **動態擴容** | 當 `terms == capacity` 時把容量 *2 重新配置（`expand()`），保證 **均攤 O(1)** 的插入成本。 |
| **加法** | 依序把兩個多項式的項目插入結果，多項式的合併全部交給 `insertTerm` 完成。 |
| **乘法** | 雙層迴圈生成每一對項的乘積（`coef*coef`、`exp+exp`），再交給 `insertTerm` 合併。 |
| **求值** | 直接遍歷 `termArray` ，累加 `coef * x^exp`（`pow` 來自 `<cmath>`）。 |
| **I/O** | `operator>>` 讀取「項數 + (exp coef)」序列；`operator<<` 先依指數遞減排序，再依慣例（係數 1 省略、指數 0 只印係數、指數 1 省略 `^1`）輸出。 |

---

## 程式實作（關鍵程式碼）

> 完整程式請見 `Polynomial.cpp`（已放入 `src/` 目錄）。以下摘錄最具代表性的區段。

```cpp
/* ---------- Term ---------- */
class Polynomial;                 // 前向宣告
class Term {
    friend class Polynomial;      // 讓 Polynomial 能直接存取 coef / exp
private:
    float coef;
    int   exp;
};

/* ---------- insertTerm (合併同指數) ---------- */
void Polynomial::insertTerm(const Term &t)
{
    if (t.coef == 0.0f) return;               // 零係數直接拋棄
    for (int i = 0; i < terms; ++i) {          // 找到相同 exponent
        if (termArray[i].exp == t.exp) {
            termArray[i].coef += t.coef;
            if (fabs(termArray[i].coef) < 1e-6f) { // 變 0，刪除
                for (int j = i; j < terms - 1; ++j)
                    termArray[j] = termArray[j + 1];
                --terms;
            }
            return;
        }
    }
    if (terms == capacity) expand();           // 必要時擴容
    termArray[terms++] = t;                    // 新增項目
}

/* ---------- 加法 ---------- */
Polynomial Polynomial::Add(const Polynomial &poly) const
{
    Polynomial result;
    for (int i = 0; i < terms; ++i)   result.insertTerm(termArray[i]);
    for (int i = 0; i < poly.terms; ++i) result.insertTerm(poly.termArray[i]);
    result.cleanZero();
    return result;
}

/* ---------- 乘法 ---------- */
Polynomial Polynomial::Mult(const Polynomial &poly) const
{
    Polynomial result;
    for (int i = 0; i < terms; ++i)
        for (int j = 0; j < poly.terms; ++j) {
            Term t;
            t.coef = termArray[i].coef * poly.termArray[j].coef;
            t.exp  = termArray[i].exp  + poly.termArray[j].exp;
            result.insertTerm(t);
        }
    result.cleanZero();
    return result;
}

/* ---------- 輸入運算子 ---------- */
istream& operator>>(istream &is, Polynomial &poly)
{
    poly = Polynomial();                // 清空舊資料
    int n; if (!(is >> n)) return is;   // 讀不到項數直接返回
    for (int i = 0; i < n; ++i) {
        int   e; float c; is >> e >> c;
        Term t; t.exp = e; t.coef = c;
        poly.insertTerm(t);
    }
    poly.cleanZero();
    return is;
}

/* ---------- 輸出運算子 ---------- */
ostream& operator<<(ostream &os, const Polynomial &p)
{
    if (p.terms == 0) { os << "0"; return os; }
    // 依指數遞減排序（簡易 bubble）
    Term *sorted = new Term[p.terms];
    memcpy(sorted, p.termArray, p.terms * sizeof(Term));
    for (int i = 0; i < p.terms - 1; ++i)
        for (int j = i + 1; j < p.terms; ++j)
            if (sorted[i].exp < sorted[j].exp) {
                Term tmp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = tmp;
            }
    // 輸出
    for (int i = 0; i < p.terms; ++i) {
        const Term &t = sorted[i];
        if (i > 0) os << (t.coef >= 0 ? " + " : " - ");
        else if (t.coef < 0) os << "-";
        float a = fabs(t.coef);
        if (t.exp == 0) { os << setprecision(6) << a; }
        else {
            if (a != 1.0f) os << setprecision(6) << a;
            os << "x";
            if (t.exp != 1) os << "^" << t.exp;
        }
    }
    delete[] sorted;
    return os;
}
```
## 效能分析
## 5️⃣ 效能分析

| 演算法 / 操作 | 時間複雜度 | 空間複雜度 | 說明 |
|----------------|------------|------------|------|
| **insertTerm**（單項插入） | **`O(t)`**<br>（`t` 為目前已存的非零項數） | **`O(1)`**（不計擴容） | 必須線性搜尋已有項目以判斷指數是否相同；若相同則係數相加，否則直接加入。 |
| **Add**（多項式相加） | **`O(t₁ + t₂)`** | **`O(t₁ + t₂)`**（結果多項式的項數） | 依序把 `*this` 與 `poly` 的每一項插入結果，多次呼叫 `insertTerm` 完成合併。 |
| **Mult**（多項式相乘） | **`O(t₁ · t₂)`** | **`O(t₁ · t₂)`**（最壞情況下，每對項目都產生新項） | 雙層迴圈產生每一對項的乘積（係數相乘、指數相加），再交給 `insertTerm` 合併。 |
| **Eval**（求值） | **`O(t)`** | **`O(1)`** | 只需遍歷所有非零項，對每項呼叫一次 `pow(x,exp)`。 |
| **I/O（>> / <<）** | **`O(t)`** | **`O(1)`** | 讀取或輸出時只需一次線性掃描 `t` 個項目，`<<` 內部做一次簡易排序（bubble sort），但 `t` 通常很小，仍視為 `O(t)`。 |

> **`t`、`t₁`、`t₂`** 均代表「多項式的非零項個數」。  
> 以上複雜度皆已達到該問題的理論下界——多項式運算必須檢視（或產生）每個非零項，無法再進一步降低。
