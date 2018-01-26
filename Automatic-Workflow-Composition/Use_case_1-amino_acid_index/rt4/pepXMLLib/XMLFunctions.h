#ifndef __XML_FUNCTIONS_H__
#define __XML_FUNCTIONS_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <stdio.h>

#define TIME_SEPARATOR 'T'
#define HOUR_SEPARATOR 'H'
#define MINUTE_SEPARATOR 'M'
#define SECOND_SEPARATOR 'S'

#define XML_TAG_CTAG "/>" 
#define XML_TAG_ETAG ">" 
#define XML_TAG_OTAG "<" 

/* Function that gets the next tag that's not a system tag or a comment */
char* get_xml_tag(char* buffer, char** bufpos, FILE* file_handle, int buffersize, long* offset);

/* Returns the value of a certain attribute within a string */
char* get_xml_attribute_value(char* str, char* attribute);

/* Function that gets the next tag that's not a system tag or a comment */
char* get_xml_tag_value(char* buffer, char** bufpos, FILE* file_handle, int buffersize, long* offset);

/* Converts a duration type to seconds */
double xml_duration_to_seconds(char* duration);

/* Converts seconds to duration type */
char* seconds_to_xml_duration(double seconds);

#ifdef __cplusplus
}
#endif

#endif