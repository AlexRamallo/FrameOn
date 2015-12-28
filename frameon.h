#ifndef __FRAME_ON___H_
#define __FRAME_ON___H_
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

long int getLocation(	int x,
			int y,
			struct fb_var_screeninfo *v,
			struct fb_fix_screeninfo *f){
	return (x+v->xoffset) * (v->bits_per_pixel/8)
		+ (y+v->yoffset) * f->line_length;
}

void setPixel(	char *fbp,
		long int location,
		int r, int g, int b, int a,
		struct fb_var_screeninfo *v,
		struct fb_fix_screeninfo *f){
	switch(v->bits_per_pixel){
		default://Assume 16bpp...
			*((unsigned short int*)(fbp + location)) = 
				0<<r | 0<<g | b;
			break;
		case 32:
			*(fbp + location) = b;
			*(fbp + location + 1) = g;
			*(fbp + location + 2) = r;
			*(fbp + location + 3) = a;
			break;
		case 24:
			*(fbp + location) = b;
			*(fbp + location + 1) = g;
			*(fbp + location + 2) = r;
			break;
	}
}

int drawImage(char *fbp, int x, int y, const char *filename,
		struct fb_var_screeninfo *v,
		struct fb_fix_screeninfo *f){
	int width, height, comp;
	unsigned char *data = stbi_load(filename, &width, &height, &comp, 0);
	if(data == NULL){
		return 1;
	}
	int linesize = width*comp;
	int loc,ry;
	for(ry=0; ry<height; ry++){
		loc = getLocation(x,ry,v,f);
		memcpy(fbp+loc, data+(linesize*ry), linesize);
	}
	return 0;
}
void clearBufferColor(	char *fbp,
			int r, int g, int b, int a,
			struct fb_var_screeninfo *v,
			struct fb_fix_screeninfo *f){
	int x, y;
	for(y = 0; y<v->yres; y++)
		for(x=0; x<v->xres; x++){
			setPixel(fbp,
				getLocation(x,y,v,f),
				r,g,b,a,v,f);
		}
}
void clearBuffer(	char *fbp,
			int screensize){
	memset(fbp, 0, screensize);
}
#endif


