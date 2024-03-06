#include <stdio.h>
int main() {
	int n;
	scanf("%d", &n);

	// 1. 得到一个int，是输入的对称
	int original = n;
	int reversed = 0;
	while(n>0){
		int digit = n % 10;
		reversed = reversed*10 + digit;
		n /= 10;	
	}

	// 2.  判断对称之后是否还等于本身
	if (reversed==original) {
		printf("Y\n");
	} else {
		printf("N\n");
	}
	return 0;
}
