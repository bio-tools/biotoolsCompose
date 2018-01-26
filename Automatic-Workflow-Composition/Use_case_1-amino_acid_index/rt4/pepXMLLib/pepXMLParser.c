#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "pepXMLParser.h"
#include "pepXMLDefs.h"
#include "IOFunctions.h"
#include "XMLFunctions.h"
#include "StringFunctions.h"

static char  read_buffer[READ_BUFF_SIZE];
static long  offset = 0;
static long  read_chars = 0;
static char* walkptr;

static int analysis_summary_alloc_count = 1;
static int run_summary_alloc_count = 1;	

static int search_summary_alloc_count = 1;
static int analysis_timestamp_alloc_count = 1;
static int spectrum_query_alloc_count = 1;
static int search_result_alloc_count = 1;
static int search_hit_alloc_count = 1;
static int alternative_protein_alloc_count = 1;
static int mod_aminoacid_mass_alloc_count = 1;
static int search_score_alloc_count = 1;
static int analysis_result_alloc_count = 1;
static int parameter_alloc_count = 1;

int search_hit_phase_counter = 0;


/* Main interface function */
void parse_msms_pipeline_analysis(pmsms_pipeline_analysis retval, FILE* finput, pipeline_analysis_flags paflags, run_summary_flags rsflags, pdelegate_list dlist)
{	
	char* tag;	/* pointer to the most recent tag found */
	int phasecounter = 0;

	/* Global initialization */
	offset = 0;
	read_chars = 0;
	search_summary_alloc_count = 1;
	analysis_timestamp_alloc_count = 1;
	spectrum_query_alloc_count = 1;
	search_result_alloc_count = 1;
	search_hit_alloc_count = 1;
	alternative_protein_alloc_count = 1;
	mod_aminoacid_mass_alloc_count = 1;
	search_score_alloc_count = 1;
	analysis_result_alloc_count = 1;
	parameter_alloc_count = 1;
	
	/* Reading buffer in */
	read_chars += fread(read_buffer, sizeof(char), READ_BUFF_SIZE-1, finput);
	read_buffer[read_chars] = '\0';
	walkptr = read_buffer;

	/* Setting default values for optional elements */
	retval->name = NULL;
	retval->date = NULL;
	retval->summary_xml = NULL;

	retval->analysis_summary_offset = -1;
	retval->analysis_summary_length = -1;
	retval->analysis_summary_array = NULL;
	retval->analysis_summary_count = 0;

	retval->dataset_derivation_offset = -1;
	retval->dataset_derivation_length = -1;
	retval->dataset_derivation_struct = NULL;	
	
	retval->run_summary_array = NULL;
	retval->run_summary_count = 0;	

	/* Scanning till we see the end */
	tag = get_xml_tag(read_buffer, &walkptr, finput, READ_BUFF_SIZE, &offset);
	while (strstr(tag, MSMS_PIPELINE_ANALYSIS_CTAG) == NULL) {
		/* Opening tag, filter attributes */
		if (strstr(tag, MSMS_PIPELINE_ANALYSIS_OTAG) && phasecounter <= 0) {
			retval->start_offset = offset + ((int)tag - (int)read_buffer);
			retval->name = get_xml_attribute_value(tag, MSMS_PIPELINE_ANALYSIS_ATTRIB_NAME);
			retval->date = get_xml_attribute_value(tag, MSMS_PIPELINE_ANALYSIS_ATTRIB_DATE);
			retval->summary_xml = get_xml_attribute_value(tag, MSMS_PIPELINE_ANALYSIS_ATTRIB_SUMMARY_XML);
		}/* if */
		/* Optional analysis summary */
		else if (strstr(tag, ANALYSIS_SUMMARY_OTAG) && phasecounter <= 1) {
			if (retval->analysis_summary_offset < 0) {
				retval->analysis_summary_offset = offset + ((int)tag - (int)read_buffer);
			}// if
			if (paflags & load_analysis_summary) {
				parse_analysis_summary_structure(retval, tag, finput, dlist);
			}// if
			phasecounter = 1;
		}/* else if */
		/* Optional dataset derivation */
		else if (strstr(tag, DATASET_DERIVATION_OTAG) && phasecounter <= 2) {
			if (retval->dataset_derivation_offset < 0) {
				retval->dataset_derivation_offset = offset + ((int)tag - (int)read_buffer);
				if (retval->analysis_summary_offset >= 0 && retval->analysis_summary_length < 0) {
					retval->analysis_summary_length = (retval->dataset_derivation_offset - retval->analysis_summary_offset)-1;
				}// if
			}// if
			if (paflags & load_dataset_derivation) {
				parse_dataset_derivation_structure(retval, tag, finput);
			}// if
		}/* else if */
		/* Certain structure Run summary */
		else if (strstr(tag, MSMS_RUN_SUMMARY_OTAG) && phasecounter <= 3) {
			if (retval->dataset_derivation_offset >= 0 && retval->dataset_derivation_length < 0) {
				retval->dataset_derivation_length = ((offset + ((int)tag - (int)read_buffer)) - retval->dataset_derivation_offset)-1;
			}
			else if (retval->analysis_summary_offset >= 0 && retval->analysis_summary_length < 0) {
				retval->analysis_summary_length = ((offset + ((int)tag - (int)read_buffer)) - retval->dataset_derivation_offset)-1;
			}// if
			parse_run_summary_structure(retval, tag, finput, rsflags, dlist);
			phasecounter = 3;
		}/* else if */

		tag = get_xml_tag(read_buffer, &walkptr, finput, READ_BUFF_SIZE, &offset);
	}/* while */

	if (retval->analysis_summary_array)
		retval->analysis_summary_array = realloc(retval->analysis_summary_array, retval->analysis_summary_count * sizeof(analysis_summary));
	if (retval->run_summary_array)
		retval->run_summary_array = realloc(retval->run_summary_array, retval->run_summary_count * sizeof(msms_run_summary));

}/* void parse_msms_pipeline_analysis(pmsms_pipeline_analysis retval, FILE* finput, pipeline_analysis_flags paflags, run_summary_flags rsflags, pdelegate_list dlist) */


