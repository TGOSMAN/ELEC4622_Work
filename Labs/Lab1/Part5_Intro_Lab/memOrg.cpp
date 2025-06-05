
#include <stdlib.h>
#include <stdio.h>
#include "memOrg.h"
#include "io_bmp.h"


/*Function to Read, Create and Fill Image Component Buffers From BMP*/
/*Assume File is In CWD*/
struct io_image IO_ImageNew(char* fname) {
	/*FILE* file;
	file = fopen(fname,"r");
	//Check if file successfully opened
	if (file == NULL) {
		fprintf(stderr, "FILE NOT OPENED SUCCESSFULLLY");
		return NULL;
	}*/
	bmp_in in;
	io_image image_placeholder = {0};

	// Attempt to open the BMP file
	if (bmp_in__open(&in, fname) != 0) { // Assuming bmp_in__open returns non-zero on failure
		fprintf(stderr, "FILE NOT OPENED SUCCESSFULLY\n");
		// Initialize the image with default values
		image_placeholder.num_components = 0;
		image_placeholder.comps = nullptr;
		return image_placeholder;
	}

	image_placeholder.num_components = in.num_components;
	image_placeholder.comps = new io_comp[in.num_components];
	printf("My Components Are: %d\n", image_placeholder.num_components);
	/*Setup the data structure and initialise memory with boundary*/
	for (int c = 0; c < in.num_components; c++) {
		struct io_comp* comp = image_placeholder.comps + c;
		comp->width = in.cols;
		comp->height = in.rows;
		comp->stride = comp->width + 32;// 16 extra cols left & right
		int true_height = comp->height + 32;// 16 extra rows top & bottom
		comp->handle = new int[(comp->stride) * (true_height)];
		comp->buf = (comp->handle) + (16 * (comp->stride)) + 16;
	}
	printf("%d\n", image_placeholder.comps[0].height);
	/*Now we put the data from the file in the components*/
	io_byte* buffered_row = new io_byte[in.num_components * in.cols];//block of memory to process colours from
	for (int n = in.rows; n > 0; n--) {
		bmp_in__get_line(&in, buffered_row);//read the row in from bottom to top
		for (int i = 0; i < in.num_components; i++) {
			struct io_comp* comp = image_placeholder.comps + i;
			for (int j = i; j < in.num_components * in.cols; j += in.num_components) {
				comp->buf[n * (comp->stride) + (j / in.num_components)] = buffered_row[j];
			}
		}
	}
	delete[] buffered_row;
	//bmp_in__get_line(&in, dp);
	bmp_in__close(&in);
	return image_placeholder;
}


void IO_ImageWriteBMP(io_image Input_Image, char*fname){




}

void IO_ImageAdd(io_image Input_Image, int val){



}

void IO_ImageDelete(void) {


}
