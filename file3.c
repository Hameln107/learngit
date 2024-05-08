#include <stdio.h>
#include <string.h>


int plusten(int x);
int change(int (*pointer)(int), int x);


int main() {	

	int (*ptr)(int) = plusten;
	printf("ptr(10): %i \n", ptr(10));
	printf("change(ptr, 5): %i ", change(ptr, 5));

	return 0;
}

int plusten(int x) {
	return x + 10;
}

int change(int (*pointer)(int), int x) {
	return pointer(x);
}
