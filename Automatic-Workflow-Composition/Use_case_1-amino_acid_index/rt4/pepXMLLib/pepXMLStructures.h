#ifndef __PEPXML_SPECTRUM_QUERY_H__
#define __PEPXML_SPECTRUM_QUERY_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* analysis_result element. Hook refers to a custom structure the user can hang on it */
typedef struct pepxml_analysis_result
{
	char* analysis;	/* Name of the analysis */
	int   id;		/* Unique identifier */
	void* hook;		/* Contents */

} analysis_result, *panalysis_result;


/* search_score element. */
typedef struct pepxml_name_value_type
{	
    char*  name;	/* Name of the search score */
    double value;	/* score value */
	char*  type;	/* type of score */
		
} search_score, *psearch_score, parameter, *pparameter;


/* Modification info element */
typedef struct pepxml_mod_aminoacid_mass
{	
    unsigned int position; /* Modified aminoacid position in peptide [ranging from 1 to peptide length] */
    double		 mass;     /* Modified mass of aminoacid */

} mod_aminoacid_mass, *pmod_aminoacid_mass;


/* modification info structure. */
typedef struct pepxml_modification_info
{
	/* Attributes */
	char*  modified_peptide;	/* Peptide sequence (with indicated modifications) */
	double mod_nterm_mass;		/* Mass of modified N terminus */
	double mod_cterm_mass;		/* Mass of modified C terminus */

	/* Elements */
	pmod_aminoacid_mass mod_aminoacid_mass_array;
	int                 mod_aminoacid_mass_count;   

} modification_info, *pmodification_info;


/* alternative_protein element. */
typedef struct pepxml_alternative_protein
{
	/* Attributes */
	char*        protein;					/* Required */
	char*        protein_descr;				/* Extracted from search database */ 
	unsigned int num_tol_term;				/* Number of peptide termini consistent with cleavage by sample enzyme */
	double		 protein_mw;				/* Calculated protein molecular weight (assuming monoisotopic masses */
    char*        peptide_prev_aa;			/* Aminoacid preceding peptide (- if none) */
	char*        peptide_next_aa;			/* Aminoacid following peptide (- if none) */

} alternative_protein, *palternative_protein;


/* search_hit element. */
typedef struct pepxml_search_hit 
{
	/* Attributes */
    unsigned int  hit_rank;					/* Required */
	char*         peptide;					/* Peptide aminoacid sequence (with no indicated modifications) */
    char*         peptide_prev_aa;			/* Aminoacid preceding peptide (- if none) */
	char*         peptide_next_aa;			/* Aminoacid following peptide (- if none) */
	char*         protein;					/* Required */
	int			  num_tot_proteins;			/* Number of unique proteins in search database containing peptide */
	int			  num_matched_ions;			/* Number of peptide fragment ions found in spectrum */
	unsigned int  tot_num_ions;				/* Number of peptide fragment ions predicted for peptide */
	double        calc_neutral_pep_mass;	/* Required  */
	double		  massdiff;					/* Mass(precursor ion) - Mass(peptide) */
	int           num_tol_term;				/* Number of peptide termini consistent with cleavage by sample enzyme */
	int           num_missed_cleavages;		/* Number of sample enzyme cleavage sites internal to peptide */
	unsigned char is_rejected;				/* Potential use in future for user manual validation (0 or 1) */
	char*         protein_descr;			/* Extracted from search database */ 	
	double		  calc_pI;					/* Calculated isoelectric point */
	double		  protein_mw;				/* Calculated protein molecular weight (assuming monoisotopic masses */

	/* Elements */
	/* Alternatives section */
	palternative_protein alternative_protein_array;
	int                   alternative_protein_count;

	/* Modification info section */
	pmodification_info modification_info_struct;

	/* Search score section */
    psearch_score search_score_array;
	int           search_score_count;

	/* Analysis result section */
	panalysis_result analysis_result_array;
	int	             analysis_result_count;

	/* Parameter section */
	pparameter parameter_array;
	int	       parameter_count;

} search_hit, *psearch_hit;


/* search_result element. */
typedef struct pepxml_search_result
{
	/* Attributes */
	int search_id;	/* Unique identifier to search summary */

	/* Child elements */
	psearch_hit search_hit_array;	/* Peptide assignment array */
	int			search_hit_count;	/* Length of said array */

} search_result, *psearch_result;


/* spectrum_query element */
typedef struct pepxml_spectrum_query
{	
    char*			spectrum;				/* Required */
	unsigned int    start_scan;				/* First scan number integrated into MS/MS spectrum */
	unsigned int    end_scan;				/* Last scan number integrated into MS/MS spectrum */
	double			retention_time_sec;		/* Retention time associated with start_scan */
	char*			activation_method;		/* Activation or fragmentation method: ETD, ECD, CID, etc */
	double			precursor_neutral_mass;	/* Required */
	unsigned int    assumed_charge;			/* Precursor ion charge used for search */
	char*			search_specification;	/* Search constraint applied specifically to this query */
	int				index;					/* index corresponding to search query number */
	/* Ignored attributes */
	
	
	/* Search result section */
	psearch_result	search_result_array;	/* Array of all search results in the spectrum query */
	int				search_result_count;	/* Length of said array */
	
} spectrum_query, *pspectrum_query;


/* Analysis timestamp stub, to be implemented later */
typedef struct pepxml_analysis_timestamp
{
	char * contents;

} analysis_timestamp, *panalysis_timestamp;


/* Search summary stub, to be implemented later */
typedef struct pepxml_search_summary
{
	char * contents;

} search_summary, *psearch_summary;


