#ifndef __PEPXML_WRITE_FUNCTIONS_H__
#define __PEPXML_WRITE_FUNCTIONS_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#include "pepXMLStructures.h"
#include "pepXMLDelegates.h"

#define WRITE_BUFF_SIZE 129472	/* 128KB reading buffer as standard */

/* Writes the entire file. Copies all residual info from the file recorded in pepxml_file */
void write_msms_pipeline_analysis(pmsms_pipeline_analysis pepxml_file, char* dest_file_name, pdelegate_list dlist);

/* Writes all analysis_summary structures */
void print_analysis_summary(pmsms_pipeline_analysis pepxml_file, FILE* output, pdelegate_list dlist);

/* Writes a single analysis_summary element */
void print_analysis_summary_element(analysis_summary analysis, FILE* output, pdelegate_list dlist);

/* Writes the dataset derivation structures */
void print_dataset_derivation(pmsms_pipeline_analysis pepxml_file, FILE* output);

/* Writes the run summary structures */
void print_run_summary(pmsms_pipeline_analysis pepxml_file, FILE* output, pdelegate_list dlist);

/* Writes a single run_sammary element */
void print_run_summary_element(msms_run_summary summary, FILE* output, char *input, pdelegate_list dlist);

/* Writes the sample enzyme structure */
void print_sample_enzyme(msms_run_summary rsummary, FILE* output, char *input);	

/* Writes the search summary structures */
void print_search_summary(msms_run_summary rsummary, FILE* output, char *input);	

/* Writes the analysis timestamp structures */
void print_analysis_timestamp(msms_run_summary rsummary, FILE* output, char *input);	

/* Writes the spectrum query structures */
void print_spectrum_query(msms_run_summary rsummary, FILE* output, pdelegate_list dlist);	

/* Writes a single spectrum query element */
void print_spectrum_query_element(spectrum_query squery, FILE* output, pdelegate_list dlist);	

/* Writes the search result structures */
void print_search_result(spectrum_query squery, FILE* output, pdelegate_list dlist);

/* Writes the search hit structures */
void print_search_hit(search_result sresult, FILE* output, pdelegate_list dlist);

/* Writes a single search hit element */
void print_search_hit_element(search_hit s_hit, FILE* output, pdelegate_list dlist);

/* Writes the alternative protein structures */
void print_alternative_protein(search_hit s_hit, FILE* output);

/* Writes the modification info structures */
void print_modification_info(search_hit s_hit, FILE* output);

/* Writes the modified aminoacid mass structure */
void print_mod_aminoacid_mass(modification_info minfo, FILE* output);

/* Writes the search score structures */
void print_search_score(search_hit s_hit, FILE* output);

/* Writes the analysis result structures */
void print_analysis_result(search_hit s_hit, FILE* output, pdelegate_list dlist);

/* Writes the search score structures */
void print_parameter(search_hit s_hit, FILE* output);

#ifdef __cplusplus
}
#endif

#endif
