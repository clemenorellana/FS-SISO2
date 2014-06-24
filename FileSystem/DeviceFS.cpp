#include "DeviceFS.h"
#include <cstdio>

using namespace std;

DeviceFS::DeviceFS(char * filePath)
{
	this->filePath = filePath;
}


DeviceFS::~DeviceFS(void)
{
}

void DeviceFS::createDevice(unsigned int sizeInMb)
{
	unsigned int sizeInBytes = sizeInMb*1024*1024;
	FILE * deviceFile; 
	fopen_s(&deviceFile,filePath,"wb");
	fseek(deviceFile,sizeInBytes-1,SEEK_SET);
	char value = 0;
	fwrite(&value,1,1,deviceFile);
	fclose(deviceFile);

}
	
void DeviceFS::readBlock(unsigned int blockNumber,void * blockData)
{
	FILE * deviceFile;
	fopen_s(&deviceFile,filePath,"rb");
	fseek(deviceFile,blockNumber*BLOCKSIZE,SEEK_SET);
	fread(blockData,1,BLOCKSIZE,deviceFile);
	fclose(deviceFile);
}

void DeviceFS::writeBlock(unsigned int blockNumber,void * blockData)
{
	FILE * deviceFile;
	fopen_s(&deviceFile,filePath,"rb+");
	fseek(deviceFile,blockNumber*BLOCKSIZE,SEEK_SET);
	fwrite(blockData,1,BLOCKSIZE,deviceFile);
	fclose(deviceFile);
}

unsigned int DeviceFS::getTotalBlocks()
{
	FILE * deviceFile;
	fopen_s(&deviceFile,filePath,"rb");
	fseek(deviceFile,0,SEEK_END);
	unsigned int totalBlocks = ftell(deviceFile)/BLOCKSIZE;
	fclose(deviceFile);
	return totalBlocks;
}
