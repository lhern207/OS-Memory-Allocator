-Four files are included in the 'Code' directory:
	mem.c - file containing all library functions.
        mem.h - header file containing prototypes of all functions.
	testmem.c - Test file for the library functions.
        Makefile - To compile the code. 
-Environment variable 'LD_LIBRARY_PATH' must be set to library directory before executing program.
-Code can be compiled using makefile provided:
	-"make libmem.so" will compile the shared library file 'libmem.so' from 'mem.c', and 'mem.h'.
	-"make" will compile both the library file 'libmem.so' (from mem.c), 
  		and the testfile 'testmem.c' into an executable file called 'myprogram',
                which serves as a test of the library's basic functionalities.
	-'myprogram' can be run just like any other program executable from the command line.
	-"make clean" removes all object files, the library file (.so), and 'myprogram' executable.

*The allocation policy tested by 'testmem.c', and 'myprogram' is Best-Fit. To test other policies,
 one can simply change the policy in 'testmem.c', or just create another testfile. I have included
 screenshots of the output for each policy using 'testmem.c'.
