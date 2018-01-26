#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "pepXMLDefs.h"
#include "pepXMLReader.h"
#include "pepXMLReaderDefs.h"
#include "pepXMLDelegates.h"
#include "pepXMLParser.h"

/* Main interface function */
pmsms_pipeline_analysis read_pepxml_file(char* file_name, pipeline_analysis_flags paflags, run_summary_flags rsflags, pdelegate_list dlist)
{
	pmsms_pipeline_analysis retval = (pmsms_pipeline_analysis) malloc(sizeof(msms_pipeline_analysis));
	FILE* pepxml_filehandle;

	/* Logging file and opening it */
	retval->file_name = file_name;
	pepxml_filehandle = fopen(file_name, "rb");
	if (!pepxml_filehandle)
		return NULL;

	parse_msms_pipeline_analysis(retval, pepxml_filehandle, paflags, rsflags, dlist);

	fclose(pepxml_filehandle);
	return retval;

}/* pmsms_pipeline_analysis read_pepxml_file(char* file_name, pipeline_analysis_flags paflags, run_summary_flags rsflags, pdelegate_list dlist) */


/* Gets the analysis summary element of the pipeline summary */
panalysis_summary get_analysis_summary(pmsms_pipeline_analysis analysis, int* array_length, pdelegate_list dlist)
{
	FILE* input_file;

	/* Setting standard value */
	*array_length = 0;

	/* Fishing for the offset and the values */
	if (analysis->analysis_summary_count == 0 && analysis->analysis_summary_offset >= 0)
	{
		/* Opening and positioning the input file */
		input_file = fopen(analysis->file_name, "rb");
		if (!input_file)
			return NULL;
		fseek(input_file, analysis->analysis_summary_offset, SEEK_SET);

		/* Parse */
		parse_analysis_summary_structure_from_file(analysis, input_file, dlist);

		/* close */
		fclose(input_file);
	}/* if */
	
	*array_length = analysis->analysis_summary_count;
	return analysis->analysis_summary_array;

}/* panalysis_summary get_analysis_summary(pmsms_pipeline_analysis analysys, int* array_length) */


/* Gets the dataset derivation element of the pipeline summary */
pdataset_derivation get_dataset_derivation(pmsms_pipeline_analysis analysis)
{
	FILE* input_file;

	/* Fishing for the offset and the values */
	if (analysis->dataset_derivation_struct == NULL && analysis->dataset_derivation_offset >= 0)
	{
		/* Opening and positioning the input file */
		input_file = fopen(analysis->file_name, "rb");
		if (!input_file)
			return NULL;
		fseek(input_file, analysis->dataset_derivation_offset, SEEK_SET);

		/* Parse */
		parse_dataset_derivation_structure_from_file(analysis, input_file);

		/* close */
		fclose(input_file);
	}/* if */
	
	return analysis->dataset_derivation_struct;

}/* pdataset_derivation get_dataset_derivation(pmsms_pipeline_analysis analysis) */


/* Gets the sample enzyme element from a specific run summary TO BE IMPLEMENTED */
psample_enzyme run_summary_sample_enzyme(pmsms_run_summary run_summary)
{
	return NULL;

}/* psample_enzyme run_summary_sample_enzyme(pmsms_run_summary run_summary) */


/* Gets the search_summary element array from a specific run summary TO BE IMPLEMENTED */
psearch_summary run_summary_search_summary(pmsms_run_summary run_summary, int* array_length)
{
	*array_length = 0;
	return NULL;
}/* psearch_summary run_summary_search_summary(pmsms_run_summary run_summary, int* array_length) */


/* Gets the analysis time stamp element array from a specific run summary */
panalysis_timestamp run_summary_analysis_timestamp(pmsms_run_summary run_summary, int* array_length)
{
	*array_length = 0;
	return NULL;
}/* panalysis_timestamp run_summary_analysis_timestamp(pmsms_run_summary run_summary, int* array_length) */
