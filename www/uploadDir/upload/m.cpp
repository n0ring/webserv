#include <vector>
#include <iostream>


int maxSumArr(std::vector<int> arr, int k) {
	int maxSum = INT_MIN;
	int currentSum = 0;

	for (int i = 0; i < arr.size(); i++) {
		currentSum += arr[i];
		if (i >= k) {
			currentSum -= arr[i - k];
		}
		maxSum = std::max(maxSum, currentSum);
	}
	return maxSum;
}

int main(void) {
	std::vector<int> arr(6, 0);
	arr[0] = 1;
	arr[1] = 2;

	std::cout << maxSumArr(arr, 3) << std::endl;
	return 0;
}