/* Msms run summary element */
typedef struct pepxml_specifity
{
	/* Attributes */
	char*		 sense;			/* C, N: Defines whether cleavage occurs on the C-terminal or N-terminal side of the residue(s) listed in cut */
	unsigned int min_spacing;	/* minimum separation between adjacent cleavages */
	char*		 cut;			/* [A,C-I,K-N,P-T,VWY]+ One or more 1-letter residue codes. Enzyme cleaves on the sense side of the residue(s) 
								   listed in cut unless one of the residues listed in no_cut is adjacent to the potential cleavage site */
	char*		 no_cut;		/* Zero or more 1-letter residue codes. Enzyme cleaves on the sense side of the residue(s) listed in cut unless 
								   one of the residues listed in no_cut is adjacent to the potential cleavage site */
	
} specifity, *pspecifity;


/* Msms run summary element */
typedef struct pepxml_sample_enzyme
{
	/* Attributes */
	char* name;						/* Controlled code name for the enzyme that can be referred to by applications */
	char* description;				/* Free text to describe alternative names, special conditions, etc. */
	char* fidelity;					/* Semispecific means that at least one end of a pepide must conform to the cleavage specificity, 
									   (unless the peptide was at the terminus of the parent sequence). Nonspecific means that neither 
									   end of a peptide must conform to the cleavage specificity. */
	unsigned char* independent;		/* Boolean. If there are multiple specificities and independent is true, then a single peptide cannot 
									   exhibit one specificity at one terminus and a different specificity at the other. If independent 
									   is false, then a single peptide can exhibit mixed specificities.*/

	pspecifity specifity_array;		/* Array of specifity indicators for the sample enzyme */
	int		   specifity_count;		/* Length of said array */
	
} sample_enzyme, *psample_enzyme;


/* Msms run summary element */
typedef struct pepxml_msms_run_summary
{	
	/* Attributes */	
	char* base_name;		/* Full path file name of mzXML (minus the .mzXML)	*/
	char* raw_data_type;	/* Raw data type extension (e.g. .mzXML) */
	char* raw_data;			/* Raw data type */
	char* msManufacturer;	/* Manufacturer of MS/MS instrument */
	char* msModel;			/* Instrument model (cf mzXML) */
	char* msIonization;		/* Instrument model (cf mzXML) */
	char* msMassAnalyzer;	/* Ion trap, etc (cf mzXML) */
	char* msDetector;		/* EMT, etc(cf mzXML) */

	/* Elements */
	long		   sample_enzyme_offset;	// Offset of sample enzymes
	long		   sample_enzyme_length;	// Offset of sample enzymes part
	psample_enzyme sample_enzyme_element;	// Pointer to sample enzymes struct

	long		    search_summary_offset;	/* Offset of search summaries */
	long		    search_summary_length;	/* Length of search summaries */
	psearch_summary search_summary_array;	/* Array of search summaries */
	int			    search_summary_count;	/* Size of said array */

	long				analysis_timestamp_offset;	/* Offset of analysis timestamps */
	long				analysis_timestamp_length;	/* Length of analysis timestamps */
	panalysis_timestamp analysis_timestamp_array;	/* Array of analysis timestamps */
	int					analysis_timestamp_count;	/* Length of said array */

	pspectrum_query spectrum_query_array;	/* Array of spectrum queries */
	int				spectrum_query_count;	/* Length of said array */

} msms_run_summary, *pmsms_run_summary;


/* Data filter element */
typedef struct pepxml_data_filter
{	
	/* Attributes */
	unsigned int number;		 /* Required filter number */
	char*        parent_file;	 /* File from which derived */
	char*		 windows_parent; /* Parental window description */
	char*        description; 	 /* Filtering criteria applied to data */

} data_filter, *pdata_filter;


/* Dataset derivation element */
typedef struct pepxml_dataset_derivation
{	
	/* Attributes */
	unsigned int generation_no;	/* number preceding filter generations */	
	
	/*Elements */
	pdata_filter data_filter_array;	/* Array of applied data filters */
	int          data_filter_count; /* Length of said array */

} dataset_derivation, *pdataset_derivation;


/* Analysis summary element */
typedef struct pepxml_analysis_summary
{	
	/* Attributes */
	char* time;		/* Time analysis complete (unique id) */
	char* analysis;	/* Name of analysis program */
	char* version;	/* Program version */

	/* Value */
	void* handle;	/* Contents of the analysis body */

} analysis_summary, *panalysis_summary;


/* Pipeline analysis element */
typedef struct pepxml_msms_pipeline_analysis
{	
	/* File name, for logging purposes */
	char* file_name;
	long start_offset;

	/* Attributes */
	char* name;			/* Summary name (currently not used) */
	char* date;			/* Date pepXML file was written */
	char* summary_xml;	/* Full path self reference */


	/* Elements */
	long				analysis_summary_offset;	/* Offset of the analysis summary element */
	long				analysis_summary_length;	/* Length of the analysis summary element */
	panalysis_summary	analysis_summary_array;		/* Array of analysis summaries */
	int					analysis_summary_count;		/* Size of said array */

	long				dataset_derivation_offset;	/* Offset of the dataset derivation element */
	long				dataset_derivation_length;	/* Length of the dataset derivation element */
	pdataset_derivation	dataset_derivation_struct;	/* Dataset derivation element */

	pmsms_run_summary	run_summary_array;			/* Array of all run summaries */
	int					run_summary_count;			/* Size of said array */
	
} msms_pipeline_analysis, *pmsms_pipeline_analysis;


#ifdef __cplusplus
}
#endif

#endif
