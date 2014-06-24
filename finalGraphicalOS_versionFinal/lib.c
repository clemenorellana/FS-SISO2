/*lib.c
Michael Black, 2007
This file contains the various library functions called by the shell and 
user applications
*/

/*setup enables multitasking.  it should be called at the beginning of 
any user program*/
#include "lib.h"
void setup()
{
	seti();
}

/*computes a mod b*/
int mod(int a, int b)
{
        while(a>=b)
                a=a-b;
        return a;
}

/*computes a / b using integer division*/
int div(int a, int b)  
{               
        int q=0;
        while (q*b<=a)
                q++;
        return q-1;
}               

/*reads a sector into the buffer*/
void readsector(int sectornumber, char* buffer)
{
        int sec,head,cyl;

	/*convert to CHS*/
        sec=mod(sectornumber,0x12)+1;
        head=mod(div(sectornumber,0x12),2);
        cyl=div(sectornumber,0x24);

        readsect(buffer,sec,head,cyl);
}

/*writes buffer to a sector*/
void writesector(int sectornumber, char* buffer)
{
        int sec,head,cyl;

	/*convert to CHS*/
        sec=mod(sectornumber,0x12)+1;
        head=mod(div(sectornumber,0x12),2);
        cyl=div(sectornumber,0x24);

        writesect(buffer,sec,head,cyl);
}

/*prints a character*/
void putchar(char c)
{
	printc(c);
}

/*reads a character*/
char getchar()
{
	return (char)readc();
}

/*prints a string terminated with 0*/
void printstring(char* string)
{
	int21(1,string);
}

/*prints an integer*/
void printnumber(int number)
{
	char num[12];
	char pnum[12];
	int d=1;
	int i=0;
	int j;

	/*convert the number to a string digit by digit*/
	while(i<10)
	{
		num[i]=mod(div(number,d),10)+0x30;
		i++;
		d=d*10;
		if (div(number,d)==0)
			break;
	}

	/*reverse it to read most significant to least significant*/
	j=0;
	for (d=i-1; d>=0; d--)
		pnum[j++]=num[d];
	pnum[j]=0;
	printstring(pnum);
}

/*read a line from the keyboard and put it in buffer*/
void readstring(char* buffer)
{
	int21(2,buffer);
}

/*read the file name[] into buffer[]*/
void readfile(char* name, char* buffer)
{
	int21(3,name,buffer);
}

/*write buffer[] to a file called name[]*/
void writefile(char* name, char* buffer, int sectorlength)
{
	int21(4,name,buffer,sectorlength);
}

/*delete the file name[]*/
void deletefile(char* name)
{
	int21(5,name);
}

/*execute the program*/
void executeprogram(char* buffer, int bytelength)
{
	int21(8,buffer,bytelength);
}

/*execute the program, but don't make the caller wait*/
void executeprogrambackground(char* buffer, int bytelength)
{
	int21(6,buffer,bytelength);
}

/*terminate program - this must be called by the program before ending*/
void exit()
{
	int21(7);
}

/*sets the video to 1 - graphics, 0 - text*/
void setvideo(int mode)
{
	if (mode==0)
		setvideotext();
	else
		setvideographics();
}

/*sets the pixel at columnxrow to color*/
void setpixel(int color, int column, int row)
{
	drawpixel(color,row*320+column);
}

/*sets the cursor position to row, column*/
void setcursor(int row, int column)
{
	interrupt(0x10,2*256,0,0,row*256+column);
}

/*clear the screen*/
void clearscreen()
{
        int r,c;
        for (c=0; c<80; c++)
                for (r=0; r<25; r++)
                {
                        putInMemory(0xB800,(r*80+c)*2,0x20);
                        putInMemory(0xB800,(r*80+c)*2+1,7);
                }
}

/*prints a character at a certain cursor position*/
void setchar(char c, char color, int row, int column)
{
	putInMemory(0xB800,(row*80+column)*2,c);
	putInMemory(0xB800,(row*80+column)*2+1,color);
}

/*prints a string at a certain cursor position*/
void setstring(char* c, char color, int row, int column)
{
	int i=0;
	while(c[i]!=0x0)
	{
		putInMemory(0xB800,(row*80+column+i)*2,c[i]);
		putInMemory(0xB800,(row*80+column+i)*2+1,color);
		i++;
	}
}

