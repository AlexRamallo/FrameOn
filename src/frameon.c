#include "frameon.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <errno.h>

#define true 1
#define false 0

int frameon_putvinfo(struct fb_var_screeninfo *put){
	int res = ioctl(frameon_fbfd, FBIOPUT_VSCREENINFO, put);
	//error handling/reporting
	return res;
}
int frameon_updateFBInfo(char remap){
	int fres,vres,rres;

	fres = ioctl(frameon_fbfd,
			FBIOGET_FSCREENINFO,
			&frameon_finfo);

	vres = ioctl(frameon_fbfd,
			FBIOGET_VSCREENINFO,
			&frameon_vinfo);
	if(fres!=0)
		return fres;
	if(vres!=0)
		return vres;

	if(remap == 1){
		if(frameon_fbp!=NULL)
			munmap(frameon_fbp, frameon_screensize);

		frameon_bytes_per_pixel =
				frameon_vinfo.bits_per_pixel/8;	

		frameon_screensize =	frameon_vinfo.xres
					*frameon_vinfo.yres
					*frameon_bytes_per_pixel;

		frameon_fbp = (char*)mmap(0,
					frameon_screensize,
					PROT_READ | PROT_WRITE,
					MAP_SHARED, frameon_fbfd, 0);
		rres = 0;
		if(frameon_fbp == NULL)
			rres = 1;
	}

	return 0;
}

int frameon_setVirtualResolution(int width, int height){
	frameon_vinfo.xres_virtual = width;
	frameon_vinfo.yres_virtual = height;
	int setRes = frameon_putvinfo(&frameon_vinfo);
	frameon_updateFBInfo(1);
	return setRes;
}

int frameon_setScreenResolution(int width, int height){
	frameon_vinfo.xres = width;
	frameon_vinfo.yres = height;
	int setRes = frameon_putvinfo(&frameon_vinfo);
	frameon_updateFBInfo(1);
	return setRes;
}

int frameon_loadFramebuffer(const char *fbuf, char useBackbuffer){
	frameon_fbp = NULL;
	int fd = open(fbuf, O_RDWR);		
	frameon_fbfd = fd;
	if(fd == -1)
		return 1;
	frameon_updateFBInfo(0);

	frameon_usebb = useBackbuffer;
	if(frameon_usebb == true){
		//Setup double buffering...
		if(frameon_setVirtualResolution(
				frameon_vinfo.xres,
				frameon_vinfo.yres*2) == 0){
			
			//use offset method
			frameon_tbuf = frameon_fbp;
			frameon_bbp = NULL;
			fprintf(stdout,
				"Double buffering method: 1\n");
		}else{
			//try to (m)allocate the back buffer
			frameon_bbp = (char*)malloc(frameon_screensize);
			if(frameon_bbp == NULL){
				//Failed set up double buffering
				frameon_usebb = false;
				frameon_tbuf = frameon_fbp;
				fprintf(stdout,
					"Double buffering method: 0\n");
			}else{
				frameon_tbuf = frameon_bbp;
				fprintf(stdout,
					"Double buffering method: 2\n");
			}
		}
	}else
		frameon_bbp = NULL;	
	//Update fb with any double buffering changes, and mmap it
	if(frameon_updateFBInfo(1)!=0)
		return 4;

	if(frameon_bbp == NULL)
		frameon_tbuf = frameon_fbp;

	switch(frameon_vinfo.bits_per_pixel){
		default:
			frameon_setPixel = frameon_sp_16bpp;
			break;
		case 32:
			frameon_setPixel = frameon_sp_32bpp;
			break;
		case 24:
			frameon_setPixel = frameon_sp_24bpp;
			break;
	}
	return 0;
}

long int frameon_getLocation(int x,int y){
	int yoff = frameon_vinfo.yoffset;
	if(frameon_usebb == true && frameon_bbp == NULL){
		if(yoff == 0)
			yoff = frameon_vinfo.yres;
		else
			yoff = 0;
	}
	//fprintf(stdout, "gloc:yoff = %i\n", yoff);
	return (x+frameon_vinfo.xoffset) * frameon_bytes_per_pixel
		+ (y+yoff) * frameon_finfo.line_length;
}

int frameon_drawImage(int x, int y, foImage *img, char clip){
	if(img == NULL)
		return 1;
	int hclip = img->height;
	if(clip == 1 && hclip+y>frameon_vinfo.yres)
		hclip = frameon_vinfo.yres - y;
	int wclip = img->width;
	if(clip == 1 && wclip+x>frameon_vinfo.xres)
		wclip = frameon_vinfo.xres - x;

	int linesize = wclip*4;
	int loc,ry;
	for(ry=0; ry<hclip; ry++){
		loc = getLocation(x,y+ry);
		memcpy(	frameon_tbuf+loc,
			img->data+(img->width*4*ry),linesize);
	}
	return 0;
}
void frameon_clearBufferColor(int r, int g, int b, int a){
	int x, y;
	for(y = 0; y<frameon_vinfo.yres; y++)
		for(x=0; x<frameon_vinfo.xres; x++)
			setPixel(getLocation(x,y),r,g,b,a);
}
void frameon_clearBuffer(){
	memset(frameon_tbuf, 0, frameon_screensize);
}
void frameon_swapBuffer(){
	if(frameon_usebb == false)
		return;
	if(frameon_bbp==NULL){
		if(frameon_vinfo.yoffset == 0)
			frameon_vinfo.yoffset = frameon_vinfo.yres;
		else
			frameon_vinfo.yoffset = 0;
		ioctl(frameon_fbfd, FBIOPAN_DISPLAY, &frameon_vinfo);
	}else{
		memcpy(frameon_fbp, frameon_tbuf, frameon_screensize);
	}	
}
void frameon_cleanUp(){
	munmap(frameon_fbp, frameon_screensize);
	free(frameon_bbp);
	close(frameon_fbfd);
}
void frameon_sp_16bpp(long int l, int r, int g, int b, int a){
	*((fu16*)(frameon_tbuf+l)) = 0<<r | 0<<g | b;
}
void frameon_sp_24bpp(long int l, int r, int g, int b, int a){
	*((fu8*)(frameon_tbuf+l)) = b;
	*((fu8*)(frameon_tbuf+l+1)) = g;
	*((fu8*)(frameon_tbuf+l+2)) = r;
}
void frameon_sp_32bpp(long int l, int r, int g, int b, int a){
	*((fu8*)(frameon_tbuf+l)) = b;
	*((fu8*)(frameon_tbuf+l+1)) = g;
	*((fu8*)(frameon_tbuf+l+2)) = r;
	*((fu8*)(frameon_tbuf+l+3)) = a;
}
