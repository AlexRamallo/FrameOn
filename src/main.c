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
					128*64,
					64*64,
					0,
					300);
	
	//error = FT_Set_Pixel_Sizes(face, 64,32);

	int a = FT_Get_Char_Index(face, 'A');

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
	c.data = (char*)malloc(bmp.width*bmp.rows*4);
	c.width = bmp.width;
	c.height = bmp.rows;

	int x,y;
	for(x=0; x<c.width; x+=4){
		for(y=0; y<c.height; y++){
			c.data[x + (c.width*4 * y)] = bmp.buffer[x+(c.width*y)];
			c.data[x + (c.width*4 * y)+1] = bmp.buffer[x+(c.width*y)];
			c.data[x + (c.width*4 * y)+2] = bmp.buffer[x+(c.width*y)];
			c.data[x + (c.width*4 * y)+3] = 0;
		}
	}

	fprintf(stdout, "glyp width: %i, height: %i\n",bmp.width,bmp.rows);

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

	int res = loadFramebuffer(fbf, false);
	if(res!=0){
		fprintf(stderr,"Framebuffer init failure code: %i\n",res);
		return res;
	}

	fprintf(stdout, "xres/yres: %i/%i\nvxres/vyres: %i/%i\n",
				frameon_vinfo.xres,
				frameon_vinfo.yres,
				frameon_vinfo.xres_virtual,
				frameon_vinfo.yres_virtual);	
	
	drawImage(20, 20, &c, true);
	//cleanUpImage(c);
	cleanUpBuffers();
	return 0;
}







