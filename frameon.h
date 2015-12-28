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
		int r, int g, int b,
		struct fb_var_screeninfo *v,
		struct fb_fix_screeninfo *f){
	switch(v->bits_per_pixel){
		default://Assume 16bpp...
			//int b = 16;
			//int g = 0;
			//int r = 0;
			//unsigned short int t = 0<<11 | 0<<5 | 16;
			*((unsigned short int*)(fbp + location)) = 
				0<<r | 0<<g | b;
			break;
		case 32:
			*(fbp + location) = b;
			*(fbp + location + 1) = g; //g
			*(fbp + location + 2) = r; //r
			*(fbp + location + 3) = 0; //b
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

	for(dy = 0; dy<height; dy++)
		for(dx = 0; dx<width; dx++){
			int loc = getLocation(x+dx,y+dy,v,f);
			int ploc = (width * 3 * dy) + (dx * 3);
			setPixel(fbp,loc,
				*(data + ploc),
				*(data + ploc + 1),
				*(data + ploc + 2),
				v,f);
		}
	return 0;
}
#endif


