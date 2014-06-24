#pragma once

class Utilities
{
public:
	Utilities();
	~Utilities(void);

void *malloc(int sz);

int mod(int a, int b);

char *strcpy(char* , char*);

int strCmp (char * ,char *);

int stlen(char * cadena, int start, char delim);

int qtytoken(char * token, char delim);

char * strtok(char * str, char delim, int pos);

int mystrlen(char *cadena);

char * mistrcat(char * destino,char * fuente);

};

