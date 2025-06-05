#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {

	if (argc > 1) {
		int val;
		sscanf_s(argv[1],"%d",&val);
		printf("Hello, my argument is %d\n",val);
	}
	return 0;
}