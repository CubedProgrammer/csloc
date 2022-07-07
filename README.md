# Count Source Lines Of Code
## Counts lines of code of certain file types in a directory in Linux
Simply compiling the C files directly will do.

Theoretically, version 1.8.2 and older work on windows.

To install, simply copy the executable file to /usr/bin or /usr/local/bin on BSD, Linux, or OS X.

On Microsoft Windows, there is no folder for storing executables, unless you count System32.

To install on Microsoft Windows, copy the executable file to a folder in the PATH enviornment variable.

You may need to modify the PATH variable in the settings and create a new directory if there is no suitable directory.

You may put the executable in a system directory such as System32, and that may work, but is inadvisable.
```
clang -O3 -c csloc.c get_sub_dir.c main.c
clang -o csloc csloc.o get_sub_dir.o main.o
```
This is a command line tool, it has no GUI of any sort.

Run with no args to get a help screen.

This repository can now be configured to be built into a shared library, include csloc.h in your code to use it.
```
clang -O3 -c csloc.c get_sub_dir.c -fPIC
clang -shared -o libcsloc csloc.o get_sub_dir.o
```
