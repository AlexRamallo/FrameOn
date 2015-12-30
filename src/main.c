#include<stdio.h>
#include<stdlib.h>
#include "frameon.h"
#include "frameon_extras.h"
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
		fprintf(stderr,"Framebuffer init failure code: %i\n",res);
		return res;
	}

	char done = 0;
	int x = 0;

	foImage *image = imgLoad(img);
	if(image == NULL){
		fprintf(stderr, "Failed to load image %c\n", img);
		return 1;
	}
	convert_swapRedBlue(image);
	while(done==0){
		x += 20;

		clearBuffer();
		drawImage(x, 100, image);
		swapBuffer();

		if(x > 1000)
			done = 1;
	}
	cleanUpImage(image);
	cleanUpBuffers();
	return 0;
}







