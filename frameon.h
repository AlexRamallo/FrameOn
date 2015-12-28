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
	unsigned char *data = stbi_load(filename, &width, &height, &comp, 3);
	if(data == NULL){
		return 1;
	}
	//fprintf(stdout, "actual comp#: %i\n", comp);
	int dx = 0, dy = 0;
	int *readbuf = (int*)malloc(sizeof(int)*4);
	for(dy = 0; dy<height; dy++)
		for(dx = 0; dx<width; dx++){
			int loc = getLocation(x+dx,y+dy,v,f);
			int ploc = (width * comp * dy) + (dx * comp);
			int i;
			for(i = 0; i<4; i++)
				if(i<comp)
					readbuf[i] = *(data+ploc+i);
				else
					readbuf[i] = 0;

			setPixel(fbp,loc,
				readbuf[0],
				readbuf[1],
				readbuf[2],
				readbuf[3],
				v,f);
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
			struct fb_var_screeninfo *v,
			struct fb_fix_screeninfo *f){
	clearBufferColor(fbp,0,0,0,0,v,f);
}
#endif


