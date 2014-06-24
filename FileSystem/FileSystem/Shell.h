#include "SuperFS.h"
#include "Utilities.h"

#pragma once
class Shell
{
	
public:
	Utilities utilities;
	Shell(void);
	~Shell(void);
	DeviceFS *  hdd1;
	void commandsShell(char * command, char * path, SuperFS fs);
	void doShell();
	void dohelp();
};

