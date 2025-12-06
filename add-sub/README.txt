Static Library
gcc -c [.c] -o [file.name]
ar -r [lib+Name.a] [.o] [.o]
gcc [.c] [.a] -o [file.name]


Shared Library
gcc -c -fpic [.c]
gcc -shared [.o] [.o] ..... -o [lib+Name.so]
gcc [.c] -o [name] -l[Name] -L[library_address]

!! export LD_LIBRARY_PATH = library_address
