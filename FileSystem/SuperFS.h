#include "DeviceFS.h"
#include "Utilities.h"
#include <iostream>

#pragma once

using namespace std;
#define ROOTBLOCK 35
struct SuperBlock
{
	char magicNumber[7];
	unsigned int totalBlocksInDisk;
	unsigned int totalFreeBlocks;
	unsigned int firstFreeBlock;
	unsigned int lastFreeBlock;
	char wastedSpace[4072];
};
struct UnusedBlock
{
	unsigned int nextFreeBlock;
	char wastedSpace[4092];
};
struct DirectoryEntry
{
	char  fileName[120];
	char fileType;
	unsigned int fileBlock;
	unsigned int fileBlockStart;
};
struct DirectoryBlock
{
	DirectoryEntry entries[32];
};

#define DIRECTORYENTRYCOUNT 32

	
class SuperFS
{
	private:
		
	Utilities * utilities;
	DeviceFS * disk;
	DirectoryBlock createEmptyDirectory();
	SuperBlock loadSuperBlock();
	DirectoryBlock loadRootDirectory();

	int findFileInDirectory(DirectoryBlock directory,char * fileName);
    unsigned int findFileBlockInPath(char * fileName);
public:
	SuperFS(DeviceFS * disk);
	~SuperFS(void);
	void Format();
	unsigned int  GetFreeSpace();

	void FreeBlock(unsigned int blockNumber);
	unsigned int AllocateBlock();

	void createDir(char * filePath,char * fileName);
	void listDir(char * filePath);


	void mkdirFS(char* name);
void renamePathDir(char *dirPath, char *newDirName);
void copyDir(char* source, char* destiny);
void setFreeBlock(unsigned int blockNumber); 
void *malloc(int sz);
char * getPathName(char * pathName, int type);
};

