#ifndef __XML_STRING_FUNCTIONS_H__
#define __XML_STRING_FUNCTIONS_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Returns the first position of the substring in the string */
int strpos(char* str, char* substr);

/* Returns the first position of the substring in the string, bounded by the window */
int strnpos(char* str, char* substr, int window);

/* Clones a string */
char* strclone(char* src);

#ifdef __cplusplus
}
#endif

#endif
