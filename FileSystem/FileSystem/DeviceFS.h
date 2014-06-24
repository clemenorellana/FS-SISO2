#pragma once
using namespace std;

#define BLOCKSIZE  4096

class DeviceFS
{
	private:
	 char * filePath;
public:
	DeviceFS(char * filePath);
	~DeviceFS(void);
	void createDevice(unsigned int sizeInMb);
	void readBlock(unsigned int blockNumber,void * blockData);
	void writeBlock(unsigned int blockNumber,void * blockData);
	unsigned int getTotalBlocks();
};