/*turns an integer into a string*/
void getnumberstring(char* pnum, int number)
{
	char num[12];
	int d=1;
	int i=0;
	int j;

	/*convert the number to a string digit by digit*/
	while(i<10)
	{
		num[i]=mod(div(number,d),10)+0x30;
		i++;
		d=d*10;
		if (div(number,d)==0)
			break;
	}

	/*reverse it to read most significant to least significant*/
	j=0;
	for (d=i-1; d>=0; d--)
		pnum[j++]=num[d];
	pnum[j]=0;
}

// ----------------------------- utilities ------------------------------------------

void *malloc(int sz) 
{
	int next_index = 0;
 	unsigned char our_memory[1024 * 1024];  
	void *mem;

	
	if(sizeof our_memory - next_index < sz)
  		return '\0';

	mem = &our_memory[next_index];
  	next_index += sz;

  	return mem; 
}


int stlen(char * cadena, int start, char delim)
{
    int count = 0, pos = (cadena[start]==delim) ? start+1 : start;

	while(cadena[pos]!=delim && cadena[pos]!='\0'){
		count++;
		pos++;
	}

	return count;
}

int len(char* cadena)
{
	int i = 0;
	while(cadena[i]!= '\0')
	{
		i++;
	}
	return i;
}

char *mystrcat(char *destino, char *fuente) 
{ 
	int i=0; 
	int j=0; 
	int leng = len(destino) + len(fuente); 
	char *temp = (char *)malloc(leng);
	int pos = 0;  
	printstring(" leng: ");
	printnumber(leng);
	while(destino[i] && destino[i]!='\0')
	{ 
		temp[i] = destino[i];
		i++; 
	}
	while(fuente[j]) 
	{ 
		temp[i]=fuente[j]; 
		i++; 
		j++; 
	} 
	temp[i]='\0'; 
	
	destino = (char *)malloc(leng); 
	for(pos;pos<leng;pos++) 
		destino[pos] = temp[pos];  
}

int myStrCmp (char *s1, char *s2) 
{ 
	char *p1 = (char *)s1;
	char *p2 = (char *)s2; 
	while (*p1 != '\0') 
		{ 
			if (*p2 == '\0') return 1; 
			if (*p2 > *p1) return -1; 
			if (*p1 > *p2) return 1; 
			p1++; 
			p2++; 
		} 
		if (*p2 != '\0') 
			return -1; 
			return 0; 
}

char * strcpy( char *to,char* from) 
{ 
	char *s1; 
	int i=0; 
	while (to[i]!='\0') 
		{ 
			from[i]=to[i]; 
			i++; 
		} 
	return from; 
}


void memcpy(){};


int qtytoken(char * token, char delim){
    int totalTokens = 1;

    int pos = 0;
    while(token[pos]!='\0')
    {
    	if(token[pos]==delim){
    	totalTokens++;
    	}
    	pos++;
    }
    return totalTokens;
}

char * strtok(char * str, char delim, int pos)
{
	
	int totalTokens = qtytoken(str,delim), count = 1, posStr = 0;
	char * token = (char*)malloc(stlen(str, posStr, delim));

	int posToken = 0;
	while(str[posStr]!='\0')
    {
		if(str[posStr]==delim)
        {
			count++;
		
			token =((count<pos)) ? (char*)malloc(stlen(str, posStr, delim)) : token;
		}
		else if(count==pos){
			token[posToken] = str[posStr];
			posToken++;
		}
		posStr++;
	}	
	return token;
}

// ----------------------------- SuperFS ------------------------------------------
void createEmptyDirectory(directoryBlock *newDirectory) 
{ 
  	int i = 0; 
	while(i<DIRECTORY_ENTRY_COUNT) 
	{ 
		newDirectory->entries[i].fileBlockStart = 0; 
		newDirectory->entries[i].fileBlockSize = 0;
		i++;
	} 
}

void loadRootDirectory(directoryBlock *rootBlock)
{
	readsector(ROOT_BLOCK,(char*)&rootBlock); 
}

void loadSuperBlock(superBlock *mysuperBlock)
{
	readsector(SUPER_BLOCK,(char*)mysuperBlock);
}

int findFileInDirectory(directoryBlock *directory, char * fileName)
{
	int i;
	for(i=0;i<DIRECTORY_ENTRY_COUNT;i++)
		if(myStrCmp(directory->entries[i].fileName,fileName)==0)
			return i;
	return -1;
}

