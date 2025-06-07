
#include <stdlib.h>
#include <stdio.h>
#include "memOrg.h"
#include "io_bmp.h"


/*Function to Read, Create and Fill Image Component Buffers From BMP*/
/*Assume File is In CWD*/
struct io_image *IO_ImageNew(char* fname) {
	/*FILE* file;
	file = fopen(fname,"r");
	//Check if file successfully opened
	if (file == NULL) {
		fprintf(stderr, "FILE NOT OPENED SUCCESSFULLLY");
		return NULL;
	}*/
	bmp_in in;
	io_image *image = new io_image;

	// Attempt to open the BMP file
	if (bmp_in__open(&in, fname) != 0) { // Assuming bmp_in__open returns non-zero on failure
		fprintf(stderr, "FILE NOT OPENED SUCCESSFULLY\n");
		// Initialize the image with default values
		image->num_components = 0;
		image->comps = nullptr;
		return image;
	}

	image->num_components = in.num_components;
	image->comps = new io_comp[in.num_components];
	printf("My Components Are: %d, in_line_bytes: %d\n", image->num_components, in.line_bytes);
	/*Setup the data structure and initialise memory with boundary*/
	struct io_comp* comp = image->comps;
	for (int c = 0; c < in.num_components; c++) {
		//struct io_comp* comp = image->comps + c;
		comp[c].width = (in.line_bytes)/(in.num_components);
		comp[c].height = in.rows;
		comp[c].stride = comp[c].width + 32;// 16 extra cols left & right
		int true_height = comp[c].height + 32;// 16 extra rows top & bottom
		comp[c].handle = new int[(comp[c].stride) * (true_height)];
		comp[c].buf = (comp[c].handle) + (16 * (comp[c].stride)) + 16;
	}
	printf("Width: %dHeight:%d\n", image->comps[0].width, image->comps[0].height);
	/*Now we put the data from the file in the components*/
	io_byte* buffered_row = new io_byte[in.line_bytes];//block of memory to process colours from
	for (int n = in.rows; n >= 0; n--) {
		bmp_in__get_line(&in, buffered_row);//read the row in from bottom to top
		for (int i = 0; i < in.num_components; i++) {
			//struct io_comp* comp = image->comps;// i = 0 is blue , i = 1 is green, i = 2 is red	
			for (int j = i; j < in.line_bytes; j += in.num_components) {
				//note line bytes are use since this doesnt include the padding and our buffered row will not have the padding
				comp[i].buf[n * (comp->stride) + (j / in.num_components)] = buffered_row[j];
			}
		}
	}
	delete[] buffered_row;
	bmp_in__close(&in);
	return image;
}


void IO_ImageWriteBMP(io_image *Input_Image, char*fname){
	if ((Input_Image == NULL)||(fname == NULL)||(Input_Image->comps == NULL)) {
		//Need to free memory elegantly
		fprintf(stderr, "EMPTY BUFFER OR NAME CANNOT WRITE FILE\n");

		return;
	}
	bmp_out out;
	int width, stride, height,num_components, error;
	//assuming all colour components are same dimensions
	num_components = Input_Image->num_components;
	width = (Input_Image->comps[0].width);
	height = Input_Image->comps[0].height;
	stride = Input_Image->comps[0].stride;
	error = bmp_out__open(&out, fname,width,height,num_components);
	if (error != 0) {
		fprintf(stderr, "bmp_out__open CANNOT OPEN FILE, Code: %d\n",error);
		return;
	}
	printf("Struct Dimensions: [Comp: Width: %d, Height: %d, Stride: %d]\n\r Image Dimensions: [Width: %d, Height: %d]\n\r", Input_Image->comps[0].width, Input_Image->comps[0].height,stride, width, height);
	//everything is setup for us properly proceed to write file with buffer
	io_byte* buffered_row = new io_byte[width*num_components];
	io_comp* comp = Input_Image->comps;
	// start at the bottom of our component buffers and top of the file
	// Moving backwards in a file is more annoying and probably expensive than backwards in a memory buffer
	int rel_pos = 0;
	for (int n = height -1; n >= 0; n--) {
		//fill each position of the buffered row with each value of the coloured buffer
		rel_pos = 0;
		for (int buffpos = 0; buffpos < (width)*num_components; buffpos+= num_components) {
			rel_pos = buffpos / num_components;
			for (int i = 0; i < num_components; i++) {
				//naively don't check for overflows so far
				if (((comp[i].buf[n * stride + rel_pos])&0xFFFFFF00) != 0U) {
					printf("Overflow !!!\n");
				}
				buffered_row[buffpos + i] = (unsigned char)(comp[i].buf[n*stride + rel_pos]);
			}
		}
		//Fill a line of the 
		bmp_out__put_line(&out, buffered_row);
	}
	bmp_out__close(&out);
	delete[] buffered_row;
	return;
}

void IO_ImageAdd(io_image *Input_Image, int val){
	if ((Input_Image == NULL) || (Input_Image->comps == NULL)) {
		//Need to free memory elegantly
		fprintf(stderr, "EMPTY BUFFER\n");
		return;
	}
	int stride = Input_Image->comps[0].stride;
	int sum = 0;
	for (int row=0; row < Input_Image->comps[0].height; row++) {
		for (int col = 0; col < Input_Image->comps[0].width; col++) {
			sum = Input_Image->comps[0].buf[(row * stride) + col] + val;
			if (sum <= 255) {
				//no overrun occured
				Input_Image->comps[0].buf[(row * stride) + col] = sum;
			}
		}

	}
	return;

}

void IO_ImageDelete(io_image *Input_Image) {
	for (int i = 0; i < Input_Image->num_components; i++) {
		delete[] Input_Image->comps[i].handle;
	}
	delete[] Input_Image->comps;
	delete Input_Image;
	return;
}
