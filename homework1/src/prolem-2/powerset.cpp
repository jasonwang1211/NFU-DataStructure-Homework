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

