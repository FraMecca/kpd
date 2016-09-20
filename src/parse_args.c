#include <getopt.h> // for getops_long
#include <stdlib.h> // exit
#include <stdio.h> // true false
#include <stdbool.h> // printf
#include <string.h> // strdup
#include "parse_args.h"

struct ArgumentsStruct {
	char *functionName;
	char **values;
	int nValues;
};
/* In this structure will be stored all the arguments and related options */

/*
 * struct ArgumentsStruct {
 * char *functionName = '--add';
 * char **values = {"simulazione.pdf", "matematica", "scuola"};
 * int nValues = 3;
 *
 */

typedef struct argumentsArray {
	struct ArgumentsStruct *arguments;
	int size;
} argumentsArray;

/* prototypes */
static int count_n_args (int argc, char **argv);
static void insert_argument (int argc, char **argv, int pos, struct ArgumentsStruct *argSt);
static bool parse_args (int argc, char **argv, struct ArgumentsStruct *argSt, struct option *long_options);
static argumentsArray initialize_argsarray (int argc, char **argv);
static bool push_to_table_ordered (argumentsArray args, functionTable *functions, int n);
static bool push_to_table_sequentially (argumentsArray args, functionTable *functions, int n);
static void destroy_struct (argumentsArray args);
/* end prototypes */

static int count_n_args (int argc, char **argv)
{
	int i, cnt = 0;
	for (i = 0; i < argc; ++i) {
		if (argv[i][0] == '-') {
			cnt++;
		}
	}
	return cnt;
}

static void insert_argument (int argc, char **argv, int pos, struct ArgumentsStruct *argSt)
{
	int i;

	if (argv[pos][1] == '-') { // long option
		argSt->functionName = strdup (argv[pos] + 2);
	} else {
		argSt->functionName = strdup (argv[pos] + 1);
	}

	for (i = 1; i + pos < argc && argv[pos + i][0] != '-'; i++);
		// now i = number of values for argument;
	argSt->nValues = --i;
	argSt->values = (char **) malloc (i * sizeof (char *));
	
	// now assign every value to value array
	for (i = 0; i < argSt->nValues; ++i) {
		/*printf ("%s\n", argv[pos+i + 1]);	*/
		argSt->values[i] = strdup (argv[pos + i + 1]);
	}
}

static bool parse_args (int argc, char **argv, struct ArgumentsStruct *argSt, struct option *long_options)
{
	int option_index = 0, pos;	
	char ch;
	
	ch = getopt_long (argc, argv, "c:a:m:ls:", long_options, &option_index);

	if (ch == -1) {
		return false;
	} else {
		/* '?' is managed internally by getopts */
		/*if (ch == '?' ) {*/
			/*printf ("Invald option, see --help\n");*/
			/*exit (-1);*/
			/*} else {*/
		/* getopts returns the pos of the first arguments in case the 
		 * long_option needs an argument, else
		 * it return the pos of the option;
		 * for this reason the switch is needed
		 */

				switch (argv[optind - 1][0]) {
					case '-':
						pos = optind - 1;
						break;
					default:
						pos = optind - 2;
						break;
				}
				insert_argument (argc, argv, pos, argSt); 
				return pos; 
			}
}

static argumentsArray initialize_argsarray (int argc, char **argv)
{
	argumentsArray args;

	args.size = count_n_args (argc, argv);
	args.arguments = (struct ArgumentsStruct *) malloc (args.size * sizeof (struct ArgumentsStruct));

	return args;
}

static bool push_to_table_sequentially (argumentsArray args, functionTable *functions, int n)
{
	// functions are evaluated in the order they apeear in the command line
	int i, j;
	bool flag = false;

	for (i = 0; i < args.size; ++i) {
		for (j = 0; j < n; ++j) {
			if (strcmp (functions[j].name, args.arguments[i].functionName) == 0 || 
					(strlen (args.arguments[i].functionName) == 1 &&
 					 args.arguments[i].functionName[0] == functions[j].shortOption)) {
				// execute function on functionTable
				functions[j].functionPtr (args.arguments[i].values, args.arguments[i].nValues);
				flag = true;
			}
		}
	}
	return flag;
}

static bool push_to_table_ordered (argumentsArray args, functionTable *functions, int n)
{
	// same as push_to_table_sequentially, but the functions are evaluated in the order they appear in the struct
	int i, j;
	bool flag = false; // this flag is set to true if a function is executed

	for (j = 0; j < n; ++j) {
		for (i = 0; i < args.size; ++i) {
			if (strcmp (functions[j].name, args.arguments[i].functionName) == 0 || 
					(strlen (args.arguments[i].functionName) == 1 &&
 					 args.arguments[i].functionName[0] == functions[j].shortOption)) {
				// execute function on functionTable
				functions[j].functionPtr (args.arguments[i].values, args.arguments[i].nValues);
				flag = true;
			}
		}
	}
	return flag;
}

static void destroy_struct (argumentsArray args)
{
	int i,j;

	for (i =0; i < args.size; ++i) {
		for (j = 0; j < args.arguments[i].nValues; ++j) {
			free (args.arguments[i].values[j]);
		}
		free (args.arguments[i].values);
		free (args.arguments[i].functionName);
	}
	free (args.arguments);
}


bool process_cli (int argc, char **argv, struct option * long_options, functionTable * functions, int nFunctions, bool orderFlag)
{
	 /* process command line arguments, sequantialyl or ordered
	  * returns a flag if an argument is found and a function is executed
	  */
	argumentsArray args = initialize_argsarray (argc, argv);
	int idx, cnt = 0;
	bool flag;

	while ((idx = parse_args (argc, argv, &args.arguments[cnt++], long_options)) > 0) {
		/* parse the command line and populates args with arguments and values */
	}

	if (orderFlag == false) {
		flag = push_to_table_ordered (args, functions, nFunctions);
	} else {
		flag = push_to_table_sequentially (args, functions, nFunctions);
	}
	/* execute every function linked to the commandline option */
	destroy_struct (args);	

	return flag;
}
