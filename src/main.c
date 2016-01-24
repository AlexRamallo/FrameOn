#include <stdio.h>
#include <stdlib.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "frameon.h"
#include "frameon_extras.h"
#define true 1
#define false 0

int main(int argc, char **argv){

	FT_Library library;
	int error = FT_Init_FreeType(&library);
	if(error!=0){
		fprintf(stderr, "Failed to init FreeType\n");
		return 1;
	}
	
	FT_Face face;

	error = FT_New_Face	(library,
				"/home/alex/fonts/arial.ttf",
				0,
				&face);
	if(error!=0){
		fprintf(stderr, "Failed to load font.\n");
		return 1;
	}

	error = FT_Set_Char_Size(
					face,
					0,
					64*64,
					0,
					128);
	
	int a = FT_Get_Char_Index(face, '&');

	error = FT_Load_Glyph(
			face,
			a,
			0);
	if(error!=0){
		fprintf(stderr, "Couldn't load glyph.\n");
		return 1;
	}
	if(face->glyph->format!=FT_GLYPH_FORMAT_BITMAP){
		fprintf(stdout, "glyph is not bitmap; converting...\n");
		error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		if(error!=0){
			fprintf(stderr, "\tthe conversion failed!\n");
			return 1;
		}
	}
	fprintf(stdout, "glyph conversion complete\n.");
	
	FT_Bitmap bmp = face->glyph->bitmap;
	foImage c;
	c.data = bmp.buffer;
	c.width = bmp.width;
	c.height = bmp.rows;

	fprintf(stdout, "cw: %i, ch: %i\n", c.width, c.height);
	
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

	int res = loadFramebuffer(fbf, true);
	if(res!=0){
		fprintf(stderr,"Framebuffer init failure code: %i\n",res);
		return res;
	}

	fprintf(stdout, "xres/yres: %i/%i\nvxres/vyres: %i/%i\n",
				frameon_vinfo.xres,
				frameon_vinfo.yres,
				frameon_vinfo.xres_virtual,
				frameon_vinfo.yres_virtual);	
	

	char done = 0;
	int x = 0;

	foImage *image = imgLoad(img);
	if(image == NULL){
		fprintf(stderr, "Failed to load image %c\n", img);
		return 1;
	}
	convert_swapRedBlue(image);
	while(done == false){
		x += 1;
		clearBuffer();
		drawImage(x, 100, &c, true);
		swapBuffer();
		if(x >= SCREENWIDTH - image->width)
			done = true;
	}
	//sleep(10000);

	cleanUpImage(image);
	cleanUpBuffers();
	return 0;
}







