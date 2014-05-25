#include "basicfs.h"

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