/* Parses the analysis summary structures TO BE IMPLEMENTED LATER */
void parse_analysis_summary_structure(pmsms_pipeline_analysis retval, char* beginptr, FILE* finput, pdelegate_list dlist)
{

}/* void parse_analysis_summary_structure(pmsms_pipeline_analysis retval, char* beginptr, FILE* finput, pdelegate_list dlist) */


/* Parses the analysis summary structures when loaded later */
void parse_analysis_summary_structure_from_file(pmsms_pipeline_analysis retval, FILE* finput, pdelegate_list dlist)
{
	char* tag;

	if (retval->analysis_summary_length <= 0)
		return;

	/* Reading buffer in */
	read_chars += fread(read_buffer, sizeof(char), retval->analysis_summary_length+1, finput);
	read_buffer[read_chars] = '\0';

	/* Setting default values for optional elements */	
	retval->analysis_summary_array = NULL;
	retval->analysis_summary_count = 0;

	tag = get_xml_tag(read_buffer, &walkptr, finput, OPT_READ_BUFF_SIZE, &offset);
	while (strstr(tag, ANALYSIS_SUMMARY_OTAG) != NULL) {
		parse_analysis_summary_structure(retval, tag, finput, dlist);			
		tag = get_xml_tag(read_buffer, &walkptr, finput, OPT_READ_BUFF_SIZE, &offset);
	}/* while */

	retval->analysis_summary_array = realloc(retval->analysis_summary_array, retval->analysis_summary_count * sizeof(analysis_summary));
	analysis_summary_alloc_count = 1;

}/* void parse_analysis_summary_structure_from_file(pmsms_pipeline_analysis retval, FILE* finput, pdelegate_list dlist) */


/* Parses the analysis summary structures TO BE IMPLEMENTED LATER */
void parse_dataset_derivation_structure(pmsms_pipeline_analysis retval, char* beginptr, FILE* finput)
{
	
	
}/* char* parse_dataset_derivation_structure(pmsms_pipeline_analysis retval, char* tag_contents) */


/* Parses the dataset_derivation structures from file when needed at a later stage */
void parse_dataset_derivation_structure_from_file(pmsms_pipeline_analysis retval, FILE* finput)
{
	char* tag;

	if (retval->dataset_derivation_length <= 0)
		return;
		
	/* Reading buffer in */
	read_chars += fread(read_buffer, sizeof(char), retval->dataset_derivation_length+1, finput);
	read_buffer[read_chars] = '\0';

	/* Setting default values for optional elements */	
	retval->dataset_derivation_struct = NULL;	

	tag = get_xml_tag(read_buffer, &walkptr, finput, OPT_READ_BUFF_SIZE, &offset);
	if (strstr(tag, DATASET_DERIVATION_OTAG) != NULL) {
		parse_dataset_derivation_structure(retval, tag, finput);			
	}/* if */

}/* void parse_dataset_derivation_structure_from_file(pmsms_pipeline_analysis retval, FILE* finput) */


/* Parses the pepxml_msms_run_summary elements */
void parse_run_summary_structure(pmsms_pipeline_analysis retval, char* beginptr, FILE* finput, run_summary_flags rsflags, pdelegate_list dlist)
{
	if (!(retval->run_summary_array)) {
		retval->run_summary_array = malloc(sizeof(msms_run_summary));
		retval->run_summary_count = 0;	
	}/* if */

	/* Parsing the body */	
	retval->run_summary_array[retval->run_summary_count] = parse_run_summary(beginptr, finput, rsflags, dlist);
				
	/* Updating counters and making more room for future structures */
	retval->run_summary_count += 1;
	if (retval->run_summary_count == run_summary_alloc_count) {
		run_summary_alloc_count *= 2;
		retval->run_summary_array = realloc(retval->run_summary_array, run_summary_alloc_count * sizeof(msms_run_summary));
	}/* if */		

}/* void parse_run_summary_structure(pmsms_pipeline_analysis retval, FILE* finput, run_summary_flags rsflags, pdelegate_list dlist, long offset) */


