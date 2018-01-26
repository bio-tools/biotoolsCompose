#ifndef __PEPXML_WRITER_H__
#define __PEPXML_WRITER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include "pepXMLStructures.h"
#include "pepXMLDelegates.h"

/* Writes the entire file. Copies all residual info from the file recorded in pepxml_file */
void write_pepxml_file(pmsms_pipeline_analysis pepxml_file, char* dest_file_name, pdelegate_list dlist);

#ifdef __cplusplus
}
#endif

#endif
