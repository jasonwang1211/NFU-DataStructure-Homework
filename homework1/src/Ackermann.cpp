#include <iostream>
using namespace std;

// 非遞迴 Ackermann
long long ackermannIter(int m, int n)
{
    const int MAX_STK = 100000;
    int* stack = new int[MAX_STK];
    int top = -1;

    while (true)
    {
        if (m == 0)               // A(0,n) = n+1
        {
            n = n + 1;
            if (top < 0) break;   
            m = stack[top--];     // 彈回上一層的 m
        }
        else if (n == 0)          // A(m,0) = A(m‑1,1)
        {
            n = 1;
            m = m - 1;
        }
        else                       // A(m,n) = A(m‑1, A(m,n‑1))
        {
            stack[++top] = m - 1;  
            n = n - 1;              // 再處理內層 A(m,n‑1)
            
        }
    }

    delete[] stack;
    return n;                     // 最後的 n 就是 A(m0,n0)
}

// 遞迴版 Ackermann
long long ackermannRec(int m, int n)
{
    if (m == 0) return n + 1;
    if (n == 0) return ackermannRec(m - 1, 1);
    return ackermannRec(m - 1,
        ackermannRec(m, n - 1));
}

// 主程式 – 讀入 m、n，顯示兩種實作的結果
int main()
{
    int m, n;

    cout << "請輸入 m 與 n（以空白分隔）: ";
    cin >> m >> n;                 // 讀入使用者提供的 m、n

    // 遞迴版結果
    long long ansRec = ackermannRec(m, n);
    cout << "遞迴版  A(" << m << "," << n << ") = " << ansRec << '\n';

    // 迭代版結果
    long long ansItr = ackermannIter(m, n);
    cout << "迭代版  A(" << m << "," << n << ") = " << ansItr << '\n';

    return 0;
}

