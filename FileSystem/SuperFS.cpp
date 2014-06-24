#include "SuperFS.h"
#include "Utilities.h"


SuperFS::SuperFS(DeviceFS * disk)
{
	this->disk = disk;
}


SuperFS::~SuperFS(void)
{
}

DirectoryBlock SuperFS::createEmptyDirectory()
{
	DirectoryBlock newDirectory;
	for(int i=0;i<DIRECTORYENTRYCOUNT;i++)
	{
		newDirectory.entries[i].fileBlockStart = 0;
		newDirectory.entries[i].fileBlock = 0;
	}
	return newDirectory;
}

SuperBlock  SuperFS::loadSuperBlock()
{
	SuperBlock superBlock;
	disk->readBlock(1,&superBlock);
	return superBlock;
}


DirectoryBlock  SuperFS::loadRootDirectory()
{
	DirectoryBlock rootBlock;
	disk->readBlock(2,&rootBlock);
	return rootBlock;
}

int SuperFS::findFileInDirectory(DirectoryBlock directory,char * fileName)
{
	for(int i=0;i<DIRECTORYENTRYCOUNT;i++)
		if(utilities->strCmp(directory.entries[i].fileName,fileName)==0)
			return i;
	return -1;
}
unsigned int SuperFS::findFileBlockInPath(char *fileName)
{
	char path[1000] ;
	char * directoryName,*context;
	int posToken = 1;
	int tokens = utilities->qtytoken(path,'-'); 
	utilities->strcpy(path,fileName);
	

	directoryName = utilities->strtok(path,'/',posToken);
	int  directoryEntryNumber=0;
	unsigned int directoryBlockNumber = 2;
	DirectoryBlock directory = loadRootDirectory();
	posToken = posToken + 1;
	while (posToken <= tokens) 
	 	{ 

	 		directoryEntryNumber = findFileInDirectory(directory,directoryName); 
	 		
	 		if(directoryEntryNumber==-1) 
	 		{
	 			return ROOTBLOCK;
	 		}
	 		directoryBlockNumber = directory.entries[directoryEntryNumber].fileBlockStart; 
	 		disk->readBlock( directoryBlockNumber,(char *)&directory); 
	 		directoryName = utilities->strtok(path, '-', posToken); 
	 		posToken++; 
	 	} 

	/*while(directoryName!=0)
	{
	    directoryEntryNumber = findFileInDirectory(directory,directoryName);
		if(directoryEntryNumber==-1)
			return 0;
		directoryBlockNumber = directory.entries[directoryEntryNumber].fileBlock;
		disk->readBlock( directoryBlockNumber,&directory); 
		
		directoryName = strtok_s(0,"/",&context);
	}*/
	return directoryBlockNumber;
}

void SuperFS::Format()
{
	SuperBlock superBlock;
	utilities->strcpy(superBlock.magicNumber,"SUPERF");
	superBlock.totalBlocksInDisk = disk->getTotalBlocks();
	superBlock.totalFreeBlocks = superBlock.totalBlocksInDisk  - 3;
	superBlock.firstFreeBlock = 3;
	superBlock.lastFreeBlock = superBlock.totalBlocksInDisk -1;

	
	for(int blockNumber = superBlock.firstFreeBlock;blockNumber<superBlock.lastFreeBlock;blockNumber++)
	{
		UnusedBlock freeBlock;
		freeBlock.nextFreeBlock = blockNumber +1;
		disk->writeBlock(blockNumber,&freeBlock);
	}
	UnusedBlock lastFreeBlock;
	lastFreeBlock.nextFreeBlock=0;
	disk->writeBlock(superBlock.lastFreeBlock,&lastFreeBlock);

	DirectoryBlock rootDirectory = createEmptyDirectory();
    disk->writeBlock(2,&rootDirectory);
    
	disk->writeBlock(1,&superBlock);

}

