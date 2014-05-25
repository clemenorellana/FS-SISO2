as86 lib.asm -o lib_asm.o
bcc -ansi -c lib.c -o lib.o
bcc -ansi -c basicfs.c -o basicfs.o
bcc -ansi -c format.c -o format.o
ld86 -0 -d format.o lib.o lib_asm.o
mv a.out FORMAT
./loadFile FORMAT
mv FORMAT format