/* Parses a single pepxml_msms_run_summary element */
msms_run_summary parse_run_summary(char* beginptr, FILE* finput, run_summary_flags rsflags, pdelegate_list dlist)
{	
	char *tag;		
	msms_run_summary retval;
	int phasecounter = 0;
	long offset_t;

	/* Setting the optional values to their default value */
	retval.sample_enzyme_offset = -1;
	retval.sample_enzyme_length = -1;
	retval.sample_enzyme_element = NULL;

	retval.search_summary_offset = -1;
	retval.search_summary_length = -1;
	retval.search_summary_array = NULL;
	retval.search_summary_count = 0;

	retval.analysis_timestamp_offset = -1;
	retval.analysis_timestamp_length = -1;
	retval.analysis_timestamp_array = NULL;
	retval.analysis_timestamp_count = 0;

	retval.spectrum_query_array = NULL;
	retval.spectrum_query_count = 0;

	/* Filling Attributes */
	retval.base_name = get_xml_attribute_value(beginptr, MSMS_RUN_SUMMARY_ATTRIB_BASE_NAME);
	retval.raw_data_type = get_xml_attribute_value(beginptr, MSMS_RUN_SUMMARY_ATTRIB_RAW_DATA_TYPE);
	retval.raw_data = get_xml_attribute_value(beginptr, MSMS_RUN_SUMMARY_ATTRIB_RAW_DATA);
	retval.msManufacturer = get_xml_attribute_value(beginptr, MSMS_RUN_SUMMARY_ATTRIB_MSMANUFACTURER);
	retval.msModel = get_xml_attribute_value(beginptr, MSMS_RUN_SUMMARY_ATTRIB_MSMODEL);
	retval.msIonization = get_xml_attribute_value(beginptr, MSMS_RUN_SUMMARY_ATTRIB_MSIONIZATION);
	retval.msMassAnalyzer = get_xml_attribute_value(beginptr, MSMS_RUN_SUMMARY_ATTRIB_MSMASSANALYZER);
	retval.msDetector = get_xml_attribute_value(beginptr, MSMS_RUN_SUMMARY_ATTRIB_MSDETECTOR);
	
	/* Scanning till we see the end */
	tag = get_xml_tag(read_buffer, &walkptr, finput, READ_BUFF_SIZE, &offset);
	
	while (strstr(tag, MSMS_RUN_SUMMARY_CTAG) == NULL) {
		/* Optional sample enzyme */
		if (strstr(tag, SAMPLE_ENZYME_OTAG) && phasecounter <= 0) {
			retval.sample_enzyme_offset = offset + ((int)tag - (int)read_buffer);
			if (rsflags & load_sample_enzyme) {
				parse_sample_enzyme_structure(&retval, tag, finput);
			}// if
			phasecounter = 1;	/* only one element allowed */
		}/* if */
		/* Optional search summary */
		else if (strstr(tag, SEARCH_SUMMARY_OTAG) && phasecounter <= 2) {
			if (retval.search_summary_offset < 0) {
				retval.search_summary_offset = offset + ((int)tag - (int)read_buffer);
				if (retval.sample_enzyme_offset >= 0 && retval.sample_enzyme_length < 0) {
					retval.sample_enzyme_length = (retval.search_summary_offset - retval.sample_enzyme_offset)-1;
				}// if
			}// if
			if (rsflags & load_search_summary){
				parse_search_summary_structure(&retval, tag, finput);
			}
			phasecounter = 2;	
		}/* else if */
		/* Optional analysis timestamp */
		else if (strstr(tag, ANALYSIS_TIMESTAMP_OTAG) && phasecounter <= 3) {
			if (retval.analysis_timestamp_offset < 0) {
				retval.analysis_timestamp_offset = offset + ((int)tag - (int)read_buffer);
				if (retval.search_summary_offset >= 0 && retval.search_summary_length < 0) {
					retval.search_summary_length = (retval.analysis_timestamp_offset - retval.search_summary_offset)-1;
				}// if
				else if (retval.sample_enzyme_offset >= 0 && retval.sample_enzyme_length < 0) {
					retval.sample_enzyme_length = (retval.analysis_timestamp_offset - retval.sample_enzyme_offset)-1;
				}// if
			}// if
			if (rsflags & load_analysis_timestamp){
				parse_analysis_timestamp_structure(&retval, tag, finput);
			}
			phasecounter = 3;
		}/* else if */
		else if (strstr(tag, SPECTRUM_QUERY_OTAG) && phasecounter <= 4) {
			offset_t = offset + ((int)tag - (int)read_buffer);

			if (retval.analysis_timestamp_offset >= 0 && retval.analysis_timestamp_length < 0) {
				retval.analysis_timestamp_length = (offset_t - retval.analysis_timestamp_offset)-1;
			}// else if
			else if (retval.search_summary_offset >= 0 && retval.search_summary_length < 0) {
				retval.search_summary_length = (offset_t - retval.search_summary_offset)-1;
			}// else if
			else if (retval.sample_enzyme_offset >= 0 && retval.sample_enzyme_length < 0) {
				retval.sample_enzyme_length = (offset_t - retval.sample_enzyme_offset)-1;
			}// if
			
			parse_spectrum_query_structure(&retval, tag, finput, dlist);
			phasecounter = 4;
		}/* else if */
		
		tag = get_xml_tag(read_buffer, &walkptr, finput, READ_BUFF_SIZE, &offset);
		
	}/* while */

	if (retval.search_summary_array){
		retval.search_summary_array = realloc(retval.search_summary_array, retval.search_summary_count * sizeof(search_summary));
	}
	search_summary_alloc_count = 1;
	
	if (retval.analysis_timestamp_array){
		retval.analysis_timestamp_array = realloc(retval.analysis_timestamp_array, retval.analysis_timestamp_count * sizeof(analysis_timestamp));
	}
		
	analysis_timestamp_alloc_count = 1;
	
	if (retval.spectrum_query_array){
		retval.spectrum_query_array = realloc(retval.spectrum_query_array, retval.spectrum_query_count * sizeof(spectrum_query));
	}
		
	spectrum_query_alloc_count = 1;
		
	return retval;

}/* msms_run_summary parse_run_summary(char* beginptr, FILE* finput, run_summary_flags rsflags, pdelegate_list dlist) */


/* Parses the sample_enzyme elements TO BE IMPLEMENTED*/
void parse_sample_enzyme_structure(pmsms_run_summary retval, char* beginptr, FILE* finput)
{

}/* char* parse_sample_enzyme_structure(pmsms_run_summary retval, char* tag_contents) */


/* Parses the pepxml_search_summary elements TO BE IMPLEMENTED*/
void parse_search_summary_structure(pmsms_run_summary retval, char* beginptr, FILE* finput)
{

}/* char* parse_search_summary_structure(pmsms_run_summary retval, char* tag_contents) */


