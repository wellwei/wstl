#include <iostream>
#include <vector>

#include "allocator.h"

class Solution {
public:
	bool isValid(std::vector<int>& queens, int row, int col) {
		for (int i = 0; i < row; i++) {
			if (queens[i] == col || queens[i] - col == i - row || queens[i] - col == row - i) {
				return false;
			}
		}
		return true;
	}

	void dfs(std::vector<int>& queens, int row, int& res) {
		if (row == queens.size()) {
			res++;
			return;
		}
		for (int col = 0; col < queens.size(); col++) {
			if (isValid(queens, row, col)) {
				queens[row] = col;
				dfs(queens, row + 1, res);
				queens[row] = -1;
			}
		}
	}

	int totalNQueens(int n) {
		int res = 0;
		std::vector<int> queens(n, -1);
		dfs(queens, 0, res);
		return res;
	}
};

int main() {


	return 0;
}
