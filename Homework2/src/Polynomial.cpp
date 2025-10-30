#include <iostream>
#include <iomanip>      
#include <cmath>        
#include <cstring>      
using namespace std;


class Polynomial {
public:

    Polynomial();                                 
    Polynomial(const Polynomial& other);          
    Polynomial& operator=(const Polynomial& other);
    ~Polynomial();


    Polynomial Add(const Polynomial& poly) const; 
    Polynomial Mult(const Polynomial& poly) const; 
    float      Eval(float x) const;               


    friend istream& operator>>(istream& is, Polynomial& poly);
    friend ostream& operator<<(ostream& os, const Polynomial& poly);

private:
    
    struct Term {
        float coef;   
        int   exp;    
    };

    Term* termArray;   
    int   capacity;    
    int   terms;       

    void init();                
    void expand();              
    void insertTerm(const Term& t); 
    void cleanZero();          
};

Polynomial::Polynomial() { init(); }

void Polynomial::init()
{
    capacity = 2;              
    terms = 0;
    termArray = new Term[capacity];
}


Polynomial::Polynomial(const Polynomial& other)
{
    capacity = other.capacity;
    terms = other.terms;
    termArray = new Term[capacity];
    memcpy(termArray, other.termArray, terms * sizeof(Term));
}

Polynomial& Polynomial::operator=(const Polynomial& other)
{
    if (this != &other) {
        delete[] termArray;
        capacity = other.capacity;
        terms = other.terms;
        termArray = new Term[capacity];
        memcpy(termArray, other.termArray, terms * sizeof(Term));
    }
    return *this;
}


Polynomial::~Polynomial()
{
    delete[] termArray;
}


void Polynomial::expand()
{
    int newCap = capacity * 2;
    Term* newArr = new Term[newCap];
    memcpy(newArr, termArray, terms * sizeof(Term));
    delete[] termArray;
    termArray = newArr;
    capacity = newCap;
}


void Polynomial::insertTerm(const Term& t)
{
    if (t.coef == 0.0f) return;              

    
    for (int i = 0; i < terms; ++i) {
        if (termArray[i].exp == t.exp) {
            termArray[i].coef += t.coef;
            if (fabs(termArray[i].coef) < 1e-6f) { 
                for (int j = i; j < terms - 1; ++j)
                    termArray[j] = termArray[j + 1];
                --terms;
            }
            return;
        }
    }

    if (terms == capacity) expand();          
    termArray[terms++] = t;
}

void Polynomial::cleanZero()
{
    int write = 0;
    for (int i = 0; i < terms; ++i) {
        if (fabs(termArray[i].coef) >= 1e-6f) {
            termArray[write++] = termArray[i];
        }
    }
    terms = write;
}


Polynomial Polynomial::Add(const Polynomial& poly) const
{
    Polynomial result;
    for (int i = 0; i < terms; ++i)   result.insertTerm(termArray[i]);
    for (int i = 0; i < poly.terms; ++i) result.insertTerm(poly.termArray[i]);
    result.cleanZero();
    return result;
}

Polynomial Polynomial::Mult(const Polynomial& poly) const
{
    Polynomial result;
    for (int i = 0; i < terms; ++i)
        for (int j = 0; j < poly.terms; ++j) {
            Term t;
            t.coef = termArray[i].coef * poly.termArray[j].coef;
            t.exp = termArray[i].exp + poly.termArray[j].exp;
            result.insertTerm(t);
        }
    result.cleanZero();
    return result;
}

float Polynomial::Eval(float x) const
{
    float sum = 0.0f;
    for (int i = 0; i < terms; ++i)
        sum += termArray[i].coef * static_cast<float>(pow(x, termArray[i].exp));
    return sum;
}

/* ------------------- I/O Operator --------------------- */

istream& operator>>(istream& is, Polynomial& poly)
{
    poly = Polynomial();                   
    int n;
    if (!(is >> n)) return is;             

    for (int i = 0; i < n; ++i) {
        int   e;   float c;
        is >> e >> c;
        Polynomial::Term t; t.exp = e; t.coef = c;
        poly.insertTerm(t);
    }
    poly.cleanZero();
    return is;
}

ostream& operator<<(ostream& os, const Polynomial& p)
{
    if (p.terms == 0) { os << "0"; return os; }

    Polynomial::Term* sorted = new Polynomial::Term[p.terms];
    memcpy(sorted, p.termArray, p.terms * sizeof(Polynomial::Term));
    for (int i = 0; i < p.terms - 1; ++i)
        for (int j = i + 1; j < p.terms; ++j)
            if (sorted[i].exp < sorted[j].exp) {
                Polynomial::Term tmp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = tmp;
            }

    for (int i = 0; i < p.terms; ++i) {
        const Polynomial::Term& t = sorted[i];
        if (i > 0) os << (t.coef >= 0 ? " + " : " - ");
        else if (t.coef < 0) os << "-";

        float a = fabs(t.coef);
        if (t.exp == 0) {                     
            os << setprecision(6) << a;
        }
        else {
            if (a != 1.0f) os << setprecision(6) << a;
            os << "x";
            if (t.exp != 1) os << "^" << t.exp;
        }
    }
    delete[] sorted;
    return os;
}

int main()
{
    Polynomial p, q;
    cout << "請輸入第一個多項式（先輸入項數，接著每行 exponent coefficient）:" << endl;
    cin >> p;
    cout << "請輸入第二個多項式：" << endl;
    cin >> q;

    cout << "\n第一個多項式 p(x) = " << p << endl;
    cout << "第二個多項式 q(x) = " << q << endl;

    Polynomial sum = p.Add(q);
    Polynomial prod = p.Mult(q);
    cout << "\np(x) + q(x) = " << sum << endl;
    cout << "p(x) * q(x) = " << prod << endl;

    float x;
    cout << "\n請輸入要評估的 x 值: ";
    cin >> x;
    cout << "p(" << x << ") = " << p.Eval(x) << endl;
    cout << "q(" << x << ") = " << q.Eval(x) << endl;
    cout << "(p+q)(" << x << ") = " << sum.Eval(x) << endl;
    cout << "(p*q)(" << x << ") = " << prod.Eval(x) << endl;
    return 0;
}