/* Parses individual search_summary element TO BE IMPLEMENTED*/
search_summary parse_search_summary(char* beginptr, FILE* finput)
{
	search_summary ss;
	
	ss.contents = NULL;

	return ss;

}/* search_summary parse_search_summary(char* tag_contents) */


/* Parses the pepxml_analysis_timestamp structure */
void parse_analysis_timestamp_structure(pmsms_run_summary retval, char* beginptr, FILE* finput)
{	

}/* void parse_analysis_timestamp_structure(pmsms_run_summary retval, char* beginptr, FILE* finput) */


/* Parses individual analysis_timestamp element */
analysis_timestamp parse_analysis_timestamp(char* beginptr, FILE* finput)
{
	analysis_timestamp at;

	at.contents = NULL;

	return at;

}/* analysis_timestamp parse_analysis_timestamp(char* beginptr, FILE* finput) */


/* Parses spectrum query list */
void parse_spectrum_query_structure(pmsms_run_summary retval, char* beginptr, FILE* finput, pdelegate_list dlist)
{
	if (!(retval->spectrum_query_array)) {
		retval->spectrum_query_array = malloc(sizeof(spectrum_query));
		retval->spectrum_query_count = 0;	
	}/* if */

	/* Retrieving the spectrum query */
	retval->spectrum_query_array[retval->spectrum_query_count] = parse_spectrum_query(beginptr, finput, dlist);
				
	/* Updating counters and making more room for future structures */
	retval->spectrum_query_count += 1;
	if (retval->spectrum_query_count == spectrum_query_alloc_count) {
		spectrum_query_alloc_count *= 2;
		retval->spectrum_query_array = realloc(retval->spectrum_query_array, spectrum_query_alloc_count * sizeof(spectrum_query));
	}/* if */	

}/* char* parse_spectrum_query_structure(pmsms_run_summary retval, FILE* finput, pdelegate_list dlist) */


/* Parses individual sprectum_query element */
spectrum_query parse_spectrum_query(char* beginptr, FILE* finput, pdelegate_list dlist)
{
	char *tmpptr, *tag;
	spectrum_query retval;

	/* Setting the optional values to their default value */
	retval.retention_time_sec = -1;
	retval.search_result_array = NULL;
	retval.search_result_count = 0;
		
	/* Parse spectrum */
	retval.spectrum = get_xml_attribute_value(beginptr, SPECTRUM_QUERY_ATTRIB_SPECTRUM);
	
	/* Parse and convert start scan */
	tmpptr = get_xml_attribute_value(beginptr, SPECTRUM_QUERY_ATTRIB_START_SCAN);
	retval.start_scan = atoi(tmpptr);
	free(tmpptr);

	/* Parse and convert end scan */
	tmpptr = get_xml_attribute_value(beginptr, SPECTRUM_QUERY_ATTRIB_END_SCAN);
	retval.end_scan = atoi(tmpptr);
	free(tmpptr);

	/* Parse retention time in seconds */
	tmpptr = get_xml_attribute_value(beginptr, SPECTRUM_QUERY_ATTRIB_RETENTION_TIME_SEC);
	if (tmpptr) {
		retval.retention_time_sec = strtod(tmpptr, NULL);
		free(tmpptr);
	}/* if */

	retval.activation_method = get_xml_attribute_value(beginptr, SPECTRUM_QUERY_ATTRIB_ACTIVATION_METHOD);

	/* Parse and convert precursor neutral mass*/
	tmpptr = get_xml_attribute_value(beginptr, SPECTRUM_QUERY_ATTRIB_PRECURSOR_NEUTRAL_MASS);
	retval.precursor_neutral_mass = strtod(tmpptr, NULL);
	free(tmpptr);

	/* Parse and convert assumed charge */
	tmpptr = get_xml_attribute_value(beginptr, SPECTRUM_QUERY_ATTRIB_ASSUMED_CHARGE);
	retval.assumed_charge = atoi(tmpptr);
	free(tmpptr);

	retval.search_specification = get_xml_attribute_value(beginptr, SPECTRUM_QUERY_ATTRIB_SEARCH_SPECIFICATION);

	tmpptr = get_xml_attribute_value(beginptr, SPECTRUM_QUERY_ATTRIB_INDEX);
	retval.index = atoi(tmpptr);
	free(tmpptr);

	/* Scanning till we see the end */
	tag = get_xml_tag(read_buffer, &walkptr, finput, READ_BUFF_SIZE, &offset);
	while (strstr(tag, SPECTRUM_QUERY_CTAG) == NULL) {
		/* Search result */
		if (strstr(tag, SEARCH_RESULT_OTAG)) {
			parse_search_result_structure(&retval, tag, finput, dlist);
		}/* if */
		tag = get_xml_tag(read_buffer, &walkptr, finput, READ_BUFF_SIZE, &offset);
	}/* while */

	if (retval.search_result_array)
		retval.search_result_array = realloc(retval.search_result_array, retval.search_result_count * sizeof(search_result));
	search_result_alloc_count = 1;

	return retval;

}/* pspectrum_query parse_spectrum_query(char* contents, pdelegate_list dlist) */


/* Parses search_result_array structure */
void parse_search_result_structure(pspectrum_query retval, char* beginptr, FILE* finput, pdelegate_list dlist)
{
	if (!(retval->search_result_array)) {
		retval->search_result_array = malloc(sizeof(search_result));
		retval->search_result_count = 0;	
	}/* if */

	/* Retrieving search result structure */
	retval->search_result_array[retval->search_result_count] = parse_search_result(beginptr, finput, dlist);

	/* Updating counters and making more room for future structures */
	retval->search_result_count += 1;
	if (retval->search_result_count == search_result_alloc_count) {
		search_result_alloc_count *= 2;
		retval->search_result_array = realloc(retval->search_result_array, search_result_alloc_count * sizeof(search_result));
	}/* if */	
}/* void parse_search_result_structure(pspectrum_query retval, char* beginptr, FILE* finput, pdelegate_list dlist) */


