#ifndef __PEPXML_PARSER_H__
#define __PEPXML_PARSER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>

#include "pepXMLStructures.h"
#include "pepXMLReaderDefs.h"
#include "pepXMLDelegates.h"


#define HEADER_BUFF_SIZE 10240		/* 10KB reading buffer as standard for the pipeline analysis header*/
#define OPT_READ_BUFF_SIZE 2048		/* 2KB reading buffer as standard for optional belayed reads */
#define SUMMARY_BUFF_SIZE 20480		/* 20KB reading buffer as standard for the run summary header*/
#define READ_BUFF_SIZE 1048576		/* 1MB reading buffer as standard */
#define INIT_SPECTRUM_COUNT 2000	/* Initial spectrum count of 2000 */

/* Parses the entire file */
void parse_msms_pipeline_analysis(pmsms_pipeline_analysis retval, FILE* finput, pipeline_analysis_flags paflags, run_summary_flags rsflags, pdelegate_list dlist);

/* Parses the analysis summary structures */
void parse_analysis_summary_structure(pmsms_pipeline_analysis retval, char* beginptr, FILE* finput, pdelegate_list dlist);

/* Parses the analysis summary structures when loaded later */
void parse_analysis_summary_structure_from_file(pmsms_pipeline_analysis retval, FILE* finput, pdelegate_list dlist);

/* Parses an individual analysis summary element */
analysis_summary parse_analysis_summary(char* content, pdelegate_type deltype);

/* Parses the dataset_derivation structures */
void parse_dataset_derivation_structure(pmsms_pipeline_analysis retval, char* beginptr, FILE* finput);

/* Parses the dataset_derivation structures from file when needed at a later stage */
void parse_dataset_derivation_structure_from_file(pmsms_pipeline_analysis retval, FILE* finput);

/* Parses the pepxml_msms_run_summary elements */
void parse_run_summary_structure(pmsms_pipeline_analysis retval, char* beginptr, FILE* finput, run_summary_flags rsflags, pdelegate_list dlist);

/* Parses a single pepxml_msms_run_summary element */
msms_run_summary parse_run_summary(char* beginptr, FILE* finput, run_summary_flags rsflags, pdelegate_list dlist);

/* Parses the pepxml_msms_run_summary elements */
void parse_sample_enzyme_structure(pmsms_run_summary retval, char* beginptr, FILE* finput);

/* Parses the pepxml_search_summary elements */
void parse_search_summary_structure(pmsms_run_summary retval, char* beginptr, FILE* finput);

/* Parses individual search_summary element */
search_summary parse_search_summary(char* beginptr, FILE* finput);

/* Parses the pepxml_analysis_timestamp structure */
void parse_analysis_timestamp_structure(pmsms_run_summary retval, char* beginptr, FILE* finput);

/* Parses individual analysis_timestamp element */
analysis_timestamp parse_analysis_timestamp(char* beginptr, FILE* finput);

/* Parses sprectum_query list */
void parse_spectrum_query_structure(pmsms_run_summary retval, char* beginptr, FILE* finput, pdelegate_list dlist);

/* Parses individual sprectum_query element */
spectrum_query parse_spectrum_query(char* beginptr, FILE* finput, pdelegate_list dlist);

/* Parses search_result_array structure */
void parse_search_result_structure(pspectrum_query retval, char* beginptr, FILE* finput, pdelegate_list dlist);

/* Parses individual search result element */
search_result parse_search_result(char* beginptr, FILE* finput, pdelegate_list dlist);

/* Parses search_hit_array structure */
void parse_search_hit_structure(psearch_result retval, char* beginptr, FILE* finput, pdelegate_list dlist);

/* Parses individual search hit element */
search_hit parse_search_hit(char* beginptr, FILE* finput, pdelegate_list dlist);

/* Parses alternative_protein_array structure */
void parse_alternative_protein_structure(psearch_hit retval, char* beginptr, FILE* finput);

/* Parses individual alternative_protein element */
alternative_protein parse_alternative_protein(char* beginptr, FILE* finput);

/* Parses modification_info_array structure */
void parse_modification_info_structure(psearch_hit retval, char* beginptr, FILE* finput);

/* Parses mod_aminoacid_mass_array structure */
void parse_mod_aminoacid_mass_structure(pmodification_info retval, char* beginptr, FILE* finput);

/* Parses individual mod_aminoacid_mass element */
mod_aminoacid_mass parse_mod_aminoacid_mass(char* beginptr, FILE* finput);

/* Parses search_score_array structure */
void parse_search_score_structure(psearch_hit retval, char* beginptr, FILE* finput);

/* Parses analysis_result_array structure */
void parse_analysis_result_structure(psearch_hit retval, char* beginptr, FILE* finput, pdelegate_list dlist);

/* Parses parameter_array structure */
void parse_parameter_structure(psearch_hit retval, char* beginptr, FILE* finput);

#ifdef __cplusplus
}
#endif

#endif
