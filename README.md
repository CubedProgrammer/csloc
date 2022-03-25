# Count Source Lines Of Code
## Counts lines of code of certain file types in a directory in Linux
Simply compiling the C files directly will do. The older versions may work on windows.
```
gcc -O3 -c csloc.c get_sub_dir.c main.c
gcc -static -o csloc csloc.o get_sub_dir.o main.o
```
This is a command line tool, it has no GUI of any sort.

Run with no args to get a help screen.

This repository can now be configured to be built into a shared library, include csloc.h in your code to use it.
```
gcc -O3 -c csloc.c get_sub_dir.c -fPIC
gcc -shared -o libcsloc csloc.o get_sub_dir.o
```
