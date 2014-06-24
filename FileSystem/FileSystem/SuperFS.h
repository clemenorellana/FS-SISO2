#include "DeviceFS.h"
#include "Utilities.h"
#include <iostream>

#pragma once

using namespace std;
#define SUPERBLOCK 1
#define ROOTBLOCK 2
#define IS_DIRECTORY 1
#define IS_FILE 2
#define FsStart 1



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
	unsigned int fileBlockSize;
	unsigned int fileBlockStart;
};
struct DirectoryBlock
{
	DirectoryEntry entries[32];
};

struct FileBlock
{
	char fileName[20];
	unsigned int type;
	unsigned int size;//
	char acessRights[3];//xwr
	char data[481];
};

#define DIRECTORYENTRYCOUNT 32
	
class SuperFS
{
	private:
	char *SuperFS::getRealValue(char *cadena);	
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

	int createDir(char * filePath,char * fileName);
	void listDir(char * filePath);


	void mkdirFS(char* name);
	void renameDir(char *dirPath, char *newDirName);

	void setFreeBlock(unsigned int blockNumber); 
	//void *malloc(int sz);
	char * getPathName(char * pathName, int type);

	void deleteDirCascade(int block);
	void removeDir(char * path);

	void copyDirCascade(int blockSource, int blockDestino, char *pathDestino);
	void copyDir(char *pathSource, char *pathDestino,char *nameOfCopy);

	void move(char *pathSource, char *pathDestino);


	//Funciondes De Archivos
	bool FileExist(DirectoryBlock dirBlock, char *fileName);
	FileBlock createEmptyFile();
	void writeNewFile(FileBlock *file, char fileData[], int blockStart);
	void createFile(char * path, char* fileName, char acess[], char *data);
	void removeFile(char *path, char *fileName);
	void loadFile(char *path, char *fileName);
};

