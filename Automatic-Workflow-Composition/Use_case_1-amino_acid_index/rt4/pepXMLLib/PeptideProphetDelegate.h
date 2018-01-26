#ifndef __PEPTIDE_PROPHET_DELEGATE_H__
#define __PEPTIDE_PROPHET_DELEGATE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "pepXMLDelegates.h"

#define PP_RESULT_KEY "peptideprophet" 
#define PP_RESULT_CONTEXT "analysis_result" 
#define PP_RESULT_OTAG "<peptideprophet_result" 
#define PP_RESULT_CTAG "</peptideprophet_result" 

#define PP_RESULT_ATTRIB_PROBABILITY "probability"
#define PP_RESULT_ATTRIB_ALL_NTT_PROB "all_ntt_prob"
#define PP_RESULT_ATTRIB_ANALYSIS "analysis"

#define PP_PARAMETER_OTAG "<parameter"
#define PP_PARAMETER_CTAG "/>"
#define PP_PARAMETER_ATTRIB_NAME "name"
#define PP_PARAMETER_ATTRIB_VALUE "value"
#define PP_PARAMETER_ATTRIB_TYPE "type"

typedef struct peptide_prophet_parameter_struct
{
	char* name;
	char* value;
	char* type;

} pp_parameter, *ppp_parameter;


typedef struct peptide_prophet_result_struct
{
	char* analysis;
	char* all_ntt_prob;
	double probability;

	pp_parameter* parameter_array;	// Array of parameters
	int			  parameter_count;	// Size of said array

} peptide_prophet_result, *ppeptide_prophet_result;


/* Function that creates and configures a delegate */
pdelegate_type make_peptide_prophet_result_delegate();

/* Function that reads the contents of the tags and makes a strucs */
void* read_peptide_prophet_result(char* contents);

/* Function that reads the contents of the tags and makes a strucs */
char* write_peptide_prophet_result(void* res);

/* Function that returns the property of a peptideprophet score */
void* peptide_prophet_result_property(char* key, void* res);

/* Dextructor / Unloader of a result */
void unload_peptide_prophet_result(void* res);


#ifdef __cplusplus
}
#endif

#endif
