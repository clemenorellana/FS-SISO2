#include "basicfs.h"
//#include "lib.h"
#include <stdio.h>

#define FS_STAR 15 
#define SUPER_BLOCK_POS 15
#define ROOT_POS 16

void writeBlock(FILE *deviceFile, int blockNumber, void * blockData)
{
	fseek(deviceFile,blockNumber*BLOCK_SIZE,SEEK_SET);
	fwrite(blockData,1,BLOCK_SIZE,deviceFile);
}


void format(char *deviceName)
{
	//open the floppy image
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
	mysuperBlock.firstFreeBlock = (totalBlocksFloppyHeader-1) + 2;  // 2 porque van a estar el SuperBlock y luego el root
	mysuperBlock.lastFreeBlock = totalBlocksFloppy - 1;

	int blockNumber;
	for(blockNumber = mysuperBlock.firstFreeBlock;blockNumber<mysuperBlock.lastFreeBlock;blockNumber++)
	{
		unusedBlock freeBlock;
		freeBlock.nextFreeBlock = blockNumber +1;
		writeBlock(deviceFile,blockNumber,&freeBlock);
		//fseek(deviceFile,blockNumber*BLOCK_SIZE,SEEK_SET);
		//fwrite(freeBlock,1,BLOCK_SIZE,deviceFile);
	}

	unusedBlock lastFreeBlock;
	lastFreeBlock.nextFreeBlock=0;
	writeBlock(deviceFile,mysuperBlock.lastFreeBlock,&lastFreeBlock);
	//fseek(deviceFile,mysuperBlock.lastFreeBlock*BLOCK_SIZE,SEEK_SET);
	//fwrite(lastFreeBlock,1,BLOCK_SIZE,deviceFile);

	directoryBlock rootDirectory = createEmptyDirectory();
    writeBlock(deviceFile,ROOT_POS,&rootDirectory);
    //fseek(deviceFile,ROOT_POS*BLOCK_SIZE,SEEK_SET);
	//fwrite(rootDirectory,1,BLOCK_SIZE,deviceFile);
    
	writeBlock(deviceFile,SUPER_BLOCK_POS,&mysuperBlock);
	//fseek(deviceFile,SUPER_BLOCK_POS*BLOCK_SIZE,SEEK_SET);
	//fwrite(mysuperBlock,1,BLOCK_SIZE,deviceFile);
    
}

 main(int argc, char* argv[])
{
	format("floppya.img");

	//return 0;
}