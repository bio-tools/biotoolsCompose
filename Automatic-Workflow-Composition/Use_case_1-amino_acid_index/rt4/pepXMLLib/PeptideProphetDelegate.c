#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "PeptideProphetDelegate.h"
#include "StringFunctions.h"
#include "XMLFunctions.h"

static int parameter_alloc_count = 1;

/* Function that creates and configures a delegate */
pdelegate_type make_peptide_prophet_result_delegate()
{
	return make_delegate(PP_RESULT_KEY, PP_RESULT_CONTEXT, read_peptide_prophet_result, write_peptide_prophet_result, peptide_prophet_result_property);

}// pdelegate_type make_peptide_prophet_result_delegate()


/* Parses parameter_array structure */
void read_parameter_structure(ppeptide_prophet_result retval, char* beginptr)
{
	if (!(retval->parameter_array)) {
		retval->parameter_array = malloc(sizeof(pp_parameter));
		retval->parameter_count = 0;	
	}/* if */

	/* Filtering attributes */
	retval->parameter_array[retval->parameter_count].name = get_xml_attribute_value(beginptr, PP_PARAMETER_ATTRIB_NAME);
	retval->parameter_array[retval->parameter_count].value = get_xml_attribute_value(beginptr, PP_PARAMETER_ATTRIB_VALUE);	
	retval->parameter_array[retval->parameter_count].type = get_xml_attribute_value(beginptr, PP_PARAMETER_ATTRIB_TYPE);
				
	/* Updating counters and making more room for future structures */
	retval->parameter_count += 1;
	if (retval->parameter_count == parameter_alloc_count) {
		parameter_alloc_count *= 2;
		retval->parameter_array = realloc(retval->parameter_array, parameter_alloc_count * sizeof(pp_parameter));
	}/* if */	

}/* void parse_parameter_structure(psearch_hit retval, char* beginptr, FILE* finput) */


/* Function that reads the contents of the tags and makes a strucs */
void* read_peptide_prophet_result(char* contents)
{
	char *tempstr, *endstr;
	peptide_prophet_result* ppres = (peptide_prophet_result*) malloc(sizeof(peptide_prophet_result));

	// Filter attributes
	tempstr = get_xml_attribute_value(contents, PP_RESULT_ATTRIB_PROBABILITY);
	ppres->probability = strtod(tempstr, NULL);
	free(tempstr);

	ppres->all_ntt_prob = get_xml_attribute_value(contents, PP_RESULT_ATTRIB_ALL_NTT_PROB);
	ppres->analysis = get_xml_attribute_value(contents, PP_RESULT_ATTRIB_ANALYSIS);

	ppres->parameter_array = NULL;
	ppres->parameter_count = 0;

	tempstr = strstr(contents, PP_PARAMETER_OTAG);
	while (tempstr != NULL) {		
		endstr = strstr(tempstr, PP_PARAMETER_CTAG);
		endstr[0] = '\0';	
		read_parameter_structure(ppres, tempstr);
		tempstr = endstr+1;
		tempstr = strstr(tempstr, PP_PARAMETER_OTAG);
	}// while

	ppres->parameter_array = realloc(ppres->parameter_array, ppres->parameter_count * sizeof(pp_parameter));
	parameter_alloc_count = 1;

	return (void*)(ppres);
}// void* read_peptide_prophet_result(char* contents)

/* Function that reads the contents of the tags and makes a strucs */
char* write_peptide_prophet_result(void* res)
{
	int i;
	char tmpbuffer[1000];
	char* walkptr = tmpbuffer;
	peptide_prophet_result* ppres = (peptide_prophet_result*)res;

	// Printing header and required attributes
	sprintf(walkptr, "%s %s=\"%.6f\"", PP_RESULT_OTAG, 
		PP_RESULT_ATTRIB_PROBABILITY, ppres->probability);
	walkptr = tmpbuffer + strlen(tmpbuffer);

	// Printing optional attributes
	if (ppres->all_ntt_prob) {
		sprintf(walkptr, " %s=\"%s\"", PP_RESULT_ATTRIB_ALL_NTT_PROB, ppres->all_ntt_prob);
		walkptr = tmpbuffer + strlen(tmpbuffer);
	}// if
	if (ppres->analysis) {
		sprintf(walkptr, " %s=\"%s\"", PP_RESULT_ATTRIB_ANALYSIS, ppres->analysis);
		walkptr = tmpbuffer + strlen(tmpbuffer);
	}// if
	sprintf(walkptr, ">\n");
	walkptr = tmpbuffer + strlen(tmpbuffer);

	// Printing structures
	for (i=0; i<ppres->parameter_count; i++) {
		sprintf(walkptr, "%s %s=\"%s\" %s=\"%s\"", PP_PARAMETER_OTAG, 
			PP_PARAMETER_ATTRIB_NAME, ppres->parameter_array[i].name,
			PP_PARAMETER_ATTRIB_VALUE, ppres->parameter_array[i].value);
		walkptr = tmpbuffer + strlen(tmpbuffer);
		if (ppres->parameter_array[i].type) {
			sprintf(walkptr, " %s=\"%s\"", PP_PARAMETER_ATTRIB_TYPE, ppres->parameter_array[i].type); 
			walkptr = tmpbuffer + strlen(tmpbuffer);
		}// if
		sprintf(walkptr, "%s\n", PP_PARAMETER_CTAG);
		walkptr = tmpbuffer + strlen(tmpbuffer);
	}// for

	sprintf(walkptr, "%s>\n", PP_RESULT_CTAG);

	return strclone(tmpbuffer);

}// char* write_peptide_prophet_result(void* res)

/* Function that returns the property of a peptideprophet score */
void* peptide_prophet_result_property(char* key, void* res)
{
	peptide_prophet_result* ppres = (peptide_prophet_result*)res;
	double* dval = malloc(sizeof(double));
	int i;

	if (strcmp(key, PP_RESULT_ATTRIB_PROBABILITY) == 0) {
		*dval = ppres->probability;
		return (void*)dval;
	}// if
	else {
		for (i=0; i<ppres->parameter_count; i++) {
			if (strcmp(key, ppres->parameter_array[i].name) == 0) {
				*dval = strtod(ppres->parameter_array[i].value, NULL);
				return (void*)dval;
			}// if
		}// for
	}// else

	return NULL;

}// void* peptide_prophet_result_property(char* key, void* res)

/* Dextructor / Unloader of a result */
void unload_peptide_prophet_result(void* res)
{
	int i;
	peptide_prophet_result* ppres = (peptide_prophet_result*)res;

	free(ppres->all_ntt_prob);
	free(ppres->analysis);

	for (i=0; i<ppres->parameter_count; i++) {
		free(ppres->parameter_array[i].name);
		free(ppres->parameter_array[i].value);
		free(ppres->parameter_array[i].type);
	}// for 
	free(ppres->parameter_array);
	free(ppres);

}// void unload_peptide_prophet_result(void* res)
