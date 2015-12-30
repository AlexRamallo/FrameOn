Just me playing around with the framebuffer on Linux.

Right now all there is is a basic API for writing directly to the linux framebuffer.

Use by including `frameon.h` and `frameon_extras.h` into your program.

Then,

* Step 1: Initialize by calling `loadFramebuffer(framebuffer, doublebuffering?);` where the first argument is the framebuffer device to use (e.g. "/dev/fb0") and the second is `1` if you wish to enable double buffering, `0` otherwise.

* Step 2: Use any of the functions in `frameon.h` to start drawing.


Headers are documented and `main.c` is an example program
