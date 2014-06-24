#include "SuperFS.h"
#include "Utilities.h"


SuperFS::SuperFS(DeviceFS * disk)
{
	this->disk = disk;
}


SuperFS::~SuperFS(void)
{
}

//void * SuperFS::malloc(int sz) 
//{
//
// int next_index = 0;
// unsigned char our_memory[1024 * 1024];  
//	void *mem;
//
//if(sizeof our_memory - next_index < sz)
//  return '\0';
//
//  mem = &our_memory[next_index];
//  next_index += sz;
//  return mem; 
//}

DirectoryBlock SuperFS::createEmptyDirectory()
{
	DirectoryBlock newDirectory;
	for(int i=0;i<DIRECTORYENTRYCOUNT;i++)
	{
		newDirectory.entries[i].fileType = IS_FILE;
		newDirectory.entries[i].fileBlockStart = 0;
		newDirectory.entries[i].fileBlockSize = 0;
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
	{
		char *tempFileName = new char[utilities->mystrlen(directory.entries[i].fileName)];
		tempFileName = getRealValue(directory.entries[i].fileName);
		int cmp1 = utilities->strCmp(tempFileName,fileName);
		int cmp2 = utilities->strCmp(directory.entries[i].fileName,fileName);
		if(cmp1 == 1  || cmp2 == 1)
			return i;
	}
	return -1;
}

unsigned int SuperFS::findFileBlockInPath(char *fileName)
{
	fileName = getRealValue(fileName);
	char *path = new char[utilities->mystrlen(fileName)];
	char * directoryName,*context;
	int posToken = 2;
	
	utilities->strcpy(fileName, path);
	path = getRealValue(path);

	int tokens = utilities->qtytoken(path,'/'); 
	//directoryName = utilities->strtok(path,'/',posToken);
	directoryName = utilities->mystrtok(path,'/',posToken);
	directoryName = getRealValue(directoryName);
	int  directoryEntryNumber=0;
	unsigned int directoryBlockNumber = ROOTBLOCK;
	DirectoryBlock directory = loadRootDirectory();
	//posToken = posToken + 1;
	if(directoryName[0] == '\0')
		return directoryBlockNumber;
	while (posToken <= tokens) 
	{ 

	 	directoryEntryNumber = findFileInDirectory(directory,directoryName); 
	 		
	 	if(directoryEntryNumber==-1) 
	 	{
	 		return -1;
	 	}
	 	directoryBlockNumber = directory.entries[directoryEntryNumber].fileBlockStart; 
	 	disk->readBlock( directoryBlockNumber,(char *)&directory); 
		directoryName = utilities->mystrtok(path, '/', posToken+1); 
		directoryName = getRealValue(directoryName);
	 	posToken++; 
	} 
	return directoryBlockNumber;
}

void SuperFS::Format()
{
	SuperBlock superBlock;
	utilities->strcpy("SUPERF", superBlock.magicNumber);
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
	disk->writeBlock(ROOTBLOCK,&rootDirectory);
    
	disk->writeBlock(SUPERBLOCK,&superBlock);

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

char *SuperFS::getRealValue(char *cadena)
{
	char *temp = new char[utilities->mystrlen(cadena)];
	int i=0;
	for(i=0; i<utilities->mystrlen(cadena); i++)
	{
		temp[i] = (utilities->caracterValido(cadena[i])) ? cadena[i] : '\0';
	}
	temp[i] = '\0';
	return temp;
}

int SuperFS::createDir(char * filePath,char * fileName)
{
	filePath = getRealValue(filePath);
	fileName = getRealValue(fileName);

	 unsigned int destinationDirBlockNumber;
	 int comp = utilities->strCmp(filePath,"/");
	 if( comp == 1)
		 destinationDirBlockNumber = ROOTBLOCK;
	 else
		 destinationDirBlockNumber= findFileBlockInPath(filePath);
	 
	 if(destinationDirBlockNumber==-1)
	 {
		 cout<<"Directory does not exists";
		 return -1;
	 }
	 DirectoryBlock destinationDirectory;
	 disk->readBlock(destinationDirBlockNumber,&destinationDirectory);

	if(FileExist(destinationDirectory,fileName))
	{
		printf("Directory already exist\n");
		return -1;
	}


	 unsigned int newDirBlockNumber = AllocateBlock();
	 DirectoryBlock newDirectory = createEmptyDirectory();
	 disk->writeBlock(newDirBlockNumber,&newDirectory);

	 for(int i=0;i<DIRECTORYENTRYCOUNT;i++)
	 {
		 if(destinationDirectory.entries[i].fileBlockStart==0)
		 {
			 utilities->strcpy(fileName, destinationDirectory.entries[i].fileName);
			 destinationDirectory.entries[i].fileType=IS_DIRECTORY;
			 destinationDirectory.entries[i].fileBlockStart = newDirBlockNumber;
			 destinationDirectory.entries[i].fileBlockSize = 1;
			 break;
		 }
	 }
	 disk->writeBlock(destinationDirBlockNumber,&destinationDirectory);

	 cout<<"Directory "<<fileName<<" Created..."<<endl;

	 return newDirBlockNumber;
}


void SuperFS::listDir(char * filePath)
{
	 unsigned int destinationDirBlockNumber= findFileBlockInPath(filePath);
	 
	 if(destinationDirBlockNumber==-1)
	 {
		 cout<<"Directory does not exists";
		 return;
	 }
	 DirectoryBlock destinationDirectory;
	 disk->readBlock(destinationDirBlockNumber,&destinationDirectory);
	 for(int i=0;i<DIRECTORYENTRYCOUNT;i++)
	 {
	   if(destinationDirectory.entries[i].fileBlockStart!=0)
	   {
		   char *tempName = new char[utilities->mystrlen(destinationDirectory.entries[i].fileName)];
		   tempName = getRealValue(destinationDirectory.entries[i].fileName);
		   if(destinationDirectory.entries[i].fileType == IS_DIRECTORY)
		   {
			   printf("D ->	%s\n",tempName);
		   }
		   else if(destinationDirectory.entries[i].fileType == IS_FILE)
		   {
			   printf("F ->	%s\n",tempName);
		   }
	   }
	 }
}

char * SuperFS::getPathName(char * pathName, int type)
{
	int lengthName =  utilities->mystrlen(pathName); 
	char *path = new char[lengthName];//(char *)malloc(lengthName); 
	int tokens = utilities->qtytoken(pathName,'/'); 
	int posToken=2; 
	int i=0; 
	char *dirName = new char[lengthName];//(char*)malloc(lengthName);
	if(tokens == 1 && type == 1)
	{
		path[0] = pathName[0];
		path[1] = '\0';
	}
	if(tokens == 2)
	{
		path[0] = '/';
		path = getRealValue(path);
	}
	while (posToken <= tokens-1) 
	{ 
		char *token = utilities->mystrtok(pathName, '/', posToken); 
		
		if(posToken == 2)
			path = getRealValue(utilities->mistrcat("/",token));
		else
			path = getRealValue(utilities->mistrcat(path,token)); 

		if(posToken < tokens-1) 
		{
			path = getRealValue(path);
			path = utilities->mistrcat(path, "/"); 
			path = getRealValue(path);
		}
		posToken++; 
	} 
	
	if(type == 1)//retorna path sin la ultima carpeta o archivo
	{
		return path;
	}
	else
	{
		dirName = utilities->mystrtok(pathName, '/', tokens);
		dirName = getRealValue(dirName);
		return dirName;
	}

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

void SuperFS::renameDir(char *dirPath, char *newDirName) 
{ 
	dirPath = getRealValue(dirPath);
	newDirName = getRealValue(newDirName);
	DirectoryBlock dir; 
	int dirBlockNumber; 
	int entryNumber; 
	char *oldDirName; 
	char lengthdirPath=utilities->mystrlen(dirPath);
	char *path = new char[lengthdirPath]; 
	
	path = getPathName(dirPath,1);

	oldDirName = getPathName(dirPath,2);
	
	dirBlockNumber = findFileBlockInPath(path); 
	
	if(dirBlockNumber==-1) 
	{ 
		cout<<"Directory does not exists"; 
		return; 
	} 
	
	disk->readBlock(dirBlockNumber,&dir); 
	entryNumber = findFileInDirectory(dir, oldDirName); 
	
	if(entryNumber == -1)
	{
		cout<<"Directory does not exists"; 
		return; 
	}

	char *tempDirName = new char[utilities->mystrlen(dirPath)];
	tempDirName = getRealValue(dir.entries[entryNumber].fileName);

	if( utilities->strCmp(tempDirName,oldDirName) == 1) 
	{ 
		
		for(int i=0; i< utilities->mystrlen(dir.entries[entryNumber].fileName); i++)
		{
			dir.entries[entryNumber].fileName[i] = ' ';
		}
		strcpy(dir.entries[entryNumber].fileName,newDirName); 
		dir.entries[entryNumber].fileType = IS_DIRECTORY;
		disk->writeBlock(dirBlockNumber,(char*)&dir); 
	} 
}

void SuperFS::setFreeBlock(unsigned int blockNumber) 
{ 
    	SuperBlock spBlock; 

    	UnusedBlock newBlock; 
    	UnusedBlock lastFreeBlock; 
    	unsigned int secondToLastBlock;
    	newBlock.nextFreeBlock = 0; 
    	//loadSuperBlock(spBlock);
		spBlock = loadSuperBlock();
		disk->readBlock(spBlock.lastFreeBlock,(char*)&lastFreeBlock); 
    	lastFreeBlock.nextFreeBlock = blockNumber; 
    	secondToLastBlock =spBlock.lastFreeBlock; 
    	spBlock.lastFreeBlock = blockNumber; 
    	spBlock.totalFreeBlocks++; 
		disk->writeBlock(FsStart,(char*)&spBlock); 
    	disk->writeBlock(secondToLastBlock,(char*)&lastFreeBlock); 
    	disk->writeBlock(blockNumber,(char*)&newBlock); 
}


void SuperFS::deleteDirCascade(int block) 
{ 
	DirectoryBlock dir; 
	int i,j; 
	int fileStart; 
	int fileSize; 
	disk->readBlock(block, (char *)&dir); 
	if(block == 0) 
	{ 
		return; 
	} 
	for(i=0; i<DIRECTORYENTRYCOUNT;i++) 
	{
		if(dir.entries[i].fileType == IS_DIRECTORY) 
		{ //recorrer todo el directorio
			deleteDirCascade(dir.entries[i].fileBlockStart); 
			setFreeBlock(i); 
		} 
		else if(dir.entries[i].fileType == IS_FILE) 
		{ 
			fileStart = dir.entries[i].fileBlockStart; 
			fileSize = dir.entries[i].fileBlockSize; 
			for(j=fileStart; j<fileSize; j++ ) 
			{ 
				setFreeBlock(j);
			}
			if(i == DIRECTORYENTRYCOUNT -1 )
				return;
		} 
		/*else
		{
			deleteDirCascade(dir.entries[i].fileBlockStart);
		}*/
	} 
}

void SuperFS::removeDir(char * path)
{
	unsigned int j;
	int fileStart;
	int fileSize;
	DirectoryBlock blockToErase;
	unsigned int removableBlockEntrie;
	char *blockPath = getPathName(path,1);
	char *dirToRemove = getPathName(path,2);
	unsigned int blockToRemove = findFileBlockInPath(blockPath);
	disk->readBlock(blockToRemove,(char*)&blockToErase);
	
	removableBlockEntrie = findFileInDirectory(blockToErase,dirToRemove);
	
	if(removableBlockEntrie == -1)
	{
		cout<<"Directory does not exist"<<endl;
		return;
	}

	if(blockToErase.entries[removableBlockEntrie].fileType== IS_DIRECTORY)
	{
		deleteDirCascade(blockToErase.entries[removableBlockEntrie].fileBlockStart);
		setFreeBlock(blockToErase.entries[removableBlockEntrie].fileBlockStart);
	}
	else if(blockToErase.entries[removableBlockEntrie].fileType == IS_FILE) 
	{ 
		fileStart = blockToErase.entries[removableBlockEntrie].fileBlockStart; 
		fileSize = blockToErase.entries[removableBlockEntrie].fileBlockSize; 
		for(j=fileStart; j<fileSize; j++ ) 
		{ 
		 	setFreeBlock(j);
		} 
	} 
	for(int i=0; i<sizeof(blockToErase.entries[removableBlockEntrie].fileName); i++)
			blockToErase.entries[removableBlockEntrie].fileName[i] = '\0';
	blockToErase.entries[removableBlockEntrie].fileBlockSize = 0;
	blockToErase.entries[removableBlockEntrie].fileBlockStart = 0;
	disk->writeBlock(blockToRemove,(char*)&blockToErase);
}


void SuperFS::copyDirCascade(int blockSource, int blockDestino, char *pathDestino)
{
	DirectoryBlock dirSource;
	DirectoryBlock dirDestino;
	int i,j;
	int fileStart;
	int fileSize;
	char *dirName;	
	int newBlockDir;

	disk->readBlock(blockSource, (char *)&dirSource);
	disk->readBlock(blockDestino, (char *)&dirDestino);

	if(blockSource == 0)
	{
		return;
	}
		
	for(i=0; i<DIRECTORYENTRYCOUNT;i++)
	{
		
		if(dirSource.entries[i].fileType == IS_DIRECTORY)
		{

			//recorrer todo el directorio
			newBlockDir = createDir(pathDestino, dirSource.entries[i].fileName);
			dirName = dirSource.entries[i].fileName;
			for(j=0; j<DIRECTORYENTRYCOUNT;j++)
			{
				if(dirDestino.entries[j].fileBlockStart == 0)
				{
					dirDestino.entries[j].fileName[0] = '\0';
					utilities->strcpy(dirName, dirDestino.entries[j].fileName);
					dirDestino.entries[j].fileType = IS_DIRECTORY;
					dirDestino.entries[j].fileBlockStart = newBlockDir;	
					dirDestino.entries[j].fileBlockSize = 1;	
					break;
				}
			}
			disk->writeBlock(blockDestino, (char *)&dirDestino);
			copyDirCascade(dirSource.entries[i].fileBlockStart, newBlockDir,dirName);		
		}
		else //if(dirSource.entries[i].fileType == IS_FILE)
		{
			fileStart = dirSource.entries[i].fileBlockStart;
			fileSize = dirSource.entries[i].fileBlockSize;
			
			//TODO: llamar la funcion de crear archivo
			//createFile(fileName);

			
			if(i == DIRECTORYENTRYCOUNT -1 )
				return;

		}
		/*else
		{
			copyDirCascade(dirSource.entries[i].fileBlockStart, blockDestino, dirName);		
		}*/
	}

}

void SuperFS::copyDir(char *pathSource, char *pathDestino, char *nameOfCopy)
{
	pathSource = getRealValue(pathSource);
	pathDestino = getRealValue(pathDestino);
	nameOfCopy = getRealValue(nameOfCopy);

	DirectoryBlock dirSource;
	DirectoryBlock dirDestino;
	int blockSource;
	int blockDestino;
	char *sourceEndPathName;
	int newBlockDir;
	//int entrySource;
	int j;
	char *dirName;

	blockSource = findFileBlockInPath(pathSource);
	blockDestino = findFileBlockInPath(pathDestino);

	if(blockSource==0 || blockDestino==0) 
	{
		return;
	}
	
	sourceEndPathName = getPathName(pathSource,2);

	disk->readBlock(blockSource, (char *)&dirSource);
	disk->readBlock(blockDestino, (char *)&dirDestino);

	//entrySource = findFileInDirectory(dirSource, sourceEndPathName);
	
	//if(dirSource.entries[entrySource].fileType != IS_FILE)
	{
		//recorrer todo el directorio
		newBlockDir = createDir(pathDestino, nameOfCopy);//dirSource.entries[entrySource].fileName);
		
		if( utilities->strCmp(getRealValue(pathDestino), "/") == 1)
			dirName = getRealValue(pathDestino);
		else
		{
			dirName = utilities->mistrcat(pathDestino, "/");
			dirName = utilities->mistrcat(getRealValue(dirName), nameOfCopy);
			dirName = getRealValue(dirName);
		}
		for(j=0; j<DIRECTORYENTRYCOUNT;j++)
		{
			if(dirDestino.entries[j].fileBlockStart == 0)
			{
				dirDestino.entries[j].fileName[0] = '\0';
				utilities->strcpy(nameOfCopy, dirDestino.entries[j].fileName);
				dirDestino.entries[j].fileType = IS_DIRECTORY;
				dirDestino.entries[j].fileBlockStart = newBlockDir;	
				dirDestino.entries[j].fileBlockSize = 1;	
				break;
			}
		}

		disk->writeBlock(blockDestino, (char *)&dirDestino);
		copyDirCascade(blockSource, newBlockDir, dirName);		
	}

}

void SuperFS::move(char *pathSource, char *pathDestino)
{
	char *nameOfCopy = new char[utilities->mystrlen(getRealValue(pathSource))];
	
	nameOfCopy = getPathName(getRealValue(pathSource),2);

	nameOfCopy = getRealValue(nameOfCopy);

	copyDir(pathSource, pathDestino,nameOfCopy);
	removeDir(pathSource);
}

//FUNCIONES DE ARCHIVOS

FileBlock SuperFS::createEmptyFile()
{
	FileBlock newFile;
	newFile.size = 0;
	newFile.type = IS_FILE;
	newFile.data[0] = '\0';
	
	return newFile;
}

void SuperFS::writeNewFile(FileBlock *file, char fileData[], int blockStart)
{
	int dataLenght;
	int module;
	int blocksToWrite;
	int posData;
	int i = 0;
	int j = 0;
	int x = 0;
	char tempData[512];
	dataLenght = utilities->mystrlen(fileData);
	blocksToWrite = dataLenght / BLOCKSIZE;
	module = utilities->mod(blocksToWrite, BLOCKSIZE);
	posData = 481;
	if (module != 0)
	{
		blocksToWrite = blocksToWrite + 1;
	}
	if (blocksToWrite == 0)
	{
		blocksToWrite = 1;
	}

	file->size = blocksToWrite;
	while (i < blocksToWrite)
	{
		if (i == 0)
		{
			//j = 0;
		/*	while (j < posData)
			{
				file.data[j] = fileData[j];
				j++;
			}
			*/
			disk->writeBlock(blockStart, (char *)&file);
			blockStart++;
		}
		else
		{
			j = posData;
			while (j < posData + 512)
			{
				tempData[x] = fileData[j];
				j++;
				x++;
			}

			disk->writeBlock(blockStart, tempData);
			blockStart++;
			posData = posData + 512;
		}
		i++;
	}
}

bool SuperFS::FileExist(DirectoryBlock dirBlock, char *fileName)
{
	for(int i=0; i<DIRECTORYENTRYCOUNT; i++)
	{
		int cmp = utilities->strCmp(getRealValue(dirBlock.entries[i].fileName),fileName);
		if(cmp == 1)
		{
			return true;
		}
	}
	return false;	
}

void SuperFS::createFile(char * path, char* fileName, char acess[], char *data)
{
	path = getRealValue(path);
	fileName = getRealValue(fileName);
	acess = getRealValue(acess);
	data = getRealValue(data);
	int i;
	DirectoryBlock destinationDirectory;
	FileBlock file;
	unsigned int newDirBlockNumber;
	unsigned int destinationDirBlockNumber;
	int comp;
	comp = utilities->strCmp(path, "/");
	if (comp == 1)
	{
		destinationDirBlockNumber = ROOTBLOCK;
	}
	else
	{
		destinationDirBlockNumber = findFileBlockInPath(path);
	}
	if (destinationDirBlockNumber == -1)
	{
		printf("no se puede crear. el path no existe");
		return;
	}
	disk->readBlock(destinationDirBlockNumber, (char*)&destinationDirectory);
	
	if(FileExist(destinationDirectory,fileName))
	{
		printf("File already exist\n");
		return;
	}

	newDirBlockNumber = AllocateBlock();
	file = createEmptyFile();
	utilities->strcpy(fileName, file.fileName);
	utilities->strcpy(acess, file.acessRights);
	utilities->strcpy(data, file.data);
	file.type = IS_FILE;
	writeNewFile(&file, data, newDirBlockNumber);

	for (i = 0; i<DIRECTORYENTRYCOUNT; i++)
	{
		if (destinationDirectory.entries[i].fileBlockStart == 0)
		{
			strcpy(destinationDirectory.entries[i].fileName, file.fileName);
			destinationDirectory.entries[i].fileType = IS_FILE;
			destinationDirectory.entries[i].fileBlockStart = newDirBlockNumber;
			destinationDirectory.entries[i].fileBlockSize = file.size;
			break;
		}
	}
	disk->writeBlock(destinationDirBlockNumber, (char*)&destinationDirectory);
	return;
}

void SuperFS::removeFile(char *path, char *fileName)
{
	path = getRealValue(path);
	fileName = getRealValue(fileName);
	unsigned int j;
	int fileStart;
	int fileSize;
	DirectoryBlock blockPath;
	unsigned int removableBlock;
	unsigned int blockPathNumber = findFileBlockInPath(fileName);
	if (blockPathNumber == -1)
	{
		printf("Directory does not exist");
		return;
	}
	disk->readBlock(blockPathNumber, (char*)&blockPath);
	
	
	removableBlock = findFileInDirectory(blockPath, fileName);
	if (removableBlock == -1)
	{
		printf("File does not exist");
		return;
	}
	if (blockPath.entries[removableBlock].fileType == IS_FILE)
	{
		fileStart = blockPath.entries[removableBlock].fileBlockStart;
		fileSize = blockPath.entries[removableBlock].fileBlockSize;
		for (j = 0; j<fileSize; j++)
		{
			setFreeBlock(fileStart);
			fileStart++;
		}
		for(int i=0; i<sizeof(blockPath.entries[removableBlock].fileName); i++)
			blockPath.entries[removableBlock].fileName[i] = '\0';
		blockPath.entries[removableBlock].fileBlockSize = 0;
		blockPath.entries[removableBlock].fileBlockStart = 0;
		disk->writeBlock(blockPathNumber, (char*)&blockPath);
	}
}


void SuperFS::loadFile(char *path, char *fileName)
{
	path = getRealValue(path);
	fileName = getRealValue(fileName);
	unsigned int j;
	int fileStart;
	int fileSize;
	FileBlock file;
	char *temData;
	DirectoryBlock blockPath;
	unsigned int fileBlockNumber;
	unsigned int blockPathNumber = findFileBlockInPath(fileName);
	disk->readBlock(blockPathNumber, (char*)&blockPath);
	fileBlockNumber = findFileInDirectory(blockPath, fileName);
	j = 0;
	if (blockPathNumber == -1)
	{
		printf("El archivo %s no existe", fileName);
	}
	if (blockPath.entries[fileBlockNumber].fileType == IS_FILE)
	{
		fileStart = blockPath.entries[fileBlockNumber].fileBlockStart;
		fileSize = blockPath.entries[fileBlockNumber].fileBlockSize;

		while (j<fileSize)
		{
			if (j == 0)
			{
				disk->readBlock(fileStart, (char *)&file);
				printf("File Name: ");
				printf(file.fileName);
				printf(" \nFile Type: ");
				printf("%d", file.type);
				printf(" \nSize: ");
				printf("%d", file.size);
				printf(" \nAccess: ");
				printf(file.acessRights);
				printf(" \nData: ");
				printf(file.data);
			}
			else
			{
				disk->readBlock(fileStart, temData);
				printf(temData);
			}
			j++;
			fileStart++;
		}
	}
	else
		printf("El archivo %s no existe", fileName);
}
