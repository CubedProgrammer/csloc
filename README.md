# Count Source Lines Of Code
## Counts lines of code of a directory in Linux
Simply compiling the C files directly will do. The older versions may work on windows.

This repository can now be configured to be built into a shared library, include csloc.h in your code to use it.
```sh
gcc -O3 -c csloc.c -fPIC
gcc -O3 -c get_sub_dir.c -fPIC
gcc -shared -o libcsloc csloc.o get_sub_dir.o
```
