#include <stdlib.h>
#include <string.h>
#include "pepXMLDelegates.h"


/* Function that creates and configures a delegate */
pdelegate_type make_delegate(char* key, char* context, void* (*pread_function)(char*), char* (*write_function)(void*), void* (*property_function)(char*, void*))
{
	pdelegate_type retval = (pdelegate_type) malloc(sizeof(delegate_type));
	
	retval->key = key;
	retval->context = context;
	retval->pread_function = pread_function;
	retval->write_function = write_function;
	retval->property_function = property_function;

	return retval;

}/* pdelegate_type make_delegate(char* key, void* (*pread_function)(void**, int), char* (*write_function)(void**, int), void* (*property_function)(char*)) */


/* Function that creates a delegate list */
pdelegate_list make_delegate_list()
{
	pdelegate_list retval = (pdelegate_list) malloc(sizeof(delegate_list));

	retval->dlist = NULL;
	retval->size = 0;

	return retval;

}/* pdelegate_list make_delegate_list() */


/* Function that adds a delegate to the list */
void add_delegate_to_list(pdelegate_list list, pdelegate_type delegate_t)
{
	int i;						/* loopvar */
	pdelegate_type* tmparray;	/* delegate container */

	if (list->size == 0)
	{
		list->dlist = (pdelegate_type*) malloc(sizeof(delegate_type*));
		list->dlist[0] = delegate_t;
		list->size = 1;

	}/* if */
	else 
	{
		tmparray = list->dlist;
		list->dlist = malloc(sizeof(delegate_list*) * (list->size + 1));

		for (i=0; i<list->size; i++)		
			list->dlist[i] = tmparray[i];

		list->dlist[i] = delegate_t;
		list->size += 1;
		free(tmparray);
	}/* else */

}/* void add_delegate_to_list(pdelegate_list list, pdelegate_type delegate_t) */


/* Function that helps finding the correct delegate, NULL if key is not found in the array */
pdelegate_type get_delegate(char* key, char* context, pdelegate_list list)
{
	int i;	/* loop var */

	if (!key || !list)
		return NULL;

	for (i=0; i<list->size; i++) {
		if (strcmp(key, list->dlist[i]->key) == 0) {
			if (!context)
				return list->dlist[i];
			else if (strcmp(context, list->dlist[i]->context) == 0)
				return list->dlist[i];
		}// if			
	}/* for */

	return NULL;

}/* pdelegate_type get_delegate(char* key, pdelegate_type[], int size) */