int findFileBlockInPath(char* path)
{
	int tokens = qtytoken(path,'-'); 
	int posToken=1; 
	int directoryEntryNumber; 
	unsigned int directoryBlockNumber; 
	directoryBlock directory; 
	char *directoryName; 
	directoryName = strtok(path, '-', posToken); 
	posToken = posToken + 1; 
	directoryEntryNumber=0;
	directoryBlockNumber = ROOT_BLOCK; 
	loadRootDirectory(directory); 
	printstring("despues del leer el root"); 
	printnumber(directory.entries[directoryEntryNumber].fileBlockStart); 
	while (posToken <= tokens) 
	{ 

		directoryEntryNumber = findFileInDirectory(directory,directoryName); 
		
		if(directoryEntryNumber==-1) 
		{
			return -1;
		}
		directoryBlockNumber = directory.entries[directoryEntryNumber].fileBlockStart; 
		readsector( directoryBlockNumber,(char *)&directory); 
		directoryName = strtok(path, '-', posToken); 
		posToken++; 
	} 
	return directoryBlockNumber;
}

unsigned int AllocateBlock()
{
	superBlock mysuperBlock;
	unsigned int newFirstBlockNumber;
	unsigned int allocatedBlockNumber;
	unusedBlock  allocatedBlock;

	readsector(FsStart,(char*)&mysuperBlock);
	allocatedBlockNumber = mysuperBlock.firstFreeBlock;

	readsector(allocatedBlockNumber,(char*)&allocatedBlock);
	newFirstBlockNumber = allocatedBlock.nextFreeBlock;
	mysuperBlock.firstFreeBlock = newFirstBlockNumber;
	mysuperBlock.totalFreeBlocks--;
	writesector(totalBlocksFS,(char*)&mysuperBlock);

	return allocatedBlockNumber;
}

char *getPath(char* pathName, int type)
{
 	int lengthName = len(pathName); 
	char *path = (char *)malloc(lengthName); 
	int tokens = qtytoken(pathName,'-'); 
	int posToken=1; 
	int i=0; 
	char *dirName = (char *)malloc(lengthName);
	char *token = (char *)malloc(lengthName);
	
	if(type == 1)
	{
		while (posToken <= tokens-1) 
		{ 
			token = strtok(pathName, '-', posToken); 
			
			if(posToken == 2)				
				path = mystrcat("/",pathName);
			else 
				path = mystrcat(path, token); 
			if(posToken < tokens-1) 
				path = mystrcat(path, "-"); 
			posToken = posToken + 1; 
		} 
		return path;
	}
	else
	{
		dirName = strtok(pathName, '-', tokens);
		return dirName;
	}

}

void setFreeBlock(unsigned int blockNumber) 
{ 
	superBlock mysuperBlock ; 
	unusedBlock newBlock; 
	unusedBlock lastFreeBlock; 
	unsigned int secondToLastBlock;

	loadSuperBlock(mysuperBlock);
	newBlock.nextFreeBlock = 0; 
	readsector(mysuperBlock.lastFreeBlock,(char*)&lastFreeBlock); 
	lastFreeBlock.nextFreeBlock = blockNumber; 
	secondToLastBlock =mysuperBlock.lastFreeBlock; 
	mysuperBlock.lastFreeBlock = blockNumber; 
	mysuperBlock.totalFreeBlocks++; 
	writesector(FsStart,(char*)&mysuperBlock); 
	writesector(secondToLastBlock,(char*)&lastFreeBlock); 
	writesector(blockNumber,(char*)&newBlock); 
}


//------------------Functions for Directories------------------
unsigned int createDirectory(char* filePath,char* fileName)
{
	int i;
	directoryBlock destinationDirectory;
	directoryBlock newDirectory;
	unsigned int newDirBlockNumber;
	unsigned int destinationDirBlockNumber;
	
	if(myStrCmp(filePath,"-") == 1)
		destinationDirBlockNumber = ROOT_BLOCK;
	else
		destinationDirBlockNumber = findFileBlockInPath(filePath);

	if(destinationDirBlockNumber==-1)
	{
		printstring("Directory does not exists");
		return ROOT_BLOCK;
	}
	
	readsector(destinationDirBlockNumber,(char*)&destinationDirectory);
	newDirBlockNumber = AllocateBlock();
    createEmptyDirectory(newDirectory);
	writesector(newDirBlockNumber,(char*)&newDirectory);
	
	for(i=0;i<DIRECTORY_ENTRY_COUNT;i++)
	{
		if(destinationDirectory.entries[i].fileBlockStart==0)
		{
			strcpy(destinationDirectory.entries[i].fileName,fileName);
			destinationDirectory.entries[i].fileType='D';
			destinationDirectory.entries[i].fileBlockStart = newDirBlockNumber;
			destinationDirectory.entries[i].fileBlockSize = 1;
			break;
		}
	}
	writesector(destinationDirBlockNumber,(char*)&destinationDirectory);
	printstring("Directory created");
	return destinationDirBlockNumber;
}

