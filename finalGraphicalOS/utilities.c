#include "utilities.h"

char * strtock (char *source, char delim)
{
	char *buff;
	int i=0;
	while (source[i] != delim || source[i] != '\0')
	{
		buff[i] = source[i];
		i++;
	}
	return buff;
}


int myStrCmp (const char *s1, const char *s2) 
{
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;

    while (*p1 != '\0') {
        if (*p2 == '\0') return  1;
        if (*p2 > *p1)   return -1;
        if (*p1 > *p2)   return  1;

        p1++;
        p2++;
    }

    if (*p2 != '\0') return -1;

    return 0;
}