void SuperFS::FreeBlock(unsigned int blockNumber)
{
	SuperBlock superBlock = loadSuperBlock();
	
	UnusedBlock newBlock;
	newBlock.nextFreeBlock = 0;

	UnusedBlock lastFreeBlock;
	disk->readBlock(superBlock.lastFreeBlock,&lastFreeBlock);

	lastFreeBlock.nextFreeBlock = blockNumber;
	unsigned int secondToLastBlock =superBlock.lastFreeBlock;
	superBlock.lastFreeBlock = blockNumber;

	superBlock.totalFreeBlocks++;

	disk->writeBlock(1,&superBlock);
	disk->writeBlock(secondToLastBlock,&lastFreeBlock);
	disk->writeBlock(blockNumber,&newBlock);

}

unsigned int SuperFS::AllocateBlock()
{
	SuperBlock superBlock = loadSuperBlock();

	unsigned int allocatedBlockNumber = superBlock.firstFreeBlock;
    UnusedBlock  allocatedBlock;
	disk->readBlock(allocatedBlockNumber,&allocatedBlock);
	unsigned int newFirstBlockNumber = allocatedBlock.nextFreeBlock;
	superBlock.firstFreeBlock = newFirstBlockNumber;
	superBlock.totalFreeBlocks--;
	disk->writeBlock(1,&superBlock);

	return allocatedBlockNumber;
}


unsigned int  SuperFS::GetFreeSpace()
{
	SuperBlock super = loadSuperBlock();
	return super.totalFreeBlocks;
}

void SuperFS::createDir(char * filePath,char * fileName)
{
	 unsigned int destinationDirBlockNumber= findFileBlockInPath(filePath);
	 if(destinationDirBlockNumber==0)
	 {
		 cout<<"Directory does not exists";
		 return;
	 }
	 DirectoryBlock destinationDirectory;
	 disk->readBlock(destinationDirBlockNumber,&destinationDirectory);

	 unsigned int newDirBlockNumber = AllocateBlock();
	 DirectoryBlock newDirectory = createEmptyDirectory();
	 disk->writeBlock(newDirBlockNumber,&newDirectory);

	 for(int i=0;i<DIRECTORYENTRYCOUNT;i++)
	 {
		 if(destinationDirectory.entries[i].fileBlock==0)
		 {
			 utilities->strcpy(destinationDirectory.entries[i].fileName,fileName);
			 destinationDirectory.entries[i].fileType='D';
			 destinationDirectory.entries[i].fileBlock = newDirBlockNumber;
			 break;
		 }
	 }
	 disk->writeBlock(destinationDirBlockNumber,&destinationDirectory);
}


void SuperFS::listDir(char * filePath)
{
	 unsigned int destinationDirBlockNumber= findFileBlockInPath(filePath);
	 if(destinationDirBlockNumber==0)
	 {
		 cout<<"Directory does not exists";
		 return;
	 }
	 DirectoryBlock destinationDirectory;
	 disk->readBlock(destinationDirBlockNumber,&destinationDirectory);
	 for(int i=0;i<DIRECTORYENTRYCOUNT;i++)
	 {
	   if(destinationDirectory.entries[i].fileBlock!=0)
		  cout<<destinationDirectory.entries[i].fileName<<" "
			<<destinationDirectory.entries[i].fileType<<"\n";
			
	 }
}

char * SuperFS::getPathName(char * pathName, int type)
{
	int lengthName = utilities->mystrlen(pathName);
    int tokens = utilities->qtytoken(pathName,'/');
    int posToken = 1;
    int i=0;
    char *dirName;
    char * path  = (char *)malloc(lengthName);

    char * prueba = (char*)malloc(5);
	if(type==1)
	{
		return path;
	}
	//printnumber(mystrlen(prueba));
    path = utilities->strtok(pathName, '/', posToken);
    path = utilities->mistrcat(pathName, "/");
    
    posToken++;

    while (posToken <= tokens-1)
    {
    	char *token;
    	token = utilities->strtok(path, '/', posToken);
        path = utilities->mistrcat(path, token);
        if(posToken != tokens-1)
            path = utilities->mistrcat(path, "/");
        posToken++;
    }
    
    dirName = utilities->strtok(path, '/', tokens);

	return path;
}

void SuperFS::mkdirFS(char* name)
{
	char * path;
	char * dirName;
	path = getPathName(name,1);
	dirName = getPathName(name,2);
	cout<<"llama al create dir";
	
	createDir(path, dirName); 
}