void listDirectory(char * filePath)
{
	int i;
	directoryBlock destinationDirectory;
	unsigned int destinationDirBlockNumber;
	destinationDirBlockNumber= findFileBlockInPath(filePath);
	if(destinationDirBlockNumber==-1)
	{
		printstring("Directory does not exists.\r\n\n\0");	
		return;
	}
	
	readsector(destinationDirBlockNumber,(char*)&destinationDirectory);
	for(i=0;i<DIRECTORY_ENTRY_COUNT;i++)
	{
		if(destinationDirectory.entries[i].fileBlockStart!=0)
		{
			char *tempName;//;new char[utilities->mystrlen(destinationDirectory.entries[i].fileName)];
			strcpy(tempName,destinationDirectory.entries[i].fileName);
			if(destinationDirectory.entries[i].fileType == IS_DIRECTORY)
			{
				printstring("D -> %s\n",tempName);
			}
			else if(destinationDirectory.entries[i].fileType == IS_FILE)
			{
				printstring("F -> %s\n",tempName);
			}
		}
	}
}

void renameDirectory(char *dirPath, char *newDirName) 
{ 
	directoryBlock dir; 
	int dirBlockNumber; 
	int entryNumber; 
	char *oldDirName; 
	char *path; 
	char lengthdirPath=len(dirPath);
	path = (char *)malloc(lengthdirPath);
	path = getPath(dirPath,1); 
	oldDirName = getPath(dirPath,2);
	dirBlockNumber = findFileBlockInPath(path); 
	if(dirBlockNumber==ROOT_BLOCK) 
	{ 
		printstring("Directory does not exists"); 
		return; 
	} 
	readsector(dirBlockNumber,&dir); 
	entryNumber = findFileInDirectory(dir, oldDirName); 
	if(dir.entries[entryNumber].fileName == oldDirName) 
	{ 
		strcpy(dir.entries[entryNumber].fileName,newDirName); 
		writesector(dirBlockNumber,(char*)&dir); 
	} 
	printstring("Directory renamed");
}


void removeDirectoryCascade(int block) 
{ 
	directoryBlock dir; 
	int i,j, fileStart,fileSize; 
	readsector(block, (char *)&dir); 
	if(block == 0) 
	{ 
		return; 
	} 
	for(i=0; i<DIRECTORY_ENTRY_COUNT;i++) 
	{
		if(dir.entries[i].fileType == IS_DIRECTORY) 
		{ 
			removeDirectoryCascade(dir.entries[i].fileBlockStart); 
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
		    if(i == DIRECTORY_ENTRY_COUNT-1 )
			{
				return;
			}
		} 
	} 
}

void removeDirectory(char * path)
{
	unsigned int j,fileStart,fileSize;
	directoryBlock blockToErase;
	unsigned int removableBlock,i;
	char *blockPath = getPath(path,1);
	char *dirToRemove = getPath(path,2);
	unsigned int blockToRemove = findFileBlockInPath(blockPath);
	readsector(blockToRemove,(char*)&blockToErase);
	removableBlock = findFileInDirectory(blockToErase,dirToRemove);
	
	if(removableBlock == -1)
	{
		printstring("Directory does not exist\n");
		return;
	}

	if(blockToErase.entries[removableBlock].fileType== IS_DIRECTORY)
	{
		removeDirectoryCascade(blockToErase.entries[removableBlock].fileBlockStart);
		setFreeBlock(blockToErase.entries[removableBlock].fileBlockStart);
	}
	else if(blockToErase.entries[removableBlock].fileType == IS_FILE) 
 	{ 
 		fileStart = blockToErase.entries[removableBlock].fileBlockStart; 
 		fileSize = blockToErase.entries[removableBlock].fileBlockSize; 
 		for(j=fileStart; j<fileSize; j++ ) 
		{ 
			setFreeBlock(j);
	    } 
 	} 

 	for(i=0; i<stlen(blockToErase.entries[removableBlock].fileName); i++)
	{
		blockToErase.entries[removableBlock].fileName[i] = '\0';
	}
	blockToErase.entries[removableBlock].fileBlockSize = 0;
	blockToErase.entries[removableBlock].fileBlockStart = 0;
	writesector(blockToRemove,(char*)&blockToErase);

	printstring("Directory removed\n");
}

