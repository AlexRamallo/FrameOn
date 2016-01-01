#ifndef __FRAME_ON___H_
#define __FRAME_ON___H_
#include <linux/fb.h>

#define loadFramebuffer frameon_loadFramebuffer
#define getLocation frameon_getLocation
#define setPixel frameon_setPixel
#define drawImage frameon_drawImage
#define clearBufferColor frameon_clearBufferColor
#define clearBuffer frameon_clearBuffer
#define swapBuffer frameon_swapBuffer
#define cleanUpBuffers frameon_cleanUp

//Framebuffer data
int frameon_fbfd;
struct fb_var_screeninfo frameon_vinfo;
struct fb_fix_screeninfo frameon_finfo;
long int frameon_screensize;
int frameon_bytes_per_pixel;
//Pointer to mmap'd framebuffer data
char *frameon_fbp;
//Pointer to back buffer (if used)
char *frameon_bbp;
char frameon_usebb;
//Target buffer
char *frameon_tbuf;

typedef void (*frameon_setpixelwbd)(
				long int location,
				int red,
				int green,
				int blue,
				int alpha);

void frameon_sp_16bpp(long int,int,int,int,int);
void frameon_sp_32bpp(long int,int,int,int,int);
void frameon_sp_24bpp(long int,int,int,int,int);

//Simple representation of images
typedef struct {
	char *data;
	int width;
	int height;
} foImage;

/**
 * Loads and initializes the specified framebuffer
 *
 * fbuf should be a string pointing to the desired
 * framebuffer device (e.g. /dev/fb0)
 * */
int frameon_loadFramebuffer(const char *fbuf, char usebb);

/**
 * Get the location of a pixel in the framebuffer
 *
 * Returns an int which is to be used as an offset
 * with pointer arithmetic	
 * */
long int frameon_getLocation(int x, int y);

/**
 * Sets the color of a pixel in the framebuffer (fbp)
 *
 * location must be the correct offset obtained from
 * getLocation
 * */
frameon_setpixelwbd frameon_setPixel;

/**
 * Draw an image onto the framebuffer at desired position
 *
 * data is the raw image data (probably from stbi_load)
 * width and height are the dimensions of the image data
 *
 * if clip is '1', image data will be 'clipped' to fit in
 * screen bounds, otherwise it will be copied as-is
 * */
int frameon_drawImage(int x, int y, foImage *img, char clip);

/**
 * Clear the framebuffer to a set color
 * */
void frameon_clearBufferColor(int r, int g, int b, int a);

/**
 * Clear the framebuffer to black
 * */
void frameon_clearBuffer();

/**
 * Swap the back and front buffers
 *
 * OR do nothing if double buffering is disabled.
 * */
void frameon_swapBuffer();

/**
 * Clean up by unmapping and freeing all buffers
 * */
void frameon_cleanUp();
#endif


