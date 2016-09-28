#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

#include <stdbool.h> // true false

/* 
 * USAGE:
  
 *
 * In this struct put every argument needed.

typedef struct functionTable {
	char *name;
	char shortOption;
	void* (*functionPtr) (char **, int);
} functionTable;

static functionTable functions[] = {
	{"add", 	'a', 	(void *) &util_add},
	{"create", 	'c', 	(void *) &create_storage},
	{"list", 	'l',	(void *) &list_options},
	{"show", 	's', 	(void *) &funct},
};

 * functionTable is used to call the functions that corresponds to the various arguments:
 * {"argument in cli", 'short dash option', (void  *) pointer to function}
 * every prototype of the function should be:
 * (type) funct (char **, int)
 * because parse_args.c will pass a generic struct, char** and the dimension to the function pointer.
 *
 * process_cli is the main interface, it halts in case one of the functions returns false.
 */

typedef struct functionTable {
	char *name;
	char shortOption;
	void* (*functionPtr) (char **, int);
} functionTable;	

bool process_cli (int argc, char **argv, functionTable * functions, int nFunctions, bool orderFlag);

#endif
