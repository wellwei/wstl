#include <iostream>
#include <fstream>
#include <string>
#include<vector>

using namespace std;
using ll = long long;
#define AC 0
#define WA 1
#define ERROR (-1)

int spj(std::ifstream &input, std::ifstream &user_output);

int main(int argc, char *args[]) {
    std::ifstream input, user_output;
    int result;
    if (argc != 3) {
        std::cout << "Usage: spj x.in x.out" << std::endl;
        return ERROR;
    }
    input.open(args[1], std::ios::in);
    user_output.open(args[2], std::ios::in);
    if (!input.is_open() || !user_output.is_open()) {
        std::cout << "Failed to open file" << std::endl;
        if (input.is_open()) input.close();
        if (user_output.is_open()) user_output.close();
        return ERROR;
    }

    result = spj(input, user_output);
    std::cout << "result: " << result << std::endl;

    input.close();
    user_output.close();
    return result;
}


long long n;

vector<ll> mmm(101);

int solve() {
    long long cnt = 0;
    vector<ll> pos[101];
    vector<ll> ans(n, 3);
    for (int i = 0; i < n; i++) {
        ll x;
        x = mmm[i];
        pos[x].emplace_back(i);
    }
    for (int i = 1; i <= 100; i++) {
        if (pos[i].size() >= 2) {
            ans[pos[i].front()] = ++cnt;
            if (cnt == 2) {
                return 1;
            }
        }
    }
    return 0;

}

/**
 *
 * @param input：输入文件流
 * @param user_output：用户输出文件流
 * @return int：返回值为 AC, WA, ERROR
 * @note：AC 表示答案正确，WA 表示答案错误，ERROR 表示程序错误 (如文件打开失败、内存分配失败等)
 */
int spj(std::ifstream &input, std::ifstream &user_output) {

    int t;
    input >> t;
    while (t--) {
        input >> n;
        vector<ll> a(n);
        for (int i = 0; i < n; i++) {
            input >> a[i];
            mmm[i] = a[i];
        }
        if (!solve()) {
            ll bb;
            user_output >> bb;
            if (bb == -1) {
                continue;
            } else {
                return WA;
            }
        } else {
            long long cnt11 = 0;
            ll cnt22 = 0;
            ll cnt33 = 0;
            vector<long long> b(n);
            for (int i = 0; i < n; i++) {
                user_output >> b[i];
            }
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    if (j == i) {
                        continue;
                    }
                    if (a[i] == a[j] && (b[i] == 1 && b[j] == 2))cnt11 = 1;
                    if (a[i] == a[j] && (b[i] == 1 && b[j] == 3))cnt22 = 1;
                    if (a[i] == a[j] && (b[i] == 2 && b[j] == 3))cnt33 = 1;
                }
            }
            if ((cnt11 + cnt22 + cnt33) != 2) {
                return WA;
            }
        }

    }
    return AC;
}