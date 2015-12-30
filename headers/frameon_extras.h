#ifndef __FRAMEON____EXTRAS__H____
#define __FRAMEON____EXTRAS__H____
#include "frameon.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/**
 * Loads an image and returns an foImage
 * */
foImage *imgLoad(const char *file){
	foImage *i = (foImage*)malloc(sizeof(foImage));
	i->data = stbi_load(file,
				&(i->width),
				&(i->height),
				NULL, 4);
	return i;
}

/**
 * Swaps the values of red and blue pixels in an image
 * */
void convert_swapRedBlue(foImage *img){
	int lsize = (img->width * 4);
	int x,y;
	for(y=0; y<img->height; y++)
		for(x=0; x<img->width; x++){
			int loc = (lsize*y)+(x*4);
			int rb = *(img->data+loc);
			*(img->data+loc) = *(img->data+loc+2);
			*(img->data+loc+2) = rb;
		}
}
#endif
