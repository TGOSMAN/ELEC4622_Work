#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "io_bmp.h"
#include "memOrg.h"


int main(int argc, char* argv[]) {
	//sequence to open and close a particular image with the changes for lab1 
	char* name = new char[64];
	sscanf(argv[1], "%s", name);
	io_image *sample = IO_ImageNew(name);
	IO_ImageAdd(sample, 60);
	IO_ImageWriteBMP(sample, name);
	IO_ImageDelete(sample);
	return 0;
}