#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "pepXMLDefs.h"
#include "pepXMLWriteFunctions.h"
#include "IOFunctions.h"

static char read_buffer[WRITE_BUFF_SIZE];

/* Writes the entire file. Copies all residual info from the file recorded in pepxml_file */
void write_msms_pipeline_analysis(pmsms_pipeline_analysis pepxml_file, char* dest_file_name, pdelegate_list dlist)
{
	FILE *input, *output;			
	int read_chars;
	
	/* Opening write file handle */
	output = fopen(dest_file_name, "wb");
	if (!output)
		return;

	/* First copying all data up till the pipeline analysis opening tag */		
	input = fopen(pepxml_file->file_name, "rb");
	if (!input)
		return;	
	read_chars = fread(read_buffer, sizeof(char), pepxml_file->start_offset+1, input);
	//original was causing problem in XML tag: "<<msms_pipeline_analysis date"
	//read_buffer[read_chars] = '\0';
	read_buffer[read_chars-1] = '\0';
	fclose(input);
	
	/* Actual copying */
	fprintf(output, "%s", read_buffer);
	
	/* Printing header + attributes */
	fprintf(output, "%s", MSMS_PIPELINE_ANALYSIS_OTAG);
	if (pepxml_file->name)
		fprintf(output, " name=\"%s\"", pepxml_file->name);
		
	fprintf(output, " date=\"%s\" summary_xml=\"%s\"", pepxml_file->date, pepxml_file->summary_xml);
	fprintf(output, " xmlns=\"http://regis-web.systemsbiology.net/pepXML\"");
	fprintf(output, " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"");
	fprintf(output, " xsi:schemaLocation=\"http://sashimi.sourceforge.net/schema_revision/pepXML/pepXML_v117.xsd\">\n");
	
	/* Printing individual elements */
	print_analysis_summary(pepxml_file, output, dlist);
	print_dataset_derivation(pepxml_file, output);
	print_run_summary(pepxml_file, output, dlist);

	/* Printing the closing tages */
	fprintf(output, "%s>\n", MSMS_PIPELINE_ANALYSIS_CTAG);
			
	/* Closing the output */
	fclose(output);

}/* void write_msms_pipeline_analysis(pmsms_pipeline_analysis pepxml_file, char* dest_file_name, pdelegate_list dlist) */


/* Writes all analysis_summary structures */
void print_analysis_summary(pmsms_pipeline_analysis pepxml_file, FILE* output, pdelegate_list dlist)
{
	FILE* input;
	int i, read_chars;
		
	/* Analysis summary. If not loaded, copy, otherwise, print. */	
	if (pepxml_file->analysis_summary_offset >= 0) {
		/* Loaded */		
		if (pepxml_file->analysis_summary_count > 0) {
			for (i=0; i<pepxml_file->analysis_summary_count; i++) {
				print_analysis_summary_element(pepxml_file->analysis_summary_array[i], output, dlist);
			}/* for */	
		}/* if */
		else {
			input = fopen(pepxml_file->file_name, "rb");
			fseek(input, pepxml_file->analysis_summary_offset, SEEK_SET);
			read_chars = fread(read_buffer, sizeof(char), pepxml_file->analysis_summary_length+1, input);
			read_buffer[read_chars] = '\0';
			fclose(input);
			fprintf(output, "%s", read_buffer);			
		}/* else */
	}/* if */

}/* void print_analysis_summary(pmsms_pipeline_analysis pepxml_file, FILE* output) */


/* Writes a single analysis_summary element */
void print_analysis_summary_element(analysis_summary analysis, FILE* output, pdelegate_list dlist)
{
	pdelegate_type delegate_t;
	char* contents;

	/* Printing header and attributes */
	fprintf(output, "%s %s=\"%s\" %s=\"%s\"", ANALYSIS_SUMMARY_OTAG, 
		ANALYSIS_SUMMARY_ATTRIB_TIME, analysis.time,
		ANALYSIS_SUMMARY_ATTRIB_ANALYSIS, analysis.analysis);	
	if (analysis.version)
		fprintf(output, " %s=\"%s\"", ANALYSIS_SUMMARY_ATTRIB_VERSION, analysis.version);
	fprintf(output, ">\n");

	/* Fetching the contents of the analysis summary */
	delegate_t = get_delegate(analysis.analysis, "analysis_summary", dlist);	
	if (delegate_t != NULL) {
		contents = delegate_t->write_function(analysis.handle);	
		fprintf(output, "%s", contents);
		free(contents);
	}// if
	else
		fprintf(output, "%s", (char*)(analysis.handle));
	
	/* Printing the endtag */
	fprintf(output, "%s>\n", ANALYSIS_SUMMARY_CTAG);	

}/* void print_analysis_summary_element(analysis_summary analysis, FILE* output) */


/* Writes the dataset derivation structures */
void print_dataset_derivation(pmsms_pipeline_analysis pepxml_file, FILE* output)
{
	FILE* input;
	int i, read_chars;
		
	/* Analysis summary. If not loaded, copy, otherwise, print. */	
	if (pepxml_file->dataset_derivation_offset >= 0) {
		/* Loaded */		
		if (pepxml_file->dataset_derivation_struct != NULL) {
			fprintf(output, "%s %s=\"%i\">\n", DATASET_DERIVATION_OTAG, DATASET_DERIVATION_ATTRIB_GENERATION_NO, pepxml_file->dataset_derivation_struct->generation_no);	
						
			for (i=0; i<pepxml_file->dataset_derivation_struct->data_filter_count; i++) {
				/* Writing required attributes */
				fprintf(output, "%s %s=\"%i\" %s=\"%s\" %s=\"%s\"", DATA_FILTER_OTAG, 
					DATA_FILTER_ATTRIB_NUMBER, pepxml_file->dataset_derivation_struct->data_filter_array[i].number,
					DATA_FILTER_ATTRIB_PARENT_FILE, pepxml_file->dataset_derivation_struct->data_filter_array[i].parent_file,
					DATA_FILTER_ATTRIB_DESCRIPTION, pepxml_file->dataset_derivation_struct->data_filter_array[i].description);

				/* Writing optional attributes */
				if (pepxml_file->dataset_derivation_struct->data_filter_array[i].windows_parent)
					fprintf(output, " %s=\"%s\"", DATA_FILTER_ATTRIB_WINDOW_PARENT, pepxml_file->dataset_derivation_struct->data_filter_array[i].windows_parent);
				fprintf(output, "%s>\n", DATA_FILTER_CTAG);
			}/* for */		
		}/* if */
		else {
			input = fopen(pepxml_file->file_name, "rb");
			fseek(input, pepxml_file->dataset_derivation_offset, SEEK_SET);
			read_chars = fread(read_buffer, sizeof(char), pepxml_file->dataset_derivation_length + 1, input);
			read_buffer[read_chars] = '\0';
			fclose(input);						
			fprintf(output, "%s", read_buffer);			
		}/* else */
	}/* if */

}/* void print_dataset_derivation(pmsms_pipeline_analysis pepxml_file, FILE* output) */


/* Writes the run summary structures */
void print_run_summary(pmsms_pipeline_analysis pepxml_file, FILE* output, pdelegate_list dlist)
{
	int i;

	for (i=0; i<pepxml_file->run_summary_count; i++) {
		print_run_summary_element(pepxml_file->run_summary_array[i], output, pepxml_file->file_name, dlist);
	}/* for */		

}/* void print_run_summary(pmsms_pipeline_analysis pepxml_file, FILE* output, pdelegate_list dlist) */


/* Writes a single run_sammary element */
void print_run_summary_element(msms_run_summary summary, FILE* output, char *input, pdelegate_list dlist)
{
	/* Writing required attributes */
	fprintf(output, "\t%s %s=\"%s\" %s=\"%s\" %s=\"%s\"", MSMS_RUN_SUMMARY_OTAG, 
		MSMS_RUN_SUMMARY_ATTRIB_BASE_NAME, summary.base_name,
		MSMS_RUN_SUMMARY_ATTRIB_RAW_DATA_TYPE, summary.raw_data_type,
		MSMS_RUN_SUMMARY_ATTRIB_RAW_DATA, summary.raw_data);	

	/* writing optional attributes */
	if (summary.msManufacturer)
		fprintf(output, " %s=\"%s\"", MSMS_RUN_SUMMARY_ATTRIB_MSMANUFACTURER, summary.msManufacturer);
	if (summary.msModel)
		fprintf(output, " %s=\"%s\"", MSMS_RUN_SUMMARY_ATTRIB_MSMODEL, summary.msModel);
	if (summary.msIonization)
		fprintf(output, " %s=\"%s\"", MSMS_RUN_SUMMARY_ATTRIB_MSIONIZATION, summary.msIonization);
	if (summary.msMassAnalyzer)
		fprintf(output, " %s=\"%s\"", MSMS_RUN_SUMMARY_ATTRIB_MSMASSANALYZER, summary.msMassAnalyzer);
	if (summary.msDetector)
		fprintf(output, " %s=\"%s\"", MSMS_RUN_SUMMARY_ATTRIB_MSDETECTOR, summary.msDetector);
	
	/* Closing the opening tag */
	fprintf(output, ">\n");

	/* Printing the elements */
	print_sample_enzyme(summary, output, input);	
	print_search_summary(summary, output, input);	
	print_analysis_timestamp(summary, output, input);	
	print_spectrum_query(summary, output, dlist);	

	/* Printing the closing tag */
	fprintf(output, "\t%s>\n", MSMS_RUN_SUMMARY_CTAG);

}/* void print_run_summary_element(msms_run_summary summary, FILE* output, pdelegate_list dlist) */


/* Writes the sample enzyme structure */
void print_sample_enzyme(msms_run_summary rsummary, FILE* output, char *input)
{
	FILE* finput;
	int i, read_chars;

	if (rsummary.sample_enzyme_offset >= 0) {
		/* Loaded */		
		if (rsummary.sample_enzyme_element != NULL) {
			/* Writing required attributes */
			fprintf(output, "\t\t%s %s=\"%s\"", SAMPLE_ENZYME_OTAG, SAMPLE_ENZYME_ATTRIB_NAME, rsummary.sample_enzyme_element->name);
			
			/* Writing optional attributes */
			if (rsummary.sample_enzyme_element->description)
				fprintf(output, " %s=\"%s\"", SAMPLE_ENZYME_ATTRIB_DESCRIPTION, rsummary.sample_enzyme_element->description);
			if (rsummary.sample_enzyme_element->fidelity)
				fprintf(output, " %s=\"%s\"", SAMPLE_ENZYME_ATTRIB_FIDELITY, rsummary.sample_enzyme_element->fidelity);
			if (rsummary.sample_enzyme_element->independent)
				fprintf(output, " %s=\"%s\"", SAMPLE_ENZYME_ATTRIB_INDEPENDENT, rsummary.sample_enzyme_element->independent);
	
			/* Writing specifity element list */
			for (i=0; i<rsummary.sample_enzyme_element->specifity_count; i++) {
				/* Writing header and required attributes */				
				fprintf(output, "\t\t\t%s %s=\"%s\" %s=\"%s\" %s=\"%s\"", SPECIFITY_OTAG, 
					SPECIFITY_ATTRIB_SENSE, rsummary.sample_enzyme_element->specifity_array[i].sense,
					SPECIFITY_ATTRIB_CUT, rsummary.sample_enzyme_element->specifity_array[i].cut,
					SPECIFITY_ATTRIB_NO_CUT, rsummary.sample_enzyme_element->specifity_array[i].no_cut);

				/* Writing optional attributes */
				if (rsummary.sample_enzyme_element->specifity_array[i].min_spacing)
					fprintf(output, " %s=\"%i\"", SPECIFITY_ATTRIB_MIN_SPACING, rsummary.sample_enzyme_element->specifity_array[i].min_spacing);

				/* Printing end of tag */
				fprintf(output, ">\n");
			}/* for */	
			
			/* Printing "closing tag */
			fprintf(output, "\t\t%s>\n", SAMPLE_ENZYME_CTAG);
		}/* if */
		else {
			finput = fopen(input, "rb");
			fseek(finput, rsummary.sample_enzyme_offset, SEEK_SET);
			read_chars = fread(read_buffer, sizeof(char), rsummary.sample_enzyme_length+1, finput);
			read_buffer[read_chars] = '\0';
			fclose(finput);				
			fprintf(output, "%s", read_buffer);			
		}/* else */
	}/* if */

}/* void print_sample_enzyme(msms_run_summary rsummary, FILE* output) */


/* Writes the search summary structure */
void print_search_summary(msms_run_summary rsummary, FILE* output, char *input)
{
	FILE* finput;
	int i;

	if (rsummary.search_summary_offset >= 0) {
		/* Loaded */		
		if (rsummary.search_summary_array != NULL) {
			for (i=0; i<rsummary.search_summary_count; i++) {
				/* Print opening tag and content */
				fprintf(output, "\t\t%s %s", SEARCH_SUMMARY_OTAG, rsummary.search_summary_array[i].contents);
				/* Print closing tag */
				fprintf(output, "\t\t%s>\n", SEARCH_SUMMARY_CTAG);
			}/* for */						
		}/* if */
		else {
			finput = fopen(input, "rb");
			fseek(finput, rsummary.search_summary_offset, SEEK_SET);
			copy_input_output(read_buffer, WRITE_BUFF_SIZE, finput, output, rsummary.search_summary_length);
			fclose(finput);
		}/* else */
	}/* if */

}/* void print_search_summary(msms_run_summary rsummary, FILE* output) */


/* Writes the analysis timestamp structure */
void print_analysis_timestamp(msms_run_summary rsummary, FILE* output, char *input)
{
	FILE* finput;
	int i, read_chars;

	if (rsummary.analysis_timestamp_offset >= 0) {
		/* Loaded */		
		if (rsummary.analysis_timestamp_array != NULL) {
			for (i=0; i<rsummary.analysis_timestamp_count; i++) {
				/* Print opening tag and content */
				fprintf(output, "%s %s", ANALYSIS_TIMESTAMP_OTAG, rsummary.analysis_timestamp_array[i].contents);
				/* Print closing tag */
				fprintf(output, "%s>\n", ANALYSIS_TIMESTAMP_CTAG);
			}/* for */						
		}/* if */
		else {
			finput = fopen(input, "rb");
			fseek(finput, rsummary.analysis_timestamp_offset, SEEK_SET);
			read_chars = fread(read_buffer, sizeof(char), rsummary.analysis_timestamp_length + 1, finput);
			read_buffer[read_chars] = '\0';
			fclose(finput);
			fprintf(output, "%s", read_buffer);			
		}/* else */
	}/* if */

}/* void print_search_summary(msms_run_summary rsummary, FILE* output) */


/* Spectrum query printer, prints spectrum query to a pepxml standard format */
void print_spectrum_query(msms_run_summary rsummary, FILE* output, pdelegate_list dlist)
{
	int i;

	for (i=0; i<rsummary.spectrum_query_count; i++)
		print_spectrum_query_element(rsummary.spectrum_query_array[i], output, dlist);
	

}/* void print_spectrum_query(msms_run_summary rsummary, FILE* output, pdelegate_list dlist) */


/* Writes a single spectrum query element */
void print_spectrum_query_element(spectrum_query squery, FILE* output, pdelegate_list dlist)
{
	/* Printing opening tag and required attributes */
	fprintf(output, "\t\t%s %s=\"%s\" %s=\"%i\" %s=\"%i\" %s=\"%f\" %s=\"%i\" %s=\"%i\"", SPECTRUM_QUERY_OTAG, 
		SPECTRUM_QUERY_ATTRIB_SPECTRUM, squery.spectrum, 
		SPECTRUM_QUERY_ATTRIB_START_SCAN, squery.start_scan,
		SPECTRUM_QUERY_ATTRIB_END_SCAN, squery.end_scan,
		SPECTRUM_QUERY_ATTRIB_PRECURSOR_NEUTRAL_MASS, squery.precursor_neutral_mass,
		SPECTRUM_QUERY_ATTRIB_ASSUMED_CHARGE, squery.assumed_charge,
		SPECTRUM_QUERY_ATTRIB_INDEX, squery.index);		

	/* Printing optional attributes */
	if (squery.retention_time_sec >= 0)
		fprintf(output, " %s=\"%f\"", SPECTRUM_QUERY_ATTRIB_RETENTION_TIME_SEC, squery.retention_time_sec);
	if (squery.activation_method)
		fprintf(output, " %s=\"%s\"", SPECTRUM_QUERY_ATTRIB_ACTIVATION_METHOD, squery.activation_method);
	if (squery.search_specification)
		fprintf(output, " %s=\"%s\"", SPECTRUM_QUERY_ATTRIB_SEARCH_SPECIFICATION, squery.search_specification);		

	/* Closing the opening tag */
	fprintf(output, ">\n");

	/* Print elements */
	print_search_result(squery, output, dlist);

	/* Print closing tag */					
	fprintf(output, "\t\t%s>\n", SPECTRUM_QUERY_CTAG);

}/* void print_spectrum_query_element(spectrum_query squery, FILE* output, pdelegate_list dlist) */


/* Writes the search result structures */
void print_search_result(spectrum_query squery, FILE* output, pdelegate_list dlist)
{
	int i;	

	for (i=0; i<squery.search_result_count; i++) {
		/* Print opening tag */
		fprintf(output, "\t\t\t%s", SEARCH_RESULT_OTAG);

		/* Print optional attributes */
		if (squery.search_result_array[i].search_id >= 0)
			fprintf(output, " %s=\"%i\"", SEARCH_RESULT_ATTRIB_SEARCH_ID, squery.search_result_array[i].search_id);
	
		/* Closing header tag */
		fprintf(output, ">\n");

		/*Printing elements */
		print_search_hit(squery.search_result_array[i], output, dlist);

		/* Print closing tag */
		fprintf(output, "\t\t\t%s>\n", SEARCH_RESULT_CTAG);
	}/* for */						
	
}/* void print_search_result(spectrum_query squery, FILE* output, pdelegate_list dlist) */


/* Writes the search result structures */
void print_search_hit(search_result sresult, FILE* output, pdelegate_list dlist)
{
	int i;
		
	for (i=0; i<sresult.search_hit_count; i++) {
		print_search_hit_element(sresult.search_hit_array[i], output, dlist);
	}/* for */

}/* void print_search_hit(search_result sresult, FILE* output, pdelegate_list dlist) */


/* Writes a single search hit element */
void print_search_hit_element(search_hit s_hit, FILE* output, pdelegate_list dlist)
{
	/* Printing opening tag and required attributes */
	fprintf(output, "\t\t\t\t%s %s=\"%i\" %s=\"%s\"",SEARCH_HIT_OTAG,
		SEARCH_HIT_ATTRIB_HIT_RANK,s_hit.hit_rank,
		SEARCH_HIT_ATTRIB_PEPTIDE, s_hit.peptide);
		if (s_hit.peptide_prev_aa)
			fprintf(output, " %s=\"%s\"", SEARCH_HIT_ATTRIB_PEPTIDE_PREV_AA, s_hit.peptide_prev_aa);
		if (s_hit.peptide_next_aa)
			fprintf(output, " %s=\"%s\"", SEARCH_HIT_ATTRIB_PEPTIDE_NEXT_AA, s_hit.peptide_next_aa);
		fprintf(output," %s=\"%s\"",SEARCH_HIT_ATTRIB_PROTEIN, s_hit.protein);
		if (s_hit.protein_descr)
			fprintf(output, " %s=\"%s\"", SEARCH_HIT_ATTRIB_PROTEIN_DESCR, s_hit.protein_descr);
		fprintf(output," %s=\"%i\"",SEARCH_HIT_ATTRIB_NUM_TOT_PROTEINS, s_hit.num_tot_proteins);
		if (s_hit.num_matched_ions >= 0)
			fprintf(output, " %s=\"%i\"", SEARCH_HIT_ATTRIB_NUM_MATCHED_IONS, s_hit.num_matched_ions);
		if (s_hit.tot_num_ions >= 0)
			fprintf(output, " %s=\"%i\"", SEARCH_HIT_ATTRIB_TOT_NUM_IONS, s_hit.tot_num_ions);
		fprintf(output," %s=\"%f\"",SEARCH_HIT_ATTRIB_CALC_NEUTRAL_PEP_MASS, s_hit.calc_neutral_pep_mass);
		fprintf(output," %s=\"%f\"",SEARCH_HIT_ATTRIB_MASSDIFF, s_hit.massdiff);	
		if (s_hit.num_tol_term >= 0)
			fprintf(output, " %s=\"%i\"", SEARCH_HIT_ATTRIB_NUM_TOL_TERM, s_hit.num_tol_term);	
		if (s_hit.num_missed_cleavages >= 0)
			fprintf(output, " %s=\"%i\"", SEARCH_HIT_ATTRIB_NUM_MISSED_CLEAVAGES, s_hit.num_missed_cleavages);
		if (s_hit.is_rejected == 1)
			fprintf(output, " %s=\"1\"", SEARCH_HIT_ATTRIB_IS_REJECTED);
		if (s_hit.calc_pI >= 0)
			fprintf(output, " %s=\"%f\"", SEARCH_HIT_ATTRIB_CALC_PI, s_hit.calc_pI);
		if (s_hit.protein_mw >= 0)
			fprintf(output, " %s=\"%f\"", SEARCH_HIT_ATTRIB_PROTEIN_MW, s_hit.protein_mw);

	/* Closing the opening tag */
	fprintf(output, ">\n");

	/* Print elements */	
	print_alternative_protein(s_hit, output);
	print_modification_info(s_hit, output);
	print_search_score(s_hit, output);
	print_analysis_result(s_hit, output, dlist);
	print_parameter(s_hit, output);
	
	/* Print closing tag */					
	fprintf(output, "\t\t\t\t%s>\n", SEARCH_HIT_CTAG);

}/* void print_search_hit_element(search_hit s_hit, FILE* output, pdelegate_list dlist) */


/* Writes the alternative protein structures */
void print_alternative_protein(search_hit s_hit, FILE* output)
{
	int i;
		
	for (i=0; i<s_hit.alternative_protein_count; i++) {
		/* Printing opening tag and required attributes */
		fprintf(output, "\t\t\t\t\t%s %s=\"%s\"", ALTERNATIVE_PROTEIN_OTAG, 
			ALTERNATIVE_PROTEIN_ATTRIB_PROTEIN, s_hit.alternative_protein_array[i].protein);		

		/* Printing optional attributes */
		if (s_hit.alternative_protein_array[i].protein_descr)
			fprintf(output, " %s=\"%s\"", ALTERNATIVE_PROTEIN_ATTRIB_PROTEIN_DESCR, s_hit.alternative_protein_array[i].protein_descr);
		if (s_hit.alternative_protein_array[i].num_tol_term >= 0)
			fprintf(output, " %s=\"%i\"", ALTERNATIVE_PROTEIN_ATTRIB_NUM_TOL_TERM, s_hit.alternative_protein_array[i].num_tol_term);
		if (s_hit.alternative_protein_array[i].protein_mw >= 0)
			fprintf(output, " %s=\"%f\"", ALTERNATIVE_PROTEIN_ATTRIB_PROTEIN_MW, s_hit.alternative_protein_array[i].protein_mw);	
		if (s_hit.alternative_protein_array[i].peptide_prev_aa)
			fprintf(output, " %s=\"%s\"", ALTERNATIVE_PROTEIN_ATTRIB_PEPTIDE_PREV_AA, s_hit.alternative_protein_array[i].peptide_prev_aa);
		if (s_hit.alternative_protein_array[i].peptide_next_aa)
			fprintf(output, " %s=\"%s\"", ALTERNATIVE_PROTEIN_ATTRIB_PEPTIDE_NEXT_AA, s_hit.alternative_protein_array[i].peptide_next_aa);	

		/* Close the tage */
		fprintf(output, "%s>\n", ALTERNATIVE_PROTEIN_CTAG);
	}/* for */	

}/* void print_alternative_protein(search_hit s_hit, FILE* output) */

/* Writes the modification info structures */
void print_modification_info(search_hit s_hit, FILE* output)
{				
	if (s_hit.modification_info_struct) {
		/* Printing opening tag and required attributes */
		fprintf(output, "\t\t\t\t\t%s", MODIFICATION_INFO_OTAG);		

		/* Printing optional attributes */
		if (s_hit.modification_info_struct->mod_nterm_mass >= 0)
			fprintf(output, " %s=\"%f\"", MODIFICATION_INFO_ATTRIB_MOD_NTERM_MASS, s_hit.modification_info_struct->mod_nterm_mass);
		
		if (s_hit.modification_info_struct->mod_cterm_mass >= 0)
			fprintf(output, " %s=\"%f\"", MODIFICATION_INFO_ATTRIB_MOD_CTERM_MASS, s_hit.modification_info_struct->mod_cterm_mass);
		
		if (s_hit.modification_info_struct->modified_peptide)
			fprintf(output, " %s=\"%s\"", MODIFICATION_INFO_ATTRIB_MODIFIED_PEPTIDE, s_hit.modification_info_struct->modified_peptide);
		
		
	    if (s_hit.modification_info_struct->mod_aminoacid_mass_count > 0) {
			/* Close the tag */
			fprintf(output, ">\n");
			
			/* Print elements */
			print_mod_aminoacid_mass(*(s_hit.modification_info_struct), output);
			
			/* Printing closing tag */
			fprintf(output, "\t\t\t\t\t%s>\n", MODIFICATION_INFO_CTAG);		
		}
		
		else {
			/* Close the tag */
			fprintf(output, "/>\n");
		}	
	}/* if */	

}/* void print_modification_info(search_hit s_hit, FILE* output) */


/* Writes the modification info structures */
void print_mod_aminoacid_mass(modification_info minfo, FILE* output)
{
	int i;

	for (i=0; i<minfo.mod_aminoacid_mass_count; i++) {
		/* Printing all at once */
		fprintf(output, "\t\t\t\t\t\t%s %s=\"%f\" %s=\"%i\"/>\n", MOD_AMINOACID_MASS_OTAG, 
			MOD_AMINOACID_MASS_ATTRIB_MASS, minfo.mod_aminoacid_mass_array[i].mass,
			MOD_AMINOACID_MASS_ATTRIB_POSITION, minfo.mod_aminoacid_mass_array[i].position);		
	}/* for */	

}/* void print_modification_info(modification_info minfo, FILE* output) */


/* Writes the search score structures */
void print_search_score(search_hit s_hit, FILE* output)
{
	int i;

	for (i=0; i<s_hit.search_score_count; i++) {
		/* Printing header and required attributes */
		fprintf(output, "\t\t\t\t\t%s %s=\"%s\" %s=\"%f\"", SEARCH_SCORE_OTAG, 
			SEARCH_SCORE_ATTRIB_NAME, s_hit.search_score_array[i].name,
			SEARCH_SCORE_ATTRIB_VALUE, s_hit.search_score_array[i].value);

		/* Printing optional attributes */
		if (s_hit.search_score_array[i].type)
			fprintf(output, " %s=\"%s\"", SEARCH_SCORE_ATTRIB_TYPE, s_hit.search_score_array[i].type);

		/* Printing end of tag */
		fprintf(output, "/>\n");
	}/* for */	

}/* void print_search_score(search_hit s_hit, FILE* output) */

/* Writes the analysis result structures */
void print_analysis_result(search_hit s_hit, FILE* output, pdelegate_list dlist)
{
	int i;
	pdelegate_type delegate_t;
	char* contents;

	for (i=0; i<s_hit.analysis_result_count; i++) {
		/* Printing header and required attributes */
		fprintf(output, "%s %s=\"%s\"", ANALYSIS_RESULT_OTAG,
			ANALYSIS_RESULT_ATTRIB_ANALYSIS, s_hit.analysis_result_array[i].analysis);	
		
		/* Printing optional attributes */
		if (s_hit.analysis_result_array[i].id >= 0)
			fprintf(output, " %s=\"%i\"", ANALYSIS_RESULT_ATTRIB_ID, s_hit.analysis_result_array[i].id);
		fprintf(output, ">\n");

		/* Fetching the contents of the analysis summary */
		delegate_t = get_delegate(s_hit.analysis_result_array[i].analysis, "analysis_result", dlist);
		if (delegate_t != NULL) {
			contents = delegate_t->write_function(s_hit.analysis_result_array[i].hook);	
			fprintf(output, "%s", contents);
			free(contents);
		}/* if */
		else
			fprintf(output, "%s", (char*)(s_hit.analysis_result_array[i].hook));
	
		/* Printing the endtag */
		fprintf(output, "%s>\n", ANALYSIS_RESULT_CTAG);	
	}/* for */

}/* void print_analysis_result(search_hit s_hit, FILE* output, pdelegate_list dlist) */

/* Writes the search score structures */
void print_parameter(search_hit s_hit, FILE* output)
{
	int i;

	for (i=0; i<s_hit.parameter_count; i++) {
		/* Printing header and required attributes */
		fprintf(output, "%s %s=\"%s\" %s=\"%f\"", PARAMETER_OTAG, 
			PARAMETER_ATTRIB_NAME, s_hit.parameter_array[i].name,
			PARAMETER_ATTRIB_VALUE, s_hit.parameter_array[i].value);		

		/* Printing optional attributes */
		if (s_hit.parameter_array[i].type)
			fprintf(output, " %s=\"%s\"", PARAMETER_ATTRIB_TYPE, s_hit.parameter_array[i].type);

		/* Printing end of tag */
		fprintf(output, "/>\n");
	}/* for */

}/* void print_parameter(search_hit s_hit, FILE* output) */

