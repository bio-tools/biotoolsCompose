#ifndef __IO_FUNCTIONS_H__
#define __IO_FUNCTIONS_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Updates the contents of a buffer by copying the bufpos residu to the beginning, and adding the rest from file */
int update_pepxml_buffer(char* buffer, char* bufpos, FILE* file_handle, int buffersize);

/* Copies data from the input file to the output file */
void copy_input_output(char* buffer, int buf_len, FILE* inputf, FILE* outputf, int copy_len);

#ifdef __cplusplus
}
#endif

#endif
