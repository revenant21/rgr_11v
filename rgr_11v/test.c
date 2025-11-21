#include <stdio.h>
#include <locale.h>
#include <math.h>
#define a 0
#define b 2*3.14
#define n 20

void main() {
	setlocale(LC_ALL, "RUS");
	float F1 = 0, step = (b - a) / (n-1), F2 = 0;
	float x = a;
	printf("_n|_____x|_F1(x)|_F2(x)| \n");
	for (int i = 0; i < 20; i++) {
		F1 = 5 - 3 * cos(x);
		F2 = sqrt(1 + pow(sin(x), 2));
		printf("%2.d|%6.2f|%6.2f|%6.2f| \n", i + 1, x, F1, F2);
		x += step;
	}
}