void copyDirCascade(int blockSource, int blockDestiny, char* pathDestiny) 
{ 
	directoryBlock dirSource, dirDestiny; 
	int i,j,x,newBlock,fileStart,fileSize; 
	char *dirName;

	readsector(blockSource, (char *)&dirSource);
	readsector(blockDestiny, (char*)&dirDestiny); 
	
	if(blockSource == ROOT_BLOCK) 
	{ 
		return; 
	} 

	for(i=0; i<DIRECTORY_ENTRY_COUNT;i++) 
	{

		if(dirSource.entries[i].fileType == IS_DIRECTORY) 
		{

			newBlock = createDirectory(pathDestiny,dirSource.entries[i].fileName);
			pathDestiny = mystrcat(pathDestiny,dirSource.entries[i].fileName);
			for(x=0; x<= DIRECTORY_ENTRY_COUNT;x++)
			{
				if(dirDestiny.entries[x].fileBlockStart==0)
				{
					dirDestiny.entries[j].fileName[0] = '\0';
					strcpy(dirName, dirDestiny.entries[j].fileName);
					dirDestiny.entries[j].fileType = IS_DIRECTORY;
					dirDestiny.entries[j].fileBlockStart = newBlock;	
					dirDestiny.entries[j].fileBlockSize = 1;
					break;
				}
			}
			writesector(blockDestiny,(char*)&dirDestiny);
			copyDirCascade(dirDestiny.entries[x].fileBlockStart,newBlock,pathDestiny); 

		} 
		else 
		{
			fileStart = dirSource.entries[i].fileBlockStart;
			fileSize = dirSource.entries[i].fileBlockSize;
			if(i == DIRECTORY_ENTRY_COUNT-1 )
				return;
		}
	} 
}

void copyDirectory(char* source, char* destiny)
{
	directoryBlock dirSource, dirDestiny;
	char * endPath;
	unsigned int blockSource, blockDestiny;
	int entrySource, newBlockDir, x;

	blockSource = findFileBlockInPath(source);
	blockDestiny = findFileBlockInPath(destiny);

	if(blockSource==0 || blockDestiny==0)
	{
		return;
	}
	endPath = getPath(source,2);
	readsector(blockSource, (char *)&dirSource);
	readsector(blockDestiny, (char*)&dirDestiny); 

	newBlockDir = createDirectory(destiny, endPath); 

	entrySource = findFileInDirectory(dirSource, endPath);

	if(dirSource.entries[entrySource].fileType!= IS_FILE)
	{

		newBlockDir = createDirectory(endPath,dirSource.entries[entrySource].fileName);
		endPath = mystrcat(endPath,dirSource.entries[entrySource].fileName);
		for(x=0; x<= DIRECTORY_ENTRY_COUNT;x++)
		{
			if(dirDestiny.entries[x].fileBlockStart==0)
			{
				dirDestiny.entries[x].fileBlockStart = newBlockDir;
				break;
			}
		}
		writesector(blockDestiny,(char*)&dirDestiny);
		copyDirCascade(dirDestiny.entries[x].fileBlockStart,newBlockDir,endPath); 
	}

 	printstring("Copied Directory\n"); 
}

void moveDirectory (char* sourcePath, char* destinyPath)
{
	copyDirectory(sourcePath,destinyPath);
	removeDirectory(sourcePath);
}


//-------------------------Functions for Files---------------------------------------
void createEmptyFile(fileBlock *newFile)
{
	newFile->size = 0;
	newFile->type = IS_FILE;
	newFile->data[0] = '\0';
}

void writeNewFile(fileBlock *file, char fileData [], int blockStart)
{
	int dataLenght, module, blocksToWrite, posData , i = 0;
	int j=0;
	int x=0;
	char tempData[BLOCK_SIZE];
	dataLenght = len(fileData);
	blocksToWrite = dataLenght/BLOCK_SIZE;
	module = mod(blocksToWrite, BLOCK_SIZE);

	posData = BLOCK_SIZE - 20;
	if(module != 0)
	{
		blocksToWrite = blocksToWrite + 1;
	}
	if (blocksToWrite == 0)
	{
		blocksToWrite = 1;
	}

    file->size = blocksToWrite;
	while(i<blocksToWrite)
	{
		if(i == 0)
		{
			writesector(blockStart, (char *)&file);
			blockStart++;
		}
		else
		{
			j=posData;
			while(j < posData + BLOCK_SIZE)
			{
				tempData[x] = fileData[j];
				j++;
				x++;
			}

			writesector(blockStart, tempData);
			blockStart++;
			posData = posData + BLOCK_SIZE;
		}
		i++;
	}
}

