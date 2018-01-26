#include <stdlib.h>
#include <stdio.h>
//#include <math.h>
#include <string.h>

#include "StringFunctions.h"


/* Returns the first position of the substring in the string */
int strpos(char* str, char* substr)
{
	char* straddr = strstr(str, substr);

	if (!straddr)
		return -1;
	else
		return (straddr - str) / (sizeof(char));

}// int strpos(char* str, char* substr)


/* Returns the first position of the substring in the string within a window bounded by n */
int strnpos(char* str, char* substr, int window)
{	
	int retval = strpos(str, substr);

	if (retval > window)
		retval = -1;

	return retval;
}// int strnpos(char* str, char* substr, int window)


/* Clones a string */
char* strclone(char* src)
{
	char* retval = (char*) malloc((strlen(src)+1) * sizeof(char));
	strcpy(retval, src);
	return retval;

}/* char* strclone(char* src) */
