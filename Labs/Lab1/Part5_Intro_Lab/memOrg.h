
#ifndef MEMORG_H
#define MEMORG_H

//typedef struct io_comp;
//typedef struct io_image;

typedef struct io_comp {
	int width, height, stride;
	int *buf, *handle;
}io_comp;

typedef struct io_image {
	int num_components;
	io_comp *comps;
}io_image;

io_image *IO_ImageNew(char* fname);
void IO_ImageWriteBMP(io_image* Input_Image, char* fname);
void IO_ImageAdd(io_image* Input_Image, int val);
void IO_ImageDelete(io_image* Input_Image);
#endif