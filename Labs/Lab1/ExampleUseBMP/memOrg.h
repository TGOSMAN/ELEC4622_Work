
#ifndef MEMORG_H
#define MEMORG_H


typedef struct io_comp {
	int width, height, stride;
	int *buf, *handle;
}io_comp;

typedef struct io_image {
	int num_components;
	io_comp* comps;
}io_image;

io_image IO_ImageNew(char* fname);
#endif