int FileExist(directoryBlock dirBlock, char *fileName)
{
	int exist = 0,i;
	for( i=0; i<DIRECTORY_ENTRY_COUNT; i++)
	{
		if(myStrCmp(dirBlock.entries[i].fileName,fileName) == 1)
		{
			exist = 1;
			break;
		}
	}
	return exist;	
}

void createFile(char * path, char* fileName, char acess[], char *data)
{
	int i;
	directoryBlock destinationDirectory;
	fileBlock file;
	unsigned int newDirBlockNumber;
	unsigned int destinationDirBlockNumber;
	int comp;
	comp = myStrCmp(path, "-");
	if(comp == 1)
	{
		destinationDirBlockNumber = ROOT_BLOCK;
	}
	else
	{
		destinationDirBlockNumber= findFileBlockInPath(path);
	}

	if(destinationDirBlockNumber==-1)
	{
		printstring("Directory does not exist\n");
		return ;
	}
	
	readsector(destinationDirBlockNumber,(char*)&destinationDirectory);
	if(FileExist(destinationDirectory,fileName) == 1)
	{
		printstring("File already exist\n");
		return;
	}

	newDirBlockNumber = AllocateBlock();
	createEmptyFile(file);
	file.type = IS_FILE;
	strcpy(fileName, file.fileName);
	strcpy(acess, file.acessRights);
	strcpy(data, file.data);
    writeNewFile(file, data, newDirBlockNumber);
	
	for(i=0;i<DIRECTORY_ENTRY_COUNT;i++)
	{
		if(destinationDirectory.entries[i].fileBlockStart==0)
		{
			strcpy(destinationDirectory.entries[i].fileName,file.fileName);
			destinationDirectory.entries[i].fileType=IS_FILE;
			destinationDirectory.entries[i].fileBlockStart = newDirBlockNumber;
			destinationDirectory.entries[i].fileBlockSize = file.size;
			break;
		}
	}
	writesector(destinationDirBlockNumber,(char*)&destinationDirectory);
	
	printstring("File created\n");
	return ;
}

void removeFile(char *path, char *fileName)
{
	unsigned int j;
	int fileStart;
	int fileSize;
	directoryBlock blockPath;
	unsigned int removableBlock;
	unsigned int blockPathNumber = findFileBlockInPath(blockPath);
	readsector(blockPathNumber,(char*)&blockPath);
	removableBlock = findFileInDirectory(blockPath,fileName);
	
	if (removableBlock == -1)
	{
		printstring("File does not exist\n");
		return;
	}


	if(blockPath.entries[removableBlock].fileType == IS_FILE) 
 	{ 
 		fileStart = blockPath.entries[removableBlock].fileBlockStart; 
 		fileSize = blockPath.entries[removableBlock].fileBlockSize; 
 		for(j=0; j<fileSize; j++ ) 
		{ 
			setFreeBlock(fileStart);
			fileStart++;
	    } 
	    printstring("File removed\n");
 	} 
}

void loadFile(char *path, char *fileName)
{
	unsigned int j;
	int fileStart;
	int fileSize;
	fileBlock file;
	char *temData;
	directoryBlock blockPath;
	unsigned int fileBlockNumber;
	unsigned int blockPathNumber = findFileBlockInPath(blockPath);
	readsector(blockPathNumber,(char*)&blockPath);
	fileBlockNumber = findFileInDirectory(blockPath,fileName);
	j=0;
	if(blockPath.entries[fileBlockNumber].fileType == IS_FILE) 
 	{ 
 		fileStart = blockPath.entries[fileBlockNumber].fileBlockStart; 
 		fileSize = blockPath.entries[fileBlockNumber].fileBlockSize; 
 		while(j<fileSize)
 		{
 			if(j==0)
 			{
 				readsector(fileStart, (char *)&file);
 				printstring("File Name: ");
 				printstring(file.fileName);
 				printstring(" \nFile Type: ");
 				printstring(file.type);
 				printstring(" \nFile Size: ");
 				printstring(file.size);
 				printstring(" \nFile Access: ");
 				printstring(file.acessRights);
 				printstring(" \nFile Data: ");
 				printstring(file.data);
 			}
 			else
 			{
 				readsector(fileStart, temData);
 				printstring(temData);
 			}
 			j++;
			fileStart++;
 		}
 	}
}