/* Parses individual search result element */
search_result parse_search_result(char* beginptr, FILE* finput, pdelegate_list dlist)
{
	search_result retval;
	char *tmpptr, *tag;

	/* Setting the optional values to their default value */
	retval.search_id = -1;
	retval.search_hit_array = NULL;
	retval.search_hit_count = 0;
		
	/* Parse search ID */
	tmpptr = get_xml_attribute_value(beginptr, SEARCH_RESULT_ATTRIB_SEARCH_ID);
	if (tmpptr) {
		retval.search_id = atoi(tmpptr);
		free(tmpptr);
	}/* if */
	/* Scanning till we see the end */
	tag = get_xml_tag(read_buffer, &walkptr, finput, READ_BUFF_SIZE, &offset);
	while (strstr(tag, SEARCH_RESULT_CTAG) == NULL) {
		/* Search result */
		if (strstr(tag, SEARCH_HIT_OTAG)) {
			parse_search_hit_structure(&retval, tag, finput, dlist);
		}/* if */
		tag = get_xml_tag(read_buffer, &walkptr, finput, READ_BUFF_SIZE, &offset);
	}/* while */

	retval.search_hit_array = realloc(retval.search_hit_array, retval.search_hit_count * sizeof(search_hit));
	search_hit_alloc_count = 1;
	return retval;

}/* search_result parse_search_result(char* beginptr, FILE* finput, pdelegate_list dlist) */


/* Parses search_hit_array structure */
void parse_search_hit_structure(psearch_result retval, char* beginptr, FILE* finput, pdelegate_list dlist)
{	
	if (!(retval->search_hit_array)) {
		retval->search_hit_array = malloc(sizeof(search_hit));
		retval->search_hit_count = 0;	
	}/* if */

	/* Filtering search hit sctructure! */
	retval->search_hit_array[retval->search_hit_count] = parse_search_hit(beginptr, finput, dlist);
				
	/* Updating counters and making more room for future structures */
	retval->search_hit_count += 1;
	if (retval->search_hit_count == search_hit_alloc_count) {
		search_hit_alloc_count *= 2;
		retval->search_hit_array = realloc(retval->search_hit_array, search_hit_alloc_count * sizeof(search_hit));
	}/* if */	

}/* void parse_search_result_structure(psearch_result retval, char* beginptr, FILE* finput, pdelegate_list dlist) */


