#include <stdio.h>

extern int add(int, int);
extern int sub(int, int);

int main(int argc, const char *argv[])
{
	printf("10 + 20 = %d\n", add(10, 20));	
	printf("20 - 10 = %d\n", sub(20, 10));	

	return 0;
}
