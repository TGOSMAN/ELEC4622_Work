#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "io_bmp.h"
#include "memOrg.h"


int main(int argc, char* argv[]) {
	
	char* name = new char[64];
	sscanf(argv[1], "%s", name);
	io_image *sample = IO_ImageNew(name);
	IO_ImageAdd(sample, 60);
	IO_ImageWriteBMP(sample, name);
	//sample.num_components += 1;
	// printf("Sample Num Components %d\n", sample.num_components);
	return 0;
}