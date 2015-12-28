release:
	gcc main.c -lm -ofb.bin;
run: release
	./fb.bin
clean:
	rm fb.bin;