/* Parses individual search hit element */
search_hit parse_search_hit(char* beginptr, FILE* finput, pdelegate_list dlist)
{
	search_hit retval;
	char *tmpptr, *tag;
	search_hit_phase_counter = 0;

	/* Setting the optional values to their default value */
	retval.num_matched_ions = -1;
	retval.tot_num_ions = -1;
	retval.num_tol_term = -1;
	retval.num_missed_cleavages = -1;
	retval.is_rejected = 0;
	retval.calc_pI = -1;
	retval.protein_mw = -1;
	retval.alternative_protein_array = NULL;
	retval.alternative_protein_count = 0;
	retval.modification_info_struct = NULL;
	retval.search_score_array = NULL;
	retval.search_score_count = 0;	
	retval.analysis_result_array = NULL;
	retval.analysis_result_count = 0;
	retval.parameter_array = NULL;
	retval.parameter_count = 0;

	/* Parse and convert hit rank */
	tmpptr = get_xml_attribute_value(beginptr, SEARCH_HIT_ATTRIB_HIT_RANK);
	retval.hit_rank = atoi(tmpptr);
	free(tmpptr);

	/* Parse peptide- and protein-related descriptions */
	retval.peptide = get_xml_attribute_value(beginptr, SEARCH_HIT_ATTRIB_PEPTIDE);
	retval.peptide_prev_aa = get_xml_attribute_value(beginptr, SEARCH_HIT_ATTRIB_PEPTIDE_PREV_AA);
	retval.peptide_next_aa = get_xml_attribute_value(beginptr, SEARCH_HIT_ATTRIB_PEPTIDE_NEXT_AA);
	retval.protein = get_xml_attribute_value(beginptr, SEARCH_HIT_ATTRIB_PROTEIN);

	/* Parse and convert protein number */
	tmpptr = get_xml_attribute_value(beginptr, SEARCH_HIT_ATTRIB_NUM_TOT_PROTEINS);
	retval.num_tot_proteins = atoi(tmpptr);
	free(tmpptr);

	/* Parse and convert matched ions */
	tmpptr = get_xml_attribute_value(beginptr, SEARCH_HIT_ATTRIB_NUM_MATCHED_IONS);
	if (tmpptr) {
		retval.num_matched_ions = atoi(tmpptr);
		free(tmpptr);
	}/* if */

	/* Parse and convert matched ions */
	tmpptr = get_xml_attribute_value(beginptr, SEARCH_HIT_ATTRIB_TOT_NUM_IONS);
	if (tmpptr) {
		retval.tot_num_ions = atoi(tmpptr);
		free(tmpptr);
	}/* if */

	/* Parse and convert neutral peptide mass */
	tmpptr = get_xml_attribute_value(beginptr, SEARCH_HIT_ATTRIB_CALC_NEUTRAL_PEP_MASS);
	retval.calc_neutral_pep_mass = strtod(tmpptr, NULL);
	free(tmpptr);

	/* Parse and convert mass difference */
	tmpptr = get_xml_attribute_value(beginptr, SEARCH_HIT_ATTRIB_MASSDIFF);
	retval.massdiff = strtod(tmpptr, NULL);
	free(tmpptr);

	/* Parse and convert num tol term */	
	tmpptr = get_xml_attribute_value(beginptr, SEARCH_HIT_ATTRIB_NUM_TOL_TERM);
	if (tmpptr) {
		retval.num_tol_term = atoi(tmpptr);
		free(tmpptr);
	}/* if */

	/* Parse and convert missed cleavage number */
	tmpptr = get_xml_attribute_value(beginptr, SEARCH_HIT_ATTRIB_NUM_MISSED_CLEAVAGES);
	if (tmpptr) {
		retval.num_missed_cleavages = atoi(tmpptr);
		free(tmpptr);
	}/* if */

	/* Parse and convert missed cleavage number */
	tmpptr = get_xml_attribute_value(beginptr, SEARCH_HIT_ATTRIB_IS_REJECTED);
	if (tmpptr) {
		retval.is_rejected = atoi(tmpptr);
		free(tmpptr);
	}/* if */

	retval.protein_descr = get_xml_attribute_value(beginptr, SEARCH_HIT_ATTRIB_PROTEIN_DESCR);

	/* Parse and convert calculated isoelectrical point */
	tmpptr = get_xml_attribute_value(beginptr, SEARCH_HIT_ATTRIB_CALC_PI);
	if (tmpptr) {
		retval.calc_pI = strtod(tmpptr, NULL);
		free(tmpptr);
	}/* if */

	/* Parse and convert calculated protein molecular weight */
	tmpptr = get_xml_attribute_value(beginptr, SEARCH_HIT_ATTRIB_PROTEIN_MW);
	if (tmpptr) {
		retval.protein_mw = strtod(tmpptr, NULL);
		free(tmpptr);
	}/* if */

	/* Scanning till we see the end */
	tag = get_xml_tag(read_buffer, &walkptr, finput, READ_BUFF_SIZE, &offset);

	while (strstr(tag, SEARCH_HIT_CTAG) == NULL) {
		/* Alternative proteins */
		if (strstr(tag, ALTERNATIVE_PROTEIN_OTAG)&& search_hit_phase_counter <= 1) {
			parse_alternative_protein_structure(&retval, tag, finput);
			search_hit_phase_counter = 1;
		}/* if */
		/* Modification info tag */
		else if (strstr(tag, MODIFICATION_INFO_OTAG) && search_hit_phase_counter <= 2) {
			parse_modification_info_structure(&retval, tag, finput);
			search_hit_phase_counter = 2;
		}/* else if */
		/* Search score tag */
		else if (strstr(tag, SEARCH_SCORE_OTAG) && search_hit_phase_counter <= 3) {
	    	parse_search_score_structure(&retval, tag, finput);
			search_hit_phase_counter = 3;
		}/* else if */
		/* Analysis result tag */
		else if (strstr(tag, ANALYSIS_RESULT_OTAG) && search_hit_phase_counter <= 4) {
			parse_analysis_result_structure(&retval, tag, finput, dlist);
			search_hit_phase_counter = 4;
		}/* else if */
		/* Parameter tag */
		else if (strstr(tag, PARAMETER_OTAG) && search_hit_phase_counter <= 5) {
			parse_parameter_structure(&retval, tag, finput);
			search_hit_phase_counter = 5;
		}/* else if */

		tag = get_xml_tag(read_buffer, &walkptr, finput, READ_BUFF_SIZE, &offset);
	}/* while */


	retval.alternative_protein_array = realloc(retval.alternative_protein_array, retval.alternative_protein_count * sizeof(alternative_protein));
	alternative_protein_alloc_count = 1;

	retval.search_score_array = realloc(retval.search_score_array, retval.search_score_count * sizeof(search_score));
	search_score_alloc_count = 1;

	retval.analysis_result_array = realloc(retval.analysis_result_array, retval.analysis_result_count * sizeof(analysis_result));
	analysis_result_alloc_count = 1;

	retval.parameter_array = realloc(retval.parameter_array, retval.parameter_count * sizeof(parameter));
	parameter_alloc_count = 1;

	return retval;

}/* search_result parse_search_hit(char* beginptr, FILE* finput, pdelegate_list dlist) */


/* Parses alternative_protein_array structure */
void parse_alternative_protein_structure(psearch_hit retval, char* beginptr, FILE* finput)
{	
	if (!(retval->alternative_protein_array)) {
		retval->alternative_protein_array = malloc(sizeof(alternative_protein));
		retval->alternative_protein_count = 0;	
	}/* if */

	/* Filtering search hit sctructure! */
	retval->alternative_protein_array[retval->alternative_protein_count] = parse_alternative_protein(beginptr, finput);
				
	/* Updating counters and making more room for future structures */
	retval->alternative_protein_count += 1;
	if (retval->alternative_protein_count == alternative_protein_alloc_count) {
		alternative_protein_alloc_count *= 2;
		retval->alternative_protein_array = realloc(retval->alternative_protein_array, alternative_protein_alloc_count * sizeof(alternative_protein));
	}/* if */	

}/* void parse_alternative_protein_structure(psearch_hit retval, char* beginptr, FILE* finput) */


