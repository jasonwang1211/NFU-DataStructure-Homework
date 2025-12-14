# 41143109
作業三報告

## Polynomial ADT（循環鏈結串列實作 + Available List 加分項）
---

## 解題說明

### 1.1 題目需求
實作一個完整的 **Polynomial** 類別，使用**帶頭結點的循環鏈結串列**表示單變數多項式，每項係數為整數，指數遞減排列。每個節點包含三個成員：`coef`、`exp`、`link`。

必須實作以下功能：
(a) `istream& operator>>`（讀入 coef exp 成對）  
(b) `ostream& operator<<`（美觀輸出多項式）  
(c) Copy constructor  
(d) Assignment operator (`operator=`)  
(e) Destructor（將所有節點回收至 Available List）  
(f) `operator+`  
(g) `operator-`  
(h) `operator*`  
(i) `float Evaluate(float x)`  

加分項：實作 Available-space list 進行記憶體回收與重用。

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
| **1. ChainNode**      | 泛型結構體，包含 `coef`、`exp`、`link`                                                                    |
| **2. Chain<T>**       | 帶頭結點的循環鏈結串列，維護 `head` 與 `tail`，提供 `InsertBack`、`Begin()`、`End()`                   |
| **3. ChainIterator**  | 支援前置 `++`、`*`（回傳 `pair<int,int>`）、`!=`，讓遍歷語法與 STL 一致                                |
| **4. Polynomial**     | **新增預設建構函式**，內部成員 `public: Chain<int> terms;`（方便 main 測試與 iterator 使用）         |
| **Available List**    | `static ChainNode<T>* avail`，`NewNode()` 優先從 avail 取，`DeleteNode()` 回收節點                   |
| **輸入**              | 先清除緩衝後讀整行，再用 stringstream 解析 coef exp 配對，零係數自動忽略                            |
| **輸出**              | 使用 iterator 走訪，處理正負號、係數 1 省略、x^1 不顯示 ^1、常數項等格式                              |
| **加減法**            | 雙指標合併（類似 merge），相同指數係數相加減，結果為 0 則不插入                                       |
| **乘法**              | 每對項相乘產生新項，再呼叫 `Add` 合併（自動消除零係數）                                              |
| **Evaluate**          | 使用 `pow(x, exp)` 逐項計算，回傳 float                                                            |

---

## 程式實作（關鍵程式碼）

> 完整程式已上傳至  
> https://github.com/jasonwang1211/NFU-DataStructure-Homework/blob/main/Homework3/Polynomial.cpp

```cpp
// 1. ChainNode
template<class T>
struct ChainNode {
    T coef, exp;
    ChainNode<T>* link;
    ChainNode(const T& c = 0, const T& e = 0, ChainNode<T>* l = nullptr)
        : coef(c), exp(e), link(l) {}
};

// 2. Chain (關鍵片段)
template<class T>
class Chain {
public:
    ChainNode<T>* head;
    ChainNode<T>* tail;
    static ChainNode<T>* avail;

    Chain() {
        head = new ChainNode<T>;
        head->link = head;
        tail = head;
    }

    ~Chain() { MakeEmpty(); delete head; }

    void MakeEmpty() {
        ChainNode<T>* cur = head->link;
        while (cur != head) {
            ChainNode<T>* next = cur->link;
            DeleteNode(cur);
            cur = next;
        }
        head->link = head;
        tail = head;
    }

    void InsertBack(const T& c, const T& e) {
        ChainNode<T>* newNode = NewNode(c, e);
        tail->link = newNode;
        newNode->link = head;
        tail = newNode;
    }

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

    // iterator 相關略...
};

// 4. Polynomial (已修正所有錯誤)
class Polynomial {
public:
    Chain<int> terms;                     // 改為 public，避免 private 存取錯誤
    Polynomial() {}                       // ★ 新增預設建構函式（關鍵修正！）

    // Copy constructor
    Polynomial(const Polynomial& a) {
        for (auto it = a.terms.Begin(); it != a.terms.End(); ++it)
            terms.InsertBack((*it).first, (*it).second);
    }

    // Assignment
    Polynomial& operator=(const Polynomial& a) {
        if (this != &a) {
            terms.MakeEmpty();
            for (auto it = a.terms.Begin(); it != a.terms.End(); ++it)
                terms.InsertBack((*it).first, (*it).second);
        }
        return *this;
    }

    // Destructor（回收到 Available List）
    ~Polynomial() {
        terms.MakeEmpty();
    }
};

// 輸入運算子（已修正 getline 錯誤）
istream& operator>>(istream& is, Polynomial& x) {
    x.terms.MakeEmpty();
    string line;
    std::cin >> std::ws;              // 清除前導空白與換行
    getline(is, line);
    stringstream ss(line);
    int c, e;
    while (ss >> c >> e) {
        if (c != 0)
            x.terms.InsertBack(c, e);
    }
    return is;
}

// 其他 operator+、operator-、operator*、Evaluate 實作與作業2相同（略）
