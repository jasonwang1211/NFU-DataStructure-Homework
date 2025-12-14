#include <iostream>
#include <sstream>
#include <string>
#include <cmath>     

using namespace std;

// 1. ChainNode
template<class T>
struct ChainNode {
    T coef;
    T exp;
    ChainNode<T>* link;

    ChainNode(const T& c = 0, const T& e = 0, ChainNode<T>* l = nullptr)
        : coef(c), exp(e), link(l) {}
};

// 2. Chain 
template<class T>
class Chain {
private:
    ChainNode<T>* head;
    ChainNode<T>* tail;

public:
    // iterator
    class iterator {
    private:
        ChainNode<T>* current;
    public:
        iterator(ChainNode<T>* p = nullptr) : current(p) {}

        // 提供 public 成員讓外部存取 coef 和 exp
        T coef() const { return current->coef; }
        T exp() const { return current->exp; }

        iterator& operator++() {
            current = current->link;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator!=(const iterator& rhs) const {
            return current != rhs.current;
        }

        bool operator==(const iterator& rhs) const {
            return current == rhs.current;
        }

        friend class Chain<T>;
    };

    // Available List
    static ChainNode<T>* avail;

    static ChainNode<T>* NewNode(const T& c = 0, const T& e = 0) {
        if (avail) {
            ChainNode<T>* x = avail;
            avail = avail->link;
            x->coef = c;
            x->exp = e;
            x->link = nullptr;
            return x;
        }
        return new ChainNode<T>(c, e);
    }

    static void DeleteNode(ChainNode<T>* p) {
        p->link = avail;
        avail = p;
    }

    Chain() {
        head = new ChainNode<T>();
        head->link = head;
        tail = head;
    }

    ~Chain() {
        MakeEmpty();
        delete head;
    }

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

    bool IsEmpty() const { return head->link == head; }

    void InsertBack(const T& c, const T& e) {
        ChainNode<T>* newNode = NewNode(c, e);
        tail->link = newNode;
        newNode->link = head;
        tail = newNode;
    }

    iterator Begin() const { return iterator(head->link); }
    iterator End() const { return iterator(head); }

    ChainNode<T>* GetHead() const { return head; }
};

// 靜態成員定義
template<class T> ChainNode<T>* Chain<T>::avail = nullptr;

// 4. Polynomial
class Polynomial {
public:
    Chain<int> terms;

    Polynomial() = default;  // 預設建構函式

    // Copy constructor
    Polynomial(const Polynomial& a) {
        for (Chain<int>::iterator it = a.terms.Begin(); it != a.terms.End(); ++it) {
            terms.InsertBack(it.coef(), it.exp());
        }
    }

    // Assignment
    Polynomial& operator=(const Polynomial& a) {
        if (this != &a) {
            terms.MakeEmpty();
            for (Chain<int>::iterator it = a.terms.Begin(); it != a.terms.End(); ++it) {
                terms.InsertBack(it.coef(), it.exp());
            }
        }
        return *this;
    }

    // Destructor
    ~Polynomial() {
        terms.MakeEmpty();
    }

    // 加法核心
    Polynomial Add(const Polynomial& b) const {
        Polynomial result;
        Chain<int>::iterator aIt = terms.Begin();
        Chain<int>::iterator aEnd = terms.End();
        Chain<int>::iterator bIt = b.terms.Begin();
        Chain<int>::iterator bEnd = b.terms.End();

        while (aIt != aEnd && bIt != bEnd) {
            int ea = aIt.exp();
            int eb = bIt.exp();

            if (ea > eb) {
                result.terms.InsertBack(aIt.coef(), ea);
                ++aIt;
            } else if (ea < eb) {
                result.terms.InsertBack(bIt.coef(), eb);
                ++bIt;
            } else {
                int sum = aIt.coef() + bIt.coef();
                if (sum != 0) result.terms.InsertBack(sum, ea);
                ++aIt;
                ++bIt;
            }
        }
        while (aIt != aEnd) {
            result.terms.InsertBack(aIt.coef(), aIt.exp());
            ++aIt;
        }
        while (bIt != bEnd) {
            result.terms.InsertBack(bIt.coef(), bIt.exp());
            ++bIt;
        }
        return result;
    }

    Polynomial operator+(const Polynomial& b) const { return Add(b); }

    Polynomial operator-(const Polynomial& b) const {
        Polynomial negB;
        for (Chain<int>::iterator it = b.terms.Begin(); it != b.terms.End(); ++it) {
            negB.terms.InsertBack(-it.coef(), it.exp());
        }
        return Add(negB);
    }

    Polynomial operator*(const Polynomial& b) const {
        Polynomial result;
        for (Chain<int>::iterator aIt = terms.Begin(); aIt != terms.End(); ++aIt) {
            Polynomial temp;
            for (Chain<int>::iterator bIt = b.terms.Begin(); bIt != b.terms.End(); ++bIt) {
                int newC = aIt.coef() * bIt.coef();
                int newE = aIt.exp() + bIt.exp();
                temp.terms.InsertBack(newC, newE);
            }
            result = result.Add(temp);
        }
        return result;
    }

    float Evaluate(float x) const {
        float val = 0.0f;
        for (Chain<int>::iterator it = terms.Begin(); it != terms.End(); ++it) {
            val += it.coef() * static_cast<float>(pow(x, it.exp()));
        }
        return val;
    }
};

// 輸入
istream& operator>>(istream& is, Polynomial& x) {
    x.terms.MakeEmpty();
    string line;
    ws(is);                // 清除空白與換行
    getline(is, line);
    stringstream ss(line);
    int c, e;
    while (ss >> c >> e) {
        if (c != 0) {
            x.terms.InsertBack(c, e);
        }
    }
    return is;
}

// 輸出
ostream& operator<<(ostream& os, const Polynomial& x) {
    if (x.terms.IsEmpty()) {
        os << "0";
        return os;
    }

    bool first = true;
    for (Chain<int>::iterator it = x.terms.Begin(); it != x.terms.End(); ++it) {
        int c = it.coef();
        int e = it.exp();

        if (c == 0) continue;

        if (!first) {
            if (c > 0) os << " + ";
            else { os << " - "; c = -c; }
        } else if (c < 0) {
            os << "-";
            c = -c;
        }

        bool showCoef = (c != 1 || e == 0);
        if (showCoef) os << c;

        if (e > 0) {
            if (c != 1) os << "*";
            os << "x";
            if (e > 1) os << "^" << e;
        }

        first = false;
    }
    return os;
}

// main 測試
int main() {
    Polynomial p, q;
    cout << "輸入 p (coef exp 成對): ";
    cin >> p;
    cout << "輸入 q: ";
    cin >> q;

    cout << "p(x) = " << p << endl;
    cout << "q(x) = " << q << endl;
    cout << "p + q = " << (p + q) << endl;
    cout << "p - q = " << (p - q) << endl;
    cout << "p * q = " << (p * q) << endl;
    cout << "p(2) = " << p.Evaluate(2.0f) << endl;

    return 0;
}
