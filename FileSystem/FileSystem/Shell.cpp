#include "Shell.h"


Shell::Shell(void)
{
}


Shell::~Shell(void)
{
}

void Shell::dohelp()
{
	printf("The commands are: \r\n\0");
	printf("  HELP or ?:  Print this message\r\n\0");
	printf("  MKDIR <DIRECTORY_PATH> <NEW_DIR_NAME>:  Create Directory\r\n\0");
	printf("  LS <DIRECTORY_PATH>:  List directories\r\n\0");
	printf("  RN <DIRECTORY_PATH> <NEW_NAME>:  Rename directory or file\r\n\0");
	printf("  RM <DIRECTORY_PATH> :  Remove directory\r\n\0");
	printf("  CPY <DIRECTORY_SOURCE_PATH> <DIRECTORY_DEST_PATH> : Copy the directory\r\n\0");
	printf("  MV <DIRECTORY_SOURCE_PATH> <DIRECTORY_DEST_PATH> :  Move the directory\r\n\0");
	printf("  MKFILE <DIRECTORY_PATH> <FILE_NAME> <ACCESS> <FILE_CONTENT> : Create a file\r\n\0");
	printf("  LOADF <DIRECTORY_PATH> <FILE_NAME> :  Load the file\r\n\0");
	printf("  RMFILE <DIRECTORY_PATH> <FILE_NAME> :  Remove the file\r\n\0");

}

void Shell::commandsShell(char * command, char * path, SuperFS fs){

    
	if(utilities.strCmp(command, "mkdir")){//create
		fs.createDir(utilities.strtok(path,' ',1),utilities.strtok(path,' ',2));
	}else if(utilities.strCmp(command,"ls")){//list
		fs.listDir(path);
	}
	else if(utilities.strCmp(command,"rm")){//remove
		fs.removeDir(path);
	}
	else if(utilities.strCmp(command,"rn")){//rename
		fs.renameDir(utilities.strtok(path,' ',1),utilities.strtok(path,' ',2));
	}
	else if(utilities.strCmp(command,"cpy")){//copy
		fs.copyDir(utilities.strtok(path,' ',1), utilities.strtok(path,' ',2), utilities.strtok(path,' ',3));
	}
	else if(utilities.strCmp(command,"mv")){//move
		fs.move(utilities.strtok(path,' ',1), utilities.strtok(path,' ',2));
	}
	else if (utilities.strCmp(command, "mkfile")){//create file
		fs.createFile(utilities.strtok(path, ' ', 1), utilities.strtok(path, ' ', 2), utilities.strtok(path, ' ', 3), utilities.mystrtok(path, ' ', 4));
	}
	else if (utilities.strCmp(command, "rmfile")){//remove file
		fs.removeFile(utilities.strtok(path, ' ', 1), utilities.strtok(path, ' ', 2));
	}
	else if (utilities.strCmp(command, "loadf")){//load file
		fs.loadFile(utilities.strtok(path, ' ', 1), utilities.strtok(path, ' ', 2));
	}
	else if(utilities.strCmp(command,"help")){
		dohelp();
	}
	else if(utilities.strCmp(command,"?")){
		dohelp();
	}
	else{
		printf("Comando no existe!");
	}

}

void Shell::doShell(){
	char * command = (char*)malloc(5);
  char * path = (char*) malloc(500);
  Utilities utilities;

  hdd1 = new DeviceFS("hdd1.disk");
  hdd1->createDevice(1);
  SuperFS fs(hdd1);

	fs.Format();
  while (!utilities.strCmp(command, "exit"))
  {
	  
	  printf("SHELL>> ");
	  scanf("%s", command);
	  char key = getchar();
	  if(key != '\n'){
		scanf("%[^\n]s", path);
	  }
	   commandsShell(command, path, fs);
	   printf("\n");
  }
}