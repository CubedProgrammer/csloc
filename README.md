# Count Source Lines Of Code
## Counts lines of code of certain file types in a directory in Linux
Simply compiling the C files directly will do.

To install, simply copy the executable file to /usr/bin or /usr/local/bin on BSD, Linux, or OS X.

On Windows, there is no predefined folder for storing executables, unless you count System32 and related.

To install on Windows, copy the executable file to a folder in the PATH enviornment variable.

You may need to modify the PATH variable in the settings and create a new directory if there is no suitable directory.

You may put the executable in a system directory such as System32, and that may work, but is inadvisable.

Having symbolic links may cause an infinite loop. There is an option to disable them altogether though.

If original directory to count is ~/u, and ~/u/v is a symbolic link to ~/v, and ~/v/x/y is a symbolic link to ~/v, there will be a loop.

However, if all symbolic links to the files in the same directory, then there will be no infinite loop, and no file is counted twice.
## Examples
```
csloc -qt folder -x h c
```
Count all the .h and .c files in folder, and display each file's counts, sorted greatest to least.
```
csloc -flry folder -x hpp cpp
```
Count all the .hpp and .cpp files in folder, and display the file size of each file (-f option), sorted least to greatest (-r option), with complete sentences.
Then gets the total size of all .hpp files and the total for .cpp files, displays both, from -y option.
Treats symbolic links as regular files, from -l option.
## Compilation
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
