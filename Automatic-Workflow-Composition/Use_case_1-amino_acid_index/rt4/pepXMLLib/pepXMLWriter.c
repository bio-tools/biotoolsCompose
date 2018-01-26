#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "pepXMLWriter.h"
#include "pepXMLWriteFunctions.h"


/* Writes the entire file. Copies all residual info from the file recorded in pepxml_file */
void write_pepxml_file(pmsms_pipeline_analysis pepxml_file, char* dest_file_name, pdelegate_list dlist)
{
	write_msms_pipeline_analysis(pepxml_file, dest_file_name, dlist);

}/* write_pepxml_file(pmsms_pipeline_analysis pepxml_file, char* dest_file_name, pdelegate_list dlist) */