/* Parses individual search hit element */
alternative_protein parse_alternative_protein(char* beginptr, FILE* finput)
{
	alternative_protein retval;
	char *tmpptr;

	/* Setting the optional values to their default value */
	retval.num_tol_term = -1;
	retval.protein_mw = -1;

	/* Parse peptide- and protein-related descriptions */
	retval.protein = get_xml_attribute_value(beginptr, ALTERNATIVE_PROTEIN_ATTRIB_PROTEIN);
	retval.protein_descr = get_xml_attribute_value(beginptr, ALTERNATIVE_PROTEIN_ATTRIB_PROTEIN_DESCR);

	/* Parse and convert num tol term */	
	tmpptr = get_xml_attribute_value(beginptr, ALTERNATIVE_PROTEIN_ATTRIB_NUM_TOL_TERM);
	if (tmpptr) {
		retval.num_tol_term = atoi(tmpptr);
		free(tmpptr);
	}/* if */
	
	/* Parse and convert calculated protein molecular weight */
	tmpptr = get_xml_attribute_value(beginptr, ALTERNATIVE_PROTEIN_ATTRIB_PROTEIN_MW);
	if (tmpptr) {
		retval.protein_mw = strtod(tmpptr, NULL);
		free(tmpptr);
	}/* if */

	retval.peptide_prev_aa = get_xml_attribute_value(beginptr, ALTERNATIVE_PROTEIN_ATTRIB_PEPTIDE_PREV_AA);
	retval.peptide_next_aa = get_xml_attribute_value(beginptr, ALTERNATIVE_PROTEIN_ATTRIB_PEPTIDE_NEXT_AA);

	return retval;

}/* alternative_protein parse_alternative_protein(char* beginptr, FILE* finput) */


/* Parses modification_info_array structure */
void parse_modification_info_structure(psearch_hit retval, char* beginptr, FILE* finput)
{
	char* tmpptr, *tag;
	bool mod_mass_exists = 0;

	retval->modification_info_struct = malloc(sizeof(modification_info));
	retval->modification_info_struct->mod_aminoacid_mass_array = NULL;
	retval->modification_info_struct->mod_aminoacid_mass_count = 0;
	
	/* Filtering attributes */
	tmpptr = get_xml_attribute_value(beginptr, MODIFICATION_INFO_ATTRIB_MOD_NTERM_MASS);
	if (tmpptr) {
		mod_mass_exists = 1;
		retval->modification_info_struct->mod_nterm_mass = strtod(tmpptr, NULL);
		free(tmpptr);
	}/* if */
	else{
		retval->modification_info_struct->mod_nterm_mass = -1;
	}
		
	tmpptr = get_xml_attribute_value(beginptr, MODIFICATION_INFO_ATTRIB_MOD_CTERM_MASS);
	if (tmpptr) {
		mod_mass_exists = 1;
		retval->modification_info_struct->mod_cterm_mass = strtod(tmpptr, NULL);
		free(tmpptr);
	}/* if */
	else {
		retval->modification_info_struct->mod_cterm_mass = -1;
	}
	
    tmpptr = get_xml_attribute_value(beginptr, MODIFICATION_INFO_ATTRIB_MODIFIED_PEPTIDE);
	if (tmpptr) {
		mod_mass_exists = 1;
		retval->modification_info_struct->modified_peptide = tmpptr;
		free(tmpptr);
	}/* if */
	else {
		retval->modification_info_struct->modified_peptide = NULL;
	}

	//changed: retval->modification_info_struct->modified_peptide = get_xml_attribute_value(beginptr, MODIFICATION_INFO_ATTRIB_MODIFIED_PEPTIDE);

	/* Scanning till we see the end */
	tag = get_xml_tag(read_buffer, &walkptr, finput, READ_BUFF_SIZE, &offset);
	
	if(strstr(tag, SEARCH_SCORE_OTAG) && search_hit_phase_counter <= 3){
		parse_search_score_structure(retval, tag, finput);
		search_hit_phase_counter = 3;
	}
	
	while ((!mod_mass_exists && (strstr(tag, MODIFICATION_INFO_CTAG) == NULL))  || 
		   ( mod_mass_exists && strstr(tag, MOD_AMINOACID_MASS_OTAG) && (strstr(tag, MODIFICATION_INFO_CTAG) == NULL) ) ) {

		if (strstr(tag, MOD_AMINOACID_MASS_OTAG)) {
			parse_mod_aminoacid_mass_structure(retval->modification_info_struct, tag, finput);
		}
		tag = get_xml_tag(read_buffer, &walkptr, finput, READ_BUFF_SIZE, &offset);
	}
	
	retval->modification_info_struct->mod_aminoacid_mass_array = realloc(retval->modification_info_struct->mod_aminoacid_mass_array, retval->modification_info_struct->mod_aminoacid_mass_count * sizeof(mod_aminoacid_mass));
	mod_aminoacid_mass_alloc_count = 1;
		
}/* void parse_modification_info_structure(psearch_hit retval, char* beginptr, FILE* finput) */


/* Parses mod_aminoacid_mass_array structure */
void parse_mod_aminoacid_mass_structure(pmodification_info retval, char* beginptr, FILE* finput)
{
	if (!(retval->mod_aminoacid_mass_array)) {
		retval->mod_aminoacid_mass_array = malloc(sizeof(mod_aminoacid_mass));
		retval->mod_aminoacid_mass_count = 0;	
	}/* if */

	/* Filtering mod_aminoacid_mass structure! */
	retval->mod_aminoacid_mass_array[retval->mod_aminoacid_mass_count] = parse_mod_aminoacid_mass(beginptr, finput);
				
	/* Updating counters and making more room for future structures */
	retval->mod_aminoacid_mass_count += 1;
	if (retval->mod_aminoacid_mass_count == mod_aminoacid_mass_alloc_count) {
		mod_aminoacid_mass_alloc_count *= 2;
		retval->mod_aminoacid_mass_array = realloc(retval->mod_aminoacid_mass_array, mod_aminoacid_mass_alloc_count * sizeof(mod_aminoacid_mass));
	}/* if */	

}/* void parse_mod_aminoacid_mass_structure(pmodification_info, char* beginptr, FILE* finput) */


