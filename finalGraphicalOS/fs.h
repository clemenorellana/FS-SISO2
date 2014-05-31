#include "basicfs.h"
#include "utilities.h"

int mod2(int a, int b);
int div2(int a, int b);
void readsector2(int sectornumber, char* buffer);
void writesector2(int sectornumber, char* buffer);
directoryBlock loadRootDirectory();
directoryBlock createEmptyDirectory();
int findFileInDirectory(directoryBlock directory, char *fileName);
int findFileBlockInPath(char *path);
superBlock loadSuperBlock();
unsigned int allocateBlock();
void createDir(char *path, char *directoryName);
directoryBlock listDir(char *filePath);