#ifndef __PEPXML_DELEGATES_H__
#define __PEPXML_DELEGATES_H__

#ifdef __cplusplus
extern "C"
{
#endif
/*
	Here we define a rudimentary delegate structure and interfaces that allows the future 
	implementation of reading, printing and analyzing of custom algorithm parts of the
	pepxml files.
*/

/*	Interface definition. Each custom algorithm should provide 3 functions:
		- key: key that will trigger the use of the specified funct ions
		- read function, reads the intended part designated with the given key
		- write function, writes the custom format to a string of type char*
		- property function, returns the value of a certain key somewhere stored in the custom structure
*/
typedef struct pepxml_delegate
{
	char* key;
	char* context;
	void* (*pread_function)(char*);
	char* (*write_function)(void*);
	void* (*property_function)(char*, void*);

} delegate_type, *pdelegate_type;


/* List of delegates */
typedef struct pepxml_delegate_list
{
	pdelegate_type*	dlist;	/* list of pointers to delegates */
	int				size;	/* size of the list */

} delegate_list, *pdelegate_list;


/* Function that creates and configures a delegate */
pdelegate_type make_delegate(char* key, char* context, void* (*pread_function)(char*), char* (*write_function)(void*), void* (*property_function)(char*, void*));


/* Function that creates a delegate list */
pdelegate_list make_delegate_list();

/* Function that adds a delegate to the list */
void add_delegate_to_list(pdelegate_list list, pdelegate_type delegate_t);

/* Function that helps finding the correct delegate, NULL if key is not found in the array */
pdelegate_type get_delegate(char* key, char* context, pdelegate_list list);

#ifdef __cplusplus
}
#endif

#endif
