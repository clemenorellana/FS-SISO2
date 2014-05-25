#define BLOCK_SIZE 512
#define DIRECTORY_ENTRY_COUNT 16
#define IS_DIRECTORY 1
#define IS_FILE 2
#define MAGIC_NUMBER 1837083 // valor en decimal para FS (magigcNumber)

typedef struct{
    int magicNumber;
    int totalBlocks;
	int totalFreeBlocks;
	int firstFreeBlock;
	int lastFreeBlock;
	char wastedSpace[492];	
}superBlock;

typedef struct{
	char fileName[20];
	int fileType;	
	int fileBlockStart;
	int fileBlockSize;
} directoryEntry;

typedef struct {
    directoryEntry entries[DIRECTORY_ENTRY_COUNT];
}directoryBlock ;

typedef struct 
{
	unsigned int nextFreeBlock;
	char wastedSpace[508];
}unusedBlock;


directoryBlock createEmptyDirectory();