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

static int
is_long_option (const char *arg, const functionTable *functions, const int n)
{
	int i;
	if (arg[0] == '-' && arg[1] == '-' && strlen (arg) > 2) {
		// in this way if arg == --play tmp == play
		for (i = 0; i < n; ++i) {
			if (strncmp (functions[i].name, arg + 2, strlen (functions[i].name)) == 0) {
				return 1;
			}
		}
	}
	// else
	// arg doesn't start with '--' so it is not an argument
	return 0;
}

static int
is_short_option (const char *arg, const functionTable *functions, const int n)
{
	int i, j, cnt = 0;
	if (arg[0] == '-' && arg[1] != '-' && strlen (arg) > 1) {
		for (i = 0; i < n; ++i) {
			for (j = 1; j < strlen (arg); ++j) {
				// short options can be something like -pl where p is valid and l is valid
				if (functions[i].shortOption != '0' && arg[j] == functions[i].shortOption) {
					cnt++;
				}
			}
		}
	}
	return cnt;
}


static int count_n_args (int argc, char **argv, functionTable *functions, int n)
{
	int i, cnt = 0;
	for (i = 0; i < argc; ++i) {
		cnt += is_short_option (argv[i], functions, n);
		cnt += is_long_option (argv[i], functions, n);
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
	// will check if the args exist,
	// if it doesn't it check if it can be a value for a precedent 
	return true;
}

static void insert_argument (int argc, char **argv, int pos, int shortPos, struct ArgumentsStruct *argSt, functionTable *functions, int n)
{
	int i;

	// shortPos is used to consider short options in this form: -pla; it is ignored when long options
	if (argv[pos][1] == '-') { // long option
		// initialize the functionName with the argument that called it
		argSt->functionName = strdup (argv[pos] + 2);
	} else {
		// initialize the functionName with the argument that called it
		argSt->functionName = strdup (argv[pos] + shortPos); //shortPos is at least one, so no need to add something for initial -
	}

	for (i = 1; i + pos < argc; i++) {
		// iterate until next valid argument is found, increment i
		if (is_long_option (argv[pos + i], functions, n) || is_short_option (argv[pos + i], functions, n)) {
			break;
		}
	}
		// now i = number of values for argument;
	argSt->nValues = --i;
	argSt->values = (char **) calloc (i, sizeof (char *));
	
	// now assign every value to value array
	for (i = 0; i < argSt->nValues; ++i) {
		/*printf ("%s\n", argv[pos+i + 1]);	*/
		argSt->values[i] = strdup (argv[pos + i + 1]);
	}
}

static void parse_args (int argc, char **argv, argumentsArray argA, functionTable *functions, int n)
{
	int i, j, pos = 0;
	for (i = 0; i < argc; ++i) {
		if (is_short_option (argv[i], functions, n)) {
			for (j = 1; j < strlen (argv[i]); ++j) {
				insert_argument (argc, argv, i, j, &argA.arguments[pos], functions, n); 
				pos++;
			}
		} else {
			if (is_long_option (argv[i], functions, n)) { 
				insert_argument (argc, argv, i, 0, &argA.arguments[pos], functions, n);
				pos++;
			}
		}
	}
}

static argumentsArray initialize_argsarray (int argc, char **argv, functionTable *functions, int n)
{
	argumentsArray args;

	args.size = count_n_args (argc, argv, functions, n);
	args.arguments = (struct ArgumentsStruct *) calloc (args.size , sizeof (struct ArgumentsStruct));
	/*memset (args.arguments, 0, args.size * sizeof (char));*/


	return args;
}

static bool push_to_table_sequentially (argumentsArray args, functionTable *functions, int n)
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
				ret = functions[j].functionPtr (args.arguments[i].values, args.arguments[i].nValues);
				if (ret == false) return false;
				// halt process_cli if one of the functions return false
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
	bool flag = false, ret; // this flag is set to true if a function is executed

	for (j = 0; j < n; ++j) {
		for (i = 0; i < args.size; ++i) {
			if (strcmp (functions[j].name, args.arguments[i].functionName) == 0 || 
					(strlen (args.arguments[i].functionName) == 1 &&
 					 args.arguments[i].functionName[0] == functions[j].shortOption)) {
				// execute function on functionTable
				ret = functions[j].functionPtr ( args.arguments[i].values, args.arguments[i].nValues);
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

bool process_cli (int argc, char **argv, functionTable * functions, int nFunctions, bool orderFlag)
{
	 /* process command line arguments, sequantialyl or ordered
	  * returns a flag if an argument is found and a function is executed
	  */
	bool flag;
	argumentsArray args = initialize_argsarray (argc, argv, functions, nFunctions);
	// initialize without populate

		// check for argv sanity 
		// for now funct is shit and only checks that there is no '--' or '----' and more dashes (sigsev)
		// will improve with time
	if (check_sanity (argc, argv, functions, nFunctions) == false) {
		return false;
	}

	parse_args (argc, argv, args, functions, nFunctions);
		/* parse the command line and populates args with arguments and values */

	if (orderFlag == false) {
		flag = push_to_table_ordered (args, functions, nFunctions);
	// same as push_to_table_sequentially, but the functions are evaluated in the order they appear in the struct
	} else {
		flag = push_to_table_sequentially (args, functions, nFunctions);
	}
	/* execute every function linked to the commandline option */
	destroy_struct (args);	

	return flag;
}
