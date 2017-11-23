#ifndef __PEPXML_DEFS_H__
#define __PEPXML_DEFS_H__

/* Language definition standards */
#define XML_TAG_CTAG "/>" 
#define XML_TAG_ETAG ">" 
#define XML_TAG_OTAG "<" 

/* Global tag definition */
#define MSMS_PIPELINE_ANALYSIS_OTAG "<msms_pipeline_analysis"
#define MSMS_PIPELINE_ANALYSIS_CTAG "</msms_pipeline_analysis"
#define MSMS_PIPELINE_ANALYSIS_ATTRIB_NAME "name"
#define MSMS_PIPELINE_ANALYSIS_ATTRIB_DATE "date"
#define MSMS_PIPELINE_ANALYSIS_ATTRIB_SUMMARY_XML "summary_xml"

/* Analysis summary parts */
#define ANALYSIS_SUMMARY_OTAG "<analysis_summary"
#define ANALYSIS_SUMMARY_CTAG "</analysis_summary"
#define ANALYSIS_SUMMARY_ATTRIB_TIME "time"
#define ANALYSIS_SUMMARY_ATTRIB_ANALYSIS "analysis"
#define ANALYSIS_SUMMARY_ATTRIB_VERSION "version"

/* Dataset derivation parts */
#define DATASET_DERIVATION_OTAG "<dataset_derivation"
#define DATASET_DERIVATION_CTAG "</dataset_derivation"
#define DATASET_DERIVATION_ATTRIB_GENERATION_NO "generation_no"

/* Data filter parts */
#define DATA_FILTER_OTAG "<data_filter"
#define DATA_FILTER_CTAG "/"
#define DATA_FILTER_ATTRIB_NUMBER "number"
#define DATA_FILTER_ATTRIB_PARENT_FILE "parent_file"
#define DATA_FILTER_ATTRIB_WINDOW_PARENT "window_parent"
#define DATA_FILTER_ATTRIB_DESCRIPTION "description"

/* Demarkation of MSMS run summary */
#define MSMS_RUN_SUMMARY_OTAG "<msms_run_summary"
#define MSMS_RUN_SUMMARY_CTAG "</msms_run_summary"
#define MSMS_RUN_SUMMARY_ATTRIB_BASE_NAME "base_name"
#define MSMS_RUN_SUMMARY_ATTRIB_RAW_DATA_TYPE "raw_data_type"
#define MSMS_RUN_SUMMARY_ATTRIB_RAW_DATA "raw_data"
#define MSMS_RUN_SUMMARY_ATTRIB_MSMANUFACTURER "msManufacturer"
#define MSMS_RUN_SUMMARY_ATTRIB_MSMODEL "msModel"
#define MSMS_RUN_SUMMARY_ATTRIB_MSIONIZATION "msIonization"
#define MSMS_RUN_SUMMARY_ATTRIB_MSMASSANALYZER "msMassAnalyzer"
#define MSMS_RUN_SUMMARY_ATTRIB_MSDETECTOR "msDetector"

/* Demarcation of sample enzyme and its properties */
#define SAMPLE_ENZYME_OTAG "<sample_enzyme"
#define SAMPLE_ENZYME_CTAG "</sample_enzyme"
#define SAMPLE_ENZYME_ATTRIB_NAME "name"
#define SAMPLE_ENZYME_ATTRIB_DESCRIPTION "description"
#define SAMPLE_ENZYME_ATTRIB_FIDELITY "fidelity"
#define SAMPLE_ENZYME_ATTRIB_INDEPENDENT "independent"

/* Demarcation of ssample enzymne specifity and its properties */
#define SPECIFITY_OTAG "<specifity"
#define SPECIFITY_CTAG "/"
#define SPECIFITY_ATTRIB_SENSE "sense"
#define SPECIFITY_ATTRIB_MIN_SPACING "min_spacing"
#define SPECIFITY_ATTRIB_CUT "cut"
#define SPECIFITY_ATTRIB_NO_CUT "no_cut"

/* Demarcation of search summary and its properties */
#define SEARCH_SUMMARY_OTAG "<search_summary"
#define SEARCH_SUMMARY_CTAG "</search_summary"

/* Demarcation of analysis timestamp and its properties */
#define ANALYSIS_TIMESTAMP_OTAG "<analysis_timestamp"
#define ANALYSIS_TIMESTAMP_CTAG "</analysis_timestamp"

/* Demarkation of spectrum query and their properties */
#define SPECTRUM_QUERY_OTAG "<spectrum_query"
#define SPECTRUM_QUERY_CTAG "</spectrum_query"
#define SPECTRUM_QUERY_ATTRIB_SPECTRUM "spectrum"
#define SPECTRUM_QUERY_ATTRIB_START_SCAN "start_scan"
#define SPECTRUM_QUERY_ATTRIB_END_SCAN "end_scan"
#define SPECTRUM_QUERY_ATTRIB_RETENTION_TIME_SEC "retention_time_sec"
#define SPECTRUM_QUERY_ATTRIB_ACTIVATION_METHOD "activation_method"
#define SPECTRUM_QUERY_ATTRIB_PRECURSOR_NEUTRAL_MASS "precursor_neutral_mass"
#define SPECTRUM_QUERY_ATTRIB_ASSUMED_CHARGE "assumed_charge"
#define SPECTRUM_QUERY_ATTRIB_SEARCH_SPECIFICATION "search_specification"
#define SPECTRUM_QUERY_ATTRIB_INDEX "index"

