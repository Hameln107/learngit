#include <stdio.h>
#include <math.h>
#include <stdbool.h>


int main() {	

	int arr[5] = {1, 2, 3, 4, 6};

	for (int i = 0; i < 5; i++)
		printf("%f\n", sqrt(arr[i]));

	return 0;
}
