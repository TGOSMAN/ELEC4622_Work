#include "memOrg.h"
#include "../bmp_io/bmp_io/io_bmp.h"
#include <stdlib.h>
#include <stdio.h>

/*Function to Read, Create and Fill Image Component Buffers From BMP*/
/*Assume File is In CWD*/
io_image IO_ImageNew(char* fname) {
	/*FILE* file;
	file = fopen(fname,"r");
	//Check if file successfully opened
	if (file == NULL) {
		fprintf(stderr, "FILE NOT OPENED SUCCESSFULLLY");
		return NULL;
	}*/

	bmp_in in;
	bmp_in__open(&in, fname);
	io_image temp_image;
	temp_image.num_components = in.num_components;
	temp_image.comps = new io_comp[in.num_components];
	/*Setup the data structure and initialise memory with boundary*/
	for (int c = 0; c < in.num_components; c++) {
		io_comp* comp = temp_image.comps + c;
		comp->width = in.cols;
		comp->height = in.rows;
		comp->stride = comp->width + 32;// 16 extra cols left & right
		int true_height = comp->height + 32;// 16 extra rows top & bottom
		comp->handle = new int[(comp->stride) * (true_height)];
		comp->buf = (comp->handle) + (16 * (comp->stride)) + 16;
	}
	/*Now we put the data from the file in the components*/
	io_byte* buffered_row = new io_byte[in.num_components * in.cols];//block of memory to process colours from
	for (int n = in.rows; n > 0; n--) {
		bmp_in__get_line(&in, buffered_row);//read the row in from bottom to top
		for (int i = 0; i < in.num_components; i++) {
			io_comp* comp = temp_image.comps + i;
			for (int j = i; j <= in.num_components + in.line_bytes; j += in.num_components) {
				comp->buf[n * (comp->stride) + (j/in.num_components)] = buffered_row[j];
			}
		}
	}
	//bmp_in__get_line(&in, dp);
	delete[] buffered_row;
	bmp_in__close(&in);
	return temp_image;
}


void IO_ImageWriteBMP(io_image Input_Image, char*fname){




}

void IO_ImageAdd(io_image Input_Image, int val){



}

void IO_ImageDelete(void) {
	

}
