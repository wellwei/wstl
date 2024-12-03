#include <fstream>
#include <iostream>
#include <string>

#define AC 0
#define WA 1
#define ERROR -1

int spj(std::ifstream &input, std::ifstream &user_output);

int main(int argc, char *args[]) { // main函数不要修改
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
		if (input.is_open())
			input.close();
		if (user_output.is_open())
			user_output.close();
		return ERROR;
	}

	result = spj(input, user_output);
	std::cout << "result: " << result << std::endl;

	input.close();
	user_output.close();
	return result;
}

#include <algorithm>
using namespace std;

struct node {
	long long x, id;
} e[1000000];
long long in[1000000], user_out[1000000], ans[1000000];
bool cmp(node a, node b) {
	return a.x < b.x;
}

int spj(std::ifstream &input, std::ifstream &user_output) { // input是输入测试数据，user_output是用户输出的答案，先读入系统测试数据，计算出标准答案，再读入用户输出，判断是否正确
	int t;
	input >> t;
	while (t--) { // 多组数据
		long long n;

		{ // 读入数据，计算标准答案
			input >> n;
			for (int i = 1; i <= n; i++) {
				input >> in[i];
				e[i].x = in[i];
				e[i].id = i;
			}
			sort(e + 1, e + 1 + n, cmp);
			for (int i = 1; i <= n; i++) {
				ans[e[i].id] = n - i + 1;
			}
		}

		// ans即为正确答案

		long long t_ans = 0, u_ans = 0; // t_ans为标准答案的绝对值之和，u_ans为用户答案的绝对值之和
		for (int i = 1; i <= n; i++) {
			t_ans += abs(ans[i] - in[i]);
		}

		for (int i = 1; i <= n; i++) {
			user_output >> user_out[i];
			u_ans += abs(user_out[i] - in[i]);
		}

		sort(user_out + 1, user_out + 1 + n);
		for (int i = 1; i <= n; i++) { // 判断用户输出格式是否正确
			if (user_out[i] != i) {
				return WA;
			}
		}

		if (t_ans != u_ans) { // 判断用户输出是否正确
			return WA;
		}
	}

	return AC;
}
