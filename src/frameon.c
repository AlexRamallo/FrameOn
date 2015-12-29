#include "frameon.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#define true 1
#define false 0
int frameon_loadFramebuffer(const char *fbuf, char useBackbuffer){
	int fd = open(fbuf, O_RDWR);		
	frameon_fbfd = fd;
	if(fd == -1)
		return 1;
	if(ioctl(fd, FBIOGET_FSCREENINFO, &frameon_finfo) == -1)
		return 2;
	if(ioctl(fd, FBIOGET_VSCREENINFO, &frameon_vinfo) == -1)
		return 3;
	
	frameon_bytes_per_pixel = frameon_vinfo.bits_per_pixel/8;
	
	frameon_screensize =	frameon_vinfo.xres
				*frameon_vinfo.yres
				*frameon_bytes_per_pixel;

	frameon_fbp = (char*)mmap(0, frameon_screensize,
					PROT_READ | PROT_WRITE,
					MAP_SHARED, fd, 0);
	if(frameon_fbp == NULL)
		return 4;
	
	frameon_usebb = useBackbuffer;
	if(frameon_usebb == true){
		frameon_bbp = (char*)malloc(frameon_screensize);
		if(frameon_bbp == NULL){
			frameon_usebb = false;
			return 5;
		}
		frameon_tbuf = frameon_bbp;
	}else{
		frameon_tbuf = frameon_fbp;
	}

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
	return (x+frameon_vinfo.xoffset) * frameon_bytes_per_pixel
		+ (y+frameon_vinfo.yoffset) * frameon_finfo.line_length;
}

int frameon_drawImage(	int x, int y,
			unsigned char *data,
			int width,
			int height){
	if(data == NULL)
		return 1;
	int linesize = width*4;
	int loc,ry;
	for(ry=0; ry<height; ry++){
		loc = getLocation(x,y+ry);
		memcpy(frameon_tbuf+loc, data+(linesize*ry), linesize);
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
	if(frameon_usebb)
		memcpy(frameon_fbp, frameon_bbp, frameon_screensize);
}
void frameon_cleanUp(){
	munmap(frameon_fbp, frameon_screensize);
	free(frameon_bbp);
	close(frameon_fbfd);
}
void frameon_sp_16bpp(long int l, int r, int g, int b, int a){
	*((unsigned short int*)(frameon_tbuf+l)) = 0<<r | 0<<g | b;
}
void frameon_sp_24bpp(long int l, int r, int g, int b, int a){
	*(frameon_tbuf+l) = b;
	*(frameon_tbuf+l+1) = g;
	*(frameon_tbuf+l+2) = r;
}
void frameon_sp_32bpp(long int l, int r, int g, int b, int a){
	*(frameon_tbuf+l) = b;
	*(frameon_tbuf+l+1) = g;
	*(frameon_tbuf+l+2) = r;
	*(frameon_tbuf+l+3) = a;
}
