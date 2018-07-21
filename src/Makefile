testmem: testmem.o libmem.so
	gcc testmem.c -lmem -L. -o myprogram
libmem.so: mem.o
	gcc -c -fpic mem.c
	gcc -shared -o libmem.so mem.o
clean:
	rm *.o *.so myprogram
