./initialize.sh
./bootload.sh
./kernel.sh
gcc -o loadFile loadFile.c
./shell.sh
gcc -o loadFile loadFile.c
./format.sh