/* Demarkation of search result tags and elements */
#define SEARCH_RESULT_OTAG "<search_result"
#define SEARCH_RESULT_CTAG "</search_result"
#define SEARCH_RESULT_ATTRIB_SEARCH_ID "search_id"

/* Demarkation of search hit tags and elements */
#define SEARCH_HIT_OTAG "<search_hit"
#define SEARCH_HIT_CTAG "</search_hit"
#define SEARCH_HIT_ATTRIB_HIT_RANK "hit_rank"
#define SEARCH_HIT_ATTRIB_PEPTIDE "peptide"
#define SEARCH_HIT_ATTRIB_PEPTIDE_PREV_AA "peptide_prev_aa"
#define SEARCH_HIT_ATTRIB_PEPTIDE_NEXT_AA "peptide_next_aa"
#define SEARCH_HIT_ATTRIB_PROTEIN "protein"
#define SEARCH_HIT_ATTRIB_NUM_TOT_PROTEINS "num_tot_proteins"
#define SEARCH_HIT_ATTRIB_NUM_MATCHED_IONS "num_matched_ions"
#define SEARCH_HIT_ATTRIB_TOT_NUM_IONS "tot_num_ions"
#define SEARCH_HIT_ATTRIB_CALC_NEUTRAL_PEP_MASS "calc_neutral_pep_mass"
#define SEARCH_HIT_ATTRIB_MASSDIFF "massdiff"
#define SEARCH_HIT_ATTRIB_NUM_TOL_TERM "num_tol_term"
#define SEARCH_HIT_ATTRIB_NUM_MISSED_CLEAVAGES "num_missed_cleavages"
#define SEARCH_HIT_ATTRIB_IS_REJECTED "is_rejected"
#define SEARCH_HIT_ATTRIB_PROTEIN_DESCR "protein_descr"
#define SEARCH_HIT_ATTRIB_CALC_PI "calc_pI"
#define SEARCH_HIT_ATTRIB_PROTEIN_MW "protein_mw"

/* Demarkation of alternative protein tag */
#define ALTERNATIVE_PROTEIN_OTAG "<alternative_protein"
#define ALTERNATIVE_PROTEIN_CTAG "/"
#define ALTERNATIVE_PROTEIN_ATTRIB_PROTEIN "protein"
#define ALTERNATIVE_PROTEIN_ATTRIB_PROTEIN_DESCR "protein_descr"
#define ALTERNATIVE_PROTEIN_ATTRIB_NUM_TOL_TERM "num_tol_term"
#define ALTERNATIVE_PROTEIN_ATTRIB_PROTEIN_MW "protein_mw"
#define ALTERNATIVE_PROTEIN_ATTRIB_PEPTIDE_PREV_AA "peptide_prev_aa"
#define ALTERNATIVE_PROTEIN_ATTRIB_PEPTIDE_NEXT_AA "peptide_next_aa"

/* Demarkation of Modification Info Tag */
#define MODIFICATION_INFO_OTAG "<modification_info"
#define MODIFICATION_INFO_CTAG "</modification_info"
#define MODIFICATION_INFO_ATTRIB_MOD_NTERM_MASS "mod_nterm_mass"
#define MODIFICATION_INFO_ATTRIB_MOD_CTERM_MASS "mod_cterm_mass"
#define MODIFICATION_INFO_ATTRIB_MODIFIED_PEPTIDE "modified_peptide"

/* Demarkation of mad_aminoacid_mass Tag */
#define MOD_AMINOACID_MASS_OTAG "<mod_aminoacid_mass"
#define MOD_AMINOACID_MASS_ATTRIB_POSITION "position"
#define MOD_AMINOACID_MASS_ATTRIB_MASS "mass"

/* Demarkation of the search score */
#define SEARCH_SCORE_OTAG "<search_score"
#define SEARCH_SCORE_ATTRIB_NAME "name" 
#define SEARCH_SCORE_ATTRIB_VALUE "value" 
#define SEARCH_SCORE_ATTRIB_TYPE "type" 

/* Demarkation of the analysis result */
#define ANALYSIS_RESULT_OTAG "<analysis_result"
#define ANALYSIS_RESULT_CTAG "</analysis_result"
#define ANALYSIS_RESULT_ATTRIB_ANALYSIS "analysis"
#define ANALYSIS_RESULT_ATTRIB_ID "id"

/* Demarkation of the search_hit parameters */
#define PARAMETER_OTAG "<parameter"
#define PARAMETER_ATTRIB_NAME "name" 
#define PARAMETER_ATTRIB_VALUE "value" 
#define PARAMETER_ATTRIB_TYPE "type" 

#endif

