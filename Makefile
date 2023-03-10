all:build

build: tema3.c
	gcc -Wall tema3.c -o bmp

run:
	./bmp

clean:
	rm -f *.o bmp