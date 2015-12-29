#include<stdio.h>
#include<stdlib.h>
#include "frameon.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define true 1
#define false 0

int main(int argc, char **argv){
	const char *fbf = NULL;
	const char *img = NULL;
	if(argc == 2){
		img = argv[1];
		fbf = "/dev/fb1";
	}else if(argc == 3){
		fbf = argv[1];
		img = argv[2];
	}else{
		fprintf(stdout, "USAGE:\n%c [/dev/fb*] <image>\n",argv[0]);
		return 0;
	}

	int res = loadFramebuffer(fbf, false);
	if(res!=0){
		fprintf(stderr, "Error initializing framebuffer! code: %i\n", res);
		return res;
	}

	char done = 0;
	int x = 0;

	int iw,ih,cmp;
	unsigned char *image = stbi_load(img, &iw, &ih, &cmp, 4);
	if(image == NULL){
		fprintf(stderr, "Failed to load image %c\n", img);
		return 1;
	}
	while(done==0){
		x += 20;

		clearBuffer();
		drawImage(x, 100, image, iw, ih);	
		swapBuffer();

		if(x > 1000)
			done = 1;
	}
	cleanUpBuffers();
	return 0;
}







