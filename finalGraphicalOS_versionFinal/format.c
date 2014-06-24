
#include <stdio.h>

#define FS_STAR 34//15 
#define SUPER_BLOCK_POS 34//15
#define ROOT_POS 35//16


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

directoryBlock createEmptyDirectory()
{
	directoryBlock newDirectory;
	int i = 0;
	while(i<DIRECTORY_ENTRY_COUNT)
	{
		newDirectory.entries[i].fileBlockStart = 0;
		newDirectory.entries[i].fileBlockSize = 0;
		i++;
	}
	return newDirectory;
}


void writeBlock(FILE *deviceFile, int blockNumber, void * blockData)
{
	fseek(deviceFile,blockNumber*BLOCK_SIZE,SEEK_SET);
	fwrite(blockData,1,BLOCK_SIZE,deviceFile);
}


void format(char *deviceName)
{
	FILE * deviceFile;
	deviceFile=fopen(deviceName,"rw+");
	if (deviceFile==0)
	{
		printf("floppya.img not found\n");
		return;
	}

	fseek(deviceFile,0,SEEK_END);
	unsigned int totalBlocksFloppy = ftell(deviceFile)/BLOCK_SIZE;

	fseek(deviceFile,BLOCK_SIZE*FS_STAR,SEEK_SET);
	unsigned int totalBlocksFloppyHeader = ftell(deviceFile)/BLOCK_SIZE; //hasta donde termina el kernel

	superBlock mysuperBlock;
	mysuperBlock.magicNumber = MAGIC_NUMBER;
	mysuperBlock.totalBlocks = totalBlocksFloppy - totalBlocksFloppyHeader;
	mysuperBlock.totalFreeBlocks = mysuperBlock.totalBlocks  - 2; // 2 porque van a estar el SuperBlock y luego el root
	mysuperBlock.firstFreeBlock = totalBlocksFloppyHeader + 2;  // 2 porque van a estar el SuperBlock y luego el root
	mysuperBlock.lastFreeBlock = totalBlocksFloppy - 1;

	printf("Total Blocks: ");
	printf("%d \n", mysuperBlock.totalBlocks);

	printf("Total Free Blocks: ");
	printf("%d \n", mysuperBlock.totalFreeBlocks);

	printf("First free Blocks: ");
	printf("%d \n", mysuperBlock.firstFreeBlock);

	printf("last free Blocks: ");
	printf("%d \n", mysuperBlock.lastFreeBlock);

	int blockNumber;
	for(blockNumber = mysuperBlock.firstFreeBlock; blockNumber<mysuperBlock.lastFreeBlock; blockNumber++)
	{
		unusedBlock freeBlock;
		freeBlock.nextFreeBlock = blockNumber +1;
		writeBlock(deviceFile,blockNumber,&freeBlock);
	}

	unusedBlock lastFreeBlock;
	lastFreeBlock.nextFreeBlock=0;
	writeBlock(deviceFile,mysuperBlock.lastFreeBlock,&lastFreeBlock);

	directoryBlock rootDirectory = createEmptyDirectory();
    writeBlock(deviceFile,ROOT_POS,&rootDirectory);
    
	writeBlock(deviceFile,SUPER_BLOCK_POS,&mysuperBlock);
	
	fclose(deviceFile);
    
}

 main(int argc, char* argv[])
{
	format("floppya.img");

	//return 0;
}