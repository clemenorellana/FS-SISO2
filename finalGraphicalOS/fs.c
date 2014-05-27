#include "basicfs.h"
#include "utilities.h"

#define ROOT_BLOCK 16

char *deviceName = "floppya.img";

void writeBlock(unsigned int blockNumber, void *blockData)
{
	FILE * deviceFile;
	deviceFile=fopen(deviceName,"rw+");
	if (deviceFile==0)
	{
		printf("floppya.img not found\n");
		return;
	}
	
	fseek(deviceFile,blockNumber*BLOCK_SIZE,SEEK_SET);
	fwrite(blockData,1,BLOCK_SIZE,deviceFile);
	fclose(deviceFile);
}

void readBlock(unsigned int blockNumber, void *blockData)
{
	FILE * deviceFile;
	deviceFile=fopen(deviceName,"rw+");
	if (deviceFile==0)
	{
		printf("floppya.img not found\n");
		return;
	}

	fseek(deviceFile,blockNumber*BLOCK_SIZE,SEEK_SET);
	fread(blockData,1,BLOCK_SIZE,deviceFile);
	fclose(deviceFile);
}

directoryBlock loadRootDirectory()
{
	directoryBlock rootBlock;
	readBlock(ROOT_BLOCK,&rootBlock);
	return rootBlock;
}

int findFileInDirectory(directoryBlock directory, char *fileName)
{
	int i;
	for(i=0;i<DIRECTORY_ENTRY_COUNT;i++)
		if(strcmp(directory.entries[i].fileName,fileName)==0)
			return i;
	return -1;
}


int findFileBlockInPath(char *path)
{
	char *directoryName = strtock(path,"/");
	int  directoryEntryNumber=0;
	unsigned int directoryBlockNumber = ROOT_BLOCK; //ubicacion del root en el block 16
	directoryBlock directory = loadRootDirectory();
	while(directoryName!=0)
	{
	    directoryEntryNumber = findFileInDirectory(directory,directoryName);
		if(directoryEntryNumber==-1)
			return 0;
		directoryBlockNumber = directory.entries[directoryEntryNumber].fileBlock;
		readBlock( directoryBlockNumber,&directory); 
		
		directoryName = strtok(0,"/");
	}
	return directoryBlockNumber;
}

void createDir(char *path, char *directoryName)
{
	unsigned int destinationDirBlockNumber = findFileBlockInPath(filePath);
	if(destinationDirBlockNumber == 0)
	{
		cout<<"Directory does not exists";
		return;
	}
	directoryBlock destinationDirectory;
	readBlock(destinationDirBlockNumber,&destinationDirectory);

	unsigned int newDirBlockNumber = allocateBlock();
	directoryBlock newDirectory = createEmptyDirectory();
	writeBlock(newDirBlockNumber,&newDirectory);

	for(int i=0;i<DIRECTORY_ENTRY_COUNT;i++)
	{
		if(destinationDirectory.entries[i].fileBlock==0)
		{
			strcpy_s(destinationDirectory.entries[i].fileName,fileName.c_str());
			destinationDirectory.entries[i].fileType='D';
			destinationDirectory.entries[i].fileBlock = newDirBlockNumber;
			break;
		}
	}
	writeBlock(destinationDirBlockNumber,&destinationDirectory);

}