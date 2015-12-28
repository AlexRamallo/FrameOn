#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<linux/fb.h>
#include<sys/mman.h>
#include<sys/ioctl.h>
#include "frameon.h"
#include <math.h>

int fbfd = 0;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
long int screensize = 0;
char *fbp = 0;

int initializeFramebuffer(const char *fbf){
	fbfd = open(fbf, O_RDWR);
	if(fbfd == -1)
		return 1;
	if(ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1)
		return 2;
	if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1)
		return 3;

	screensize = vinfo.xres * vinfo.yres * (vinfo.bits_per_pixel / 8);

	fbp = (char*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	if((int)fbp == -1)
		return 4;
	return 0;
}

int main(int argc, char **argv){
	const char *fbf = NULL;
	const char *img = NULL;
	if(argc == 2){
		img = argv[1];
	}else if(argc == 3){
		fbf = argv[1];
		img = argv[2];
	}else{
		fprintf(stdout, "USAGE:\n%c [/dev/fb*] <image>\n",argv[0]);
		return 0;
	}

	int res = initializeFramebuffer("/dev/fb1");
	if(res!=0){
		fprintf(stderr, "Error initializing framebuffer! code: %i\n", res);
		return res;
	}

	/**
 	*	READY TO START WRITING TO FRAMEBUFFER
 	* 	use:
 	* 		getLocation(x, y, &vinfo, &yinfo);
 	* 	with:
 	* 		setPixel(fbp,location,r,g,b,&vinfo,&yinfo);
 	*	to do it!
	*/
	if(drawImage(fbp, 10, 10, img, &vinfo, &finfo)!=0){
		fprintf(stderr, "Failed to load image.\n");
		return 5;
	}

	munmap(fbp, screensize);
	close(fbfd);
	return 0;
}
