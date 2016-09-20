#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H

#include <getopt.h> // option datastructure
#include <stdbool.h> // true false

/* 
 * USAGE:
  
static struct option long_options[] = {    // args neeed:
	{"create", required_argument, 0, 'c'}, // 1+
	{"add",	   required_argument, 0, 'a'}, // 1+
	{"merge",  required_argument, 0, 'm'}, // 2+
	{"list",   optional_argument, 0, 'l'}, // 0
    {"show",   no_argument, 0, 's'}, // 1+	
};

 * option is used by getopt:
 * { "name of option, double dash", argument_required, NULL or 0 or a flag, 'single dash option' or 0 }
 * in particular, NULL or 0 is used when there is no necessity for a flag, if needed put a variable in the field
 * 'single dash option' is particolary useful with switch cases: if argument is found, the 'singledashoption' will be returned'.
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
 * because parse_args.c will pass a char** and the dimension to the function pointer.
 *
 * process_cli is the main interface.
 */

typedef struct functionTable {
	char *name;
	char shortOption;
	void* (*functionPtr) (char **, int);
} functionTable;	

bool process_cli (int argc, char **argv, struct option * long_options, functionTable * functions, int nFunctions, bool orderFlag);

#endif
