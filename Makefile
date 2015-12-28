release:
	gcc main.c -lm -ofb;
run: release
	./fb
clean:
	rm fb;
