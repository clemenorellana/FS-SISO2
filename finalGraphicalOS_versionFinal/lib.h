void readsector(int, char*);
void writesector(int, char*);
void putchar(char);
char getchar();
void printstring(char*);
void printnumber(int);
void readstring(char*);
void readfile(char*, char*);
void writefile(char*, char*, int);
void deletefile(char*);
void exit();
void executeprogram(char*, int,char*);
void allow_preemption();
int mod(int,int);
int mydiv(int,int);
void setvideo(int);
void setpixel(int,int,int);
void clearscreen();
void setcursor(int,int);
void setchar(char,char,int,int);
void setstring(char*,char,int,int);
void getnumberstring(char*,int);

//funciones del FS

#define BLOCK_SIZE 512
#define FsStart 34
#define magic 1837083 //valor decimal de 'fs'
#define totalBlocksDeviceSize 2880
#define totalBlocksFS 34
#define DIRECTORY_ENTRY_COUNT 16
#define IS_DIRECTORY 1
#define IS_FILE 2
#define SUPER_BLOCK 34
#define ROOT_BLOCK 35

typedef struct
{ 
	unsigned int magicNumber;
	unsigned int totalBlocks;
	unsigned int totalFreeBlocks; 
	unsigned int firstFreeBlock; 
	unsigned int lastFreeBlock; 
	char wastedSpace[492]; 
}superBlock; 

typedef struct
{ 
	char fileName[20]; 
	unsigned int fileType; 
	unsigned int fileBlockStart; 
	unsigned int fileBlockSize; 
} directoryEntry; 

typedef struct 
{ 
	directoryEntry entries[DIRECTORY_ENTRY_COUNT]; 
}directoryBlock;

typedef struct 
{ 
	unsigned int nextFreeBlock; 
	char wastedSpace[508]; 
}unusedBlock;

typedef struct 
{
	char fileName[20];
	unsigned int type;
	unsigned int size;
	char acessRights[3];
	char data [481];
}fileBlock;


// ----------------------------- utilities ------------------------------------------
int myStrCmp (const char *s1, const char *s2); 
char * strcpy( char *to,char* from);
char * strtok(char * str, char delim, int pos);
int stlen(char * cadena, int start, char delim);
int len(char* cadena);
char * mystrcat(char * destino,char * fuente); 
int qtytoken(char * token, char delim);
void *malloc(int sz);


// ----------------------------- SuperFS ------------------------------------------
void setFreeBlock(unsigned int blockNumber); 
void loadRootDirectory(directoryBlock* directory);
void loadSuperBlock(superBlock *superBlock);
int findFileInDirectory(directoryBlock directory, char * fileName);
int findFileBlockInPath(char* path);
unsigned int AllocateBlock();

// Funciones de Directorios
void createEmptyDirectory(directoryBlock *dir);
//int createDirectory(char * filePath,char * fileName);
void listDirectory(char * filePath);
void renameDirectory(char *dirPath, char *newDirName);
void removeDirectoryCascade(int block);
void removeDirectory(char * path);
void copyDirectoryCascade(int blockSource, int blockDestino, char *pathDestino);
void copyDirectory(char *pathSource, char *pathDestino,char *nameOfCopy);
void moveDirectory(char *pathSource, char *pathDestino);

// Funciones de Archivos
void createEmptyFile(fileBlock *newFile);
int FileExist(directoryBlock dirBlock, char *fileName);
void writeNewFile(fileBlock *file, char fileData [], int blockStart);
void createFile(char * path, char* fileName, char acess[], char *data);
void removeFile(char *path, char *fileName);
void loadFile(char *path, char *fileName);