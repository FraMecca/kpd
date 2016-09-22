#include <getopt.h> // for getops_long
#include <stdlib.h> // exit
#include <stdio.h> // true false
#include <stdbool.h> // printf
#include <string.h> // strdup
#include "parse_args.h"


#define OPTION_NOT_FOUND(x) fprintf (stderr, "kpd: invalid option '%s'\nTry 'kpd --help' for more information.\n", x);


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

static int count_n_args (int argc, char **argv)
{
	int i, cnt = 0;
	for (i = 0; i < argc; ++i) {
		// check if 
		if (argv[i][0] == '-') {
			cnt++;
		}
	}
	return cnt;
}

bool short_option_exist (const char opt, const functionTable *functions, const int n)
{
	int i;
	// iterate over functions and check if opt is one of the short options
	for (i = 0; i < n; ++i) {
		if (functions[i].shortOption == opt) {
			return true;
		}
	}
	OPTION_NOT_FOUND(&opt);
	return false;
}

bool long_option_exist (const char *opt, const functionTable *functions, const int n)
{
	int i;
	// iterate over functions and check if opt is one of the long options
	for (i = 0; i < n; ++i) {
		if (strcmp (functions[i].name, opt) == 0) {
			return true;
		}
	}
	OPTION_NOT_FOUND(opt);
	return false;
}

bool check_sanity (int argc, char **argv, const functionTable *functions, const int n)
{
	// check for argv sanity 
	// for now funct is shit and only checks that there is no '--' or '----' and more dashes (sigsev)
	// will improve with time
	int i;
	while (--argc > 0) {
		// check if argv[argc] is in functionTable
		if (argv[argc][0] == '-') {
			if (argv[argc][1] != '-') {
				// it is a single dash option
				// given that -ap is two corrent single dash options, it should be considered as such
				// now check if such option exist in functionTable.shortOption
				for (i = 1; i < strlen (argv[argc]); ++i) {
					if (short_option_exist (argv[argc][i], functions, n) == false) {
						return false;
					}
				}
			} else {
				// it a doubledash option
				if (long_option_exist (argv[argc] + 2, functions, n) == false) {
					return false;
				}
			}
		}
	}
	return true;
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

static bool parse_args (int argc, char **argv, struct ArgumentsStruct *argSt, struct option *long_options, char *getoptShortOptStr)
{
	int option_index = 0, pos;	
	char ch;
	
	ch = getopt_long (argc, argv, getoptShortOptStr, long_options, &option_index);

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

static bool push_to_table_sequentially (argumentsArray args, functionTable *functions, int n, void *structused)
{
	/* if order flag == false 
	 * functions are evaluated in the order they apeear in the command line # sequential
	 * else if order flag == true
	 * functions will be evaluated in the order they appear in functionstable
	 */
	
	int i, j;
	bool flag = false, ret;

	for (i = 0; i < args.size; ++i) {
		for (j = 0; j < n; ++j) {
			if (strcmp (functions[j].name, args.arguments[i].functionName) == 0 || 
					(strlen (args.arguments[i].functionName) == 1 &&
 					 args.arguments[i].functionName[0] == functions[j].shortOption)) {
				// execute function on functionTable
				ret = functions[j].functionPtr (structused, args.arguments[i].values, args.arguments[i].nValues);
				if (ret == false) return false;
				// halt process_cli if one of the functions return false
				flag = true;
			}
		}
	}
	return flag;
}

static bool push_to_table_ordered (argumentsArray args, functionTable *functions, int n, void *structused)
{
	// same as push_to_table_sequentially, but the functions are evaluated in the order they appear in the struct
	int i, j;
	bool flag = false, ret; // this flag is set to true if a function is executed

	for (j = 0; j < n; ++j) {
		for (i = 0; i < args.size; ++i) {
			if (strcmp (functions[j].name, args.arguments[i].functionName) == 0 || 
					(strlen (args.arguments[i].functionName) == 1 &&
 					 args.arguments[i].functionName[0] == functions[j].shortOption)) {
				// execute function on functionTable
				ret = functions[j].functionPtr (structused, args.arguments[i].values, args.arguments[i].nValues);
				if (ret == false) return false;
				// halt process_cli if one of the functions return false

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

char *create_getopts_short_options_string (const functionTable *functions, const int nFunctions)
{
	/* getopts wants a string that specify the singledash short options
	 * a:P::ln:
	 * in such example -a requires an argument, -P rcan accept arguments, -l does not accept argument, -n requires an argument
	 * this function generates a string with all the singledash options with a '::' appended;
	 * this library does not validate user input, so no number of options check.
	 */
	int i;
	char *getoptShortOptStr = (char *) malloc (3*nFunctions * sizeof (char));
	memset (getoptShortOptStr, 0, 3 * nFunctions);  
	for (i = 0; i < 2*nFunctions; i += 3) {
		getoptShortOptStr[i] = functions[i / 2].shortOption;
		getoptShortOptStr[i + 1] = ':';
		getoptShortOptStr[i + 2] = ':';
	}
	return getoptShortOptStr;
}

bool process_cli (int argc, char **argv, struct option * long_options, functionTable * functions, int nFunctions, void *structused, bool orderFlag)
{
	 /* process command line arguments, sequantialyl or ordered
	  * returns a flag if an argument is found and a function is executed
	  */
	char *getoptShortOptStr;
	int idx, cnt = 0;
	bool flag;
	argumentsArray args = initialize_argsarray (argc, argv);
	// initialize without populate

		// check for argv sanity 
		// for now funct is shit and only checks that there is no '--' or '----' and more dashes (sigsev)
		// will improve with time
	if (check_sanity (argc, argv, functions, nFunctions) == false) {
		return false;
	}

	getoptShortOptStr = create_getopts_short_options_string (functions, nFunctions); 
	while ((idx = parse_args (argc, argv, &args.arguments[cnt++], long_options, getoptShortOptStr)) > 0) {
		/* parse the command line and populates args with arguments and values */
	}
	free (getoptShortOptStr);

	if (orderFlag == false) {
		flag = push_to_table_ordered (args, functions, nFunctions, structused);
	// same as push_to_table_sequentially, but the functions are evaluated in the order they appear in the struct
	} else {
		flag = push_to_table_sequentially (args, functions, nFunctions, structused);
	}
	/* execute every function linked to the commandline option */
	destroy_struct (args);	

	return flag;
}
