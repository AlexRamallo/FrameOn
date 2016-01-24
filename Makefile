release: frameon
	gcc -lfreetype src/main.c -I./headers -I/usr/include/freetype2 -L./lib -lframeon -lm -o bin/fb;
frameon:
	gcc -c src/frameon.c -I./headers -o bin/frameon.o;
	ar rcs lib/libframeon.a bin/frameon.o;
clean:
	rm -rf bin;
	mkdir bin;
