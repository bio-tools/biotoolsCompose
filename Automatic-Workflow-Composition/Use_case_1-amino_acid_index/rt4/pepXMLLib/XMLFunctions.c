#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "IOFunctions.h"
#include "StringFunctions.h"
#include "XMLFunctions.h"


/* Function that gets the next tag that's not a system tag or a comment */
char* get_xml_tag(char* buffer, char** bufpos, FILE* file_handle, int buffersize, long* offset)
{
	char *tmpptr, *endptr;
	unsigned char unupdated = 1;

loop:	
	tmpptr = strstr(*bufpos, XML_TAG_OTAG);
	if (tmpptr && tmpptr[1] != '\0') {
		if (tmpptr[1] == '?' || tmpptr[1] == '!') {
			*bufpos = &(tmpptr[1]);
			goto loop;
		}/* if */
		endptr = strstr(tmpptr, XML_TAG_ETAG);
		if (endptr) {
			endptr[0] = '\0';
			*bufpos = &(endptr[1]);
			return tmpptr;
		}/* if */
		else if (file_handle) {
			*offset += update_pepxml_buffer(buffer, tmpptr, file_handle, buffersize);
			*bufpos = buffer;
			unupdated = 0;
			goto loop;
		}/* else */
		else 
			return NULL;
	}/* if */
	else if (unupdated && file_handle) {
		*offset += update_pepxml_buffer(buffer, *bufpos, file_handle, buffersize);
		*bufpos = buffer;
		unupdated = 0;
		goto loop;
	}/* else */
	else
		return NULL;

}/* char* get_xml_tag(char* char* buffer, char* bufpos, FILE* file_handle, int buffersize) */

/* Returns the value of a certain attribute within a string */
char* get_xml_attribute_value(char* str, char* attribute)
{
	char *tmpstr, *fullatt, *retval, *endptr;
	int newlen = 0;

	/* Attribute construction */
	fullatt = (char*)(malloc(strlen(attribute)+3 * sizeof(char)));
	strcpy(fullatt, attribute);
	fullatt[strlen(attribute)] = '=';
	fullatt[strlen(attribute)+1] = '\"';
	fullatt[strlen(attribute)+2] = '\0';

	/* Attribute comparison */
	tmpstr = strstr(str, fullatt);
	if (!tmpstr)
		return NULL;

	/* Freeing the attribute and checking for the end */
	free(fullatt);
	tmpstr = tmpstr+strlen(attribute)+2;
	endptr = strstr(tmpstr, "\"");
	if (!endptr)
		return NULL;

	/* Value found so copying it */
	newlen = (endptr - tmpstr)+1;
	retval = (char*) malloc(newlen*sizeof(char));
	memcpy (retval, tmpstr, newlen);
	retval[newlen-1] = '\0';
	
	return retval;

}/* char* get_attribute_string(char* str, char* attribute) */

/* Function that gets the next tag that's not a system tag or a comment */
char* get_xml_tag_value(char* buffer, char** bufpos, FILE* file_handle, int buffersize, long* offset)
{
	char *tmpptr, *retval;
	unsigned char unupdated = 1;

loop:	
	tmpptr = strstr(*bufpos, XML_TAG_OTAG);
	if (tmpptr) {
		tmpptr[0] = '\0';
		retval = strclone(*bufpos);
		tmpptr[0] = XML_TAG_OTAG[0];
		*bufpos = tmpptr;
		return retval;			
	}/* if */
	else if (file_handle && unupdated) {
		*offset += update_pepxml_buffer(buffer, *bufpos, file_handle, buffersize);
		*bufpos = buffer;		
		unupdated = 0;
		goto loop;
	}/* else */
	else 
		return NULL;

}/* char* get_xml_tag(char* char* buffer, char* bufpos, FILE* file_handle, int buffersize) */


/* Converts a duration type to seconds */
double xml_duration_to_seconds(char* duration)
{
	double seconds = 0;		
	unsigned int i;
	char *time_start, *curr;

	i=0;
	while (i < strlen(duration) && duration[i] != TIME_SEPARATOR)
		i++;

	time_start = &(duration[i+1]);
	curr = time_start;
	while (curr[0] != '\0') {
		if (curr[0] == HOUR_SEPARATOR) {
			curr[0] = '\0';
			seconds += atoi(time_start) * 3600;
			time_start++;
			curr = time_start;
		}// if
		else if (curr[0] ==  MINUTE_SEPARATOR) {
			curr[0] = '\0';
			seconds += atoi(time_start) * 60;
			time_start++;
			curr = time_start;
		}// if
		else if (curr[0] ==  SECOND_SEPARATOR) {
			curr[0] = '\0';
			seconds += atof(time_start);
			time_start++;
			curr = time_start;
		}// if
		curr++;
	}// while 

	return seconds;

}/* double duration_to_seconds(char* duration) */


/* Converts seconds to duration type */
char* seconds_to_xml_duration(double seconds)
{
	char maxbuf[100];
		
	if (seconds >= 0)
		sprintf(maxbuf, "PT%.8fS", seconds);	

	return strclone(maxbuf);	

}