/* Parses individual mod_aminoacid_mass element */
mod_aminoacid_mass parse_mod_aminoacid_mass(char* beginptr, FILE* finput)
{
	char *tmpptr;
	mod_aminoacid_mass retval;
	
	tmpptr = get_xml_attribute_value(beginptr, MOD_AMINOACID_MASS_ATTRIB_POSITION);
	retval.position = atoi(tmpptr);
	free(tmpptr);

	tmpptr = get_xml_attribute_value(beginptr, MOD_AMINOACID_MASS_ATTRIB_MASS);
	retval.mass = strtod(tmpptr, NULL);
	free(tmpptr);

	return retval;
	
}/* mod_aminoacid_mass parse_mod_aminoacid_mass(char* beginptr, FILE* finput) */


/* Parses search_score_array structure */
void parse_search_score_structure(psearch_hit retval, char* beginptr, FILE* finput)
{	
	char *tmpptr;
	
	if (!(retval->search_score_array)) {
		retval->search_score_array = malloc(sizeof(search_score));
		retval->search_score_count = 0;	
	}/* if */

	/* Filtering attributes */
	retval->search_score_array[retval->search_score_count].name = get_xml_attribute_value(beginptr, SEARCH_SCORE_ATTRIB_NAME);

	tmpptr = get_xml_attribute_value(beginptr, SEARCH_SCORE_ATTRIB_VALUE);

	retval->search_score_array[retval->search_score_count].value = strtod(tmpptr, NULL);
	free(tmpptr);

	retval->search_score_array[retval->search_score_count].type = get_xml_attribute_value(beginptr, SEARCH_SCORE_ATTRIB_TYPE);
			
	/* Updating counters and making more room for future structures */
	retval->search_score_count += 1;
	if (retval->search_score_count == search_score_alloc_count) {
		search_score_alloc_count *= 2;
		retval->search_score_array = realloc(retval->search_score_array, search_score_alloc_count * sizeof(search_score));
	}/* if */	

}/* void parse_search_score_structure(psearch_hit retval, char* beginptr, FILE* finput) */


/* Parses analysis_result_array structure */
void parse_analysis_result_structure(psearch_hit retval, char* beginptr, FILE* finput, pdelegate_list dlist)
{
	char *tmpptr, *contents;
	pdelegate_type delegate_t;

	
	if (!(retval->analysis_result_array)) {
		retval->analysis_result_array = malloc(sizeof(analysis_result));
		retval->analysis_result_count = 0;	
	}/* if */

	/* Filtering attributes */
	retval->analysis_result_array[retval->analysis_result_count].analysis = get_xml_attribute_value(beginptr, ANALYSIS_RESULT_ATTRIB_ANALYSIS);
	
	retval->analysis_result_array[retval->analysis_result_count].id = -1;
	tmpptr = get_xml_attribute_value(beginptr, ANALYSIS_RESULT_ATTRIB_ID);	
	if (tmpptr) {
		retval->analysis_result_array[retval->analysis_result_count].id = atoi(tmpptr);
		free(tmpptr);
	}/* if */	

	/* Filtering the unknown contents */
	tmpptr = strstr(walkptr, ANALYSIS_RESULT_CTAG);
	if (!tmpptr) {
		offset += update_pepxml_buffer(read_buffer, walkptr, finput, READ_BUFF_SIZE);
		walkptr = read_buffer;
		tmpptr = strstr(walkptr, ANALYSIS_RESULT_CTAG);
	}// if
	tmpptr[0] = '\0';
	contents = strclone(walkptr);
	tmpptr[0] = ANALYSIS_RESULT_CTAG[0];
	walkptr = tmpptr;

	/* Finding delegates, or if none, just store the string */
	delegate_t = get_delegate(retval->analysis_result_array[retval->analysis_result_count].analysis, "analysis_result", dlist);	
	if (delegate_t != NULL)
		retval->analysis_result_array[retval->analysis_result_count].hook = delegate_t->pread_function(contents);	
	else 
		retval->analysis_result_array[retval->analysis_result_count].hook = contents;	
				
	/* Updating counters and making more room for future structures */
	retval->analysis_result_count += 1;
	if (retval->analysis_result_count == analysis_result_alloc_count) {
		analysis_result_alloc_count *= 2;
		retval->analysis_result_array = realloc(retval->analysis_result_array, analysis_result_alloc_count * sizeof(analysis_result));
	}/* if */	

}/* void parse_analysis_result_structure(psearch_hit retval, char* beginptr, FILE* finput, pdelegate_list dlist) */


/* Parses parameter_array structure */
void parse_parameter_structure(psearch_hit retval, char* beginptr, FILE* finput)
{
	char *tmpptr;
	
	if (!(retval->parameter_array)) {
		retval->parameter_array = malloc(sizeof(parameter));
		retval->parameter_count = 0;	
	}/* if */

	/* Filtering attributes */
	retval->parameter_array[retval->parameter_count].name = get_xml_attribute_value(beginptr, PARAMETER_ATTRIB_NAME);

	tmpptr = get_xml_attribute_value(beginptr, PARAMETER_ATTRIB_VALUE);
	retval->parameter_array[retval->parameter_count].value = strtod(tmpptr, NULL);
	free(tmpptr);

	retval->parameter_array[retval->parameter_count].type = get_xml_attribute_value(beginptr, PARAMETER_ATTRIB_TYPE);
				
	/* Updating counters and making more room for future structures */
	retval->parameter_count += 1;
	if (retval->parameter_count == parameter_alloc_count) {
		parameter_alloc_count *= 2;
		retval->parameter_array = realloc(retval->parameter_array, parameter_alloc_count * sizeof(parameter));
	}/* if */	

}/* void parse_parameter_structure(psearch_hit retval, char* beginptr, FILE* finput) */
