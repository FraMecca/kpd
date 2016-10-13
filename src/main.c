#include <stdio.h> // printf
#include <getopt.h> // long_option struct
/*#include"parse_args.h" // function table struct*/
#include "util.h" // handle for libmpdclient
#include "kpd_search.h"
#include <argp.h>

static struct argp_option optionsHOSTandPORT[] = {
	{"host", 0, 0, OPTION_ARG_OPTIONAL, "host"},
	{"port", 0, 0, OPTION_ARG_OPTIONAL, "port"},
	{0}
};

static struct argp_option filtersArgp[] = {
	{"filter", 'f', 0, OPTION_ARG_OPTIONAL, "filter"},
	{"vfilter", 'v', 0, OPTION_ARG_OPTIONAL, "vfilter"},
	{"play", 	'p',	 "NUM",	 OPTION_ARG_OPTIONAL,	 "play"},
	{"pause",	'P',	  0,     OPTION_ARG_OPTIONAL, 	 "pause"},
	{"next",    'n',      0,	 OPTION_ARG_OPTIONAL,    "next"},
	{"previous",'b',      0,     OPTION_ARG_OPTIONAL,    "previous"},
	{"prev",    'b',      0,     OPTION_ARG_OPTIONAL,    "previous"},
	{"stop",    600,      0,     OPTION_ARG_OPTIONAL,    "stop"},
	{"clear",   'c',      0,     OPTION_ARG_OPTIONAL,    "clear"},
	{"random",	'r',	  0,	 OPTION_ARG_OPTIONAL,	 "random"},
	{"shuffle", 't',      0,     OPTION_ARG_OPTIONAL,    "shuffle"},
	{"shuffle-range", 604,0,     OPTION_ARG_OPTIONAL,    "shuffle_range"},
	{"update",  'u',      0,     OPTION_ARG_OPTIONAL,    "update"},
	{"add",     'a',      0,     OPTION_ARG_OPTIONAL,    "add"},
	{"search",  's',      "key", OPTION_ARG_OPTIONAL,    "search"},
	{"list",    'l',      0,     OPTION_ARG_OPTIONAL, 	 "list"},
	{"seek", 	'S',      "NUM[%]", OPTION_ARG_OPTIONAL, "seek"},
	{"forward", 'F',      "NUM[%]", OPTION_ARG_OPTIONAL, "forward"},
	{"backward",'B',      "NUM[%]", OPTION_ARG_OPTIONAL, "backward"},
	{"consume", 601,      0, 	 OPTION_ARG_OPTIONAL,    "consuma e produci"},
	{"repeat",  602, 	  0,	 OPTION_ARG_OPTIONAL,    "repeat"},
	{"single",  603,       0,     OPTION_ARG_OPTIONAL,    "single"},
	{"swap",	'w',	  "pos1 pos2", OPTION_ARG_OPTIONAL,"swap"},
	{"verbose", 'v',	  0,	OPTION_ARG_OPTIONAL,	  "verbose"},
	{"delete",  'd',	 "NUM",   OPTION_ARG_OPTIONAL,	 "delete"},
	{"delete-range", 'D',"NUM1-NUM2", OPTION_ARG_OPTIONAL,"delete_range"},
	{0}
};

void funct ()
{
	return;
}

static struct argp_option options[] = {
		{"filter", 'f', 0, OPTION_ARG_OPTIONAL, "filter"},
		{"vfilter", 'v', 0, OPTION_ARG_OPTIONAL, "vfilter"},
		{"play", 	'p',	 "NUM",	 OPTION_ARG_OPTIONAL,	 "play"},
		{"pause",	'P',	  0,     OPTION_ARG_OPTIONAL, 	 "pause"},
		{"next",    'n',      0,	 OPTION_ARG_OPTIONAL,    "next"},
		{"previous",'b',      0,     OPTION_ARG_OPTIONAL,    "previous"},
		{"prev",    'b',      0,     OPTION_ARG_OPTIONAL,    "previous"},
		{"stop",    600,      0,     OPTION_ARG_OPTIONAL,    "stop"},
		{"clear",   'c',      0,     OPTION_ARG_OPTIONAL,    "clear"},
		{"random",	'r',	  0,	 OPTION_ARG_OPTIONAL,	 "random"},
		{"shuffle", 't',      0,     OPTION_ARG_OPTIONAL,    "shuffle"},
		{"shuffle-range", 604,0,     OPTION_ARG_OPTIONAL,    "shuffle_range"},
		{"update",  'u',      0,     OPTION_ARG_OPTIONAL,    "update"},
		{"add",     'a',      0,     OPTION_ARG_OPTIONAL,    "add"},
		{"search",  's',      "key", OPTION_ARG_OPTIONAL,    "search"},
		{"list",    'l',      0,     OPTION_ARG_OPTIONAL, 	 "list"},
		{"seek", 	'S',      "NUM[%]", OPTION_ARG_OPTIONAL, "seek"},
		{"forward", 'F',      "NUM[%]", OPTION_ARG_OPTIONAL, "forward"},
		{"backward",'B',      "NUM[%]", OPTION_ARG_OPTIONAL, "backward"},
		{"consume", 601,      0, 	 OPTION_ARG_OPTIONAL,    "consuma e produci"},
		{"repeat",  602, 	  0,	 OPTION_ARG_OPTIONAL,    "repeat"},
		{"single",  603,       0,     OPTION_ARG_OPTIONAL,    "single"},
		{"swap",	'w',	  "pos1 pos2", OPTION_ARG_OPTIONAL,"swap"},
		{"verbose", 'v',	  0,	OPTION_ARG_OPTIONAL,	  "verbose"},
		{"delete",  'd',	 "NUM",   OPTION_ARG_OPTIONAL,	 "delete"},
		{"delete-range", 'D',"NUM1-NUM2", OPTION_ARG_OPTIONAL,"delete_range"},
		{0}
};

struct functionTable {
	int key;
	void * (*functionPtr) (char **, int);
};

static struct functionTable filters[] = {
	{'f', 	(void *) &filter_helper},
	{'v', 	(void *) &vfilter_helper},
	{'f', 	(void *) &funct},
	{'v', 	(void *) &funct},
	{'p', 	(void *) &funct},
	{'P', 	(void *) &funct},
	{'n',	(void *) &funct},
	{'b', 	(void *) &funct},
	{600, 	(void *) &funct},
	{'c',	(void *) &funct},
	{'r', 	(void *) &funct},
	{'t',	(void *) &funct},
	{604,	(void *) &funct},
	{'u', 	(void *) &funct},
	{'a', 	(void *) &funct},
	{'s', 	(void *) &funct},
	{'f', 	(void *) &funct},
	{'v', 	(void *) &funct},
	{'l', 	(void *) &funct},
	{'S', 	(void *) &funct},
	{'F',	(void *) &funct},
	{'B',	(void *) &funct},
	{601, 	(void *) &funct},
	{602, 	(void *) &funct},
	{603, 	(void *) &funct},
	{'w', 	(void *) &funct},
	{'m', 	(void *) &funct},
	{'v', 	(void *) &funct},
	{'d', 	(void *) &funct},
	{'D', 	(void *) &funct},
	{0, 	(void *) &funct}
};
static struct functionTable functions[] = {
	{'f', 	(void *) &funct},
	{'v', 	(void *) &funct},
	{'p', 	(void *) &play},
	{'P', 	(void *) &pause},
	{'n',	(void *) &next},
	{'b', 	(void *) &previous},
	{600, 	(void *) &stop},
	{'c',	(void *) &clear},
	{'r', 	(void *) &random_kpd},
	{'t',	(void *) &shuffle},
	{604,	(void *) &shuffle_range},
	{'u', 	(void *) &update},
	{'a', 	(void *) &add},
	{'s', 	(void *) &search_util},
	{'f', 	(void *) &funct},
	{'v', 	(void *) &funct},
	{'l', 	(void *) &list},
	{'S', 	(void *) &seek},
	{'F',	(void *) &forward},
	{'B',	(void *) &backward},
	{601, 	(void *) &consume},
	{602, 	(void *) &repeat},
	{603, 	(void *) &single},
	{'w', 	(void *) &swap},
	{'m', 	(void *) &move},
	{'v', 	(void *) &output_enable},
	{'d', 	(void *) &delete},
	{'D', 	(void *) &delete_range},
	{0, 	(void *) &funct}
};    



static int
parse_opt_filters (int k, char *arg, struct argp_state *state)
{
	char **args = NULL;
	int pos, n = 0, i;
	char *longKey = NULL;

	/*
	 * state is a struct that cointains argc and argv
	 */
	/*
	 * now find the key inside the option array, then iterate until you find the option or short option in argv
	 */
	i = 0;
	while (k != 0 && options[i].key != 0 && options[i].key != k) {
		// needed to find the long_option related to key
		++i;
	}
	if (options[i].key != 0) {
		longKey = strdup (options[i].name);
		 /*iterate argv till you find arg*/
		 /*arg is the first argument*/
		 /*iterate till you find the next option and save the arguments meanwhile*/
		for (i = 0; i < state->argc; ++i) {
			if ((strlen (state->argv[i]) > 2 && strcmp (longKey, state->argv[i] + 2) == 0)
					|| (strlen (state->argv[i]) > 1 && state->argv[i][1] == k)) {

				break;
				// found!
			}
		}
		// now count the arguments so that you can allocate args
		pos = i + 1;
		for (i = pos; i < state->argc; ++i)  {
			if (state->argv[i][0] == '-') {
				// it maybe an option (starts with '-') so stop here
				break;
			}
		}
		n = i - pos;
		args = malloc (n * sizeof (char *));
		for (i = pos; i < state->argc; ++i) {
			// now allocate arguments into arg
			if (state->argv[i][0] == '-') {
				break;
			}
			args[i - pos] = strdup (state->argv[i]);
		}
	}
	if (longKey != NULL) {
		free (longKey);
	}
	// now you have args and n

	/* now iterate function table till you find the key and
	 * call the related function
	 */
	i = 0;
	while (filters[i].key != 0 && filters[i].key != k) {
		i++;
	}
	if (filters[i].key != 0){
		filters[i].functionPtr (args, n);
	}
	// else the key was not found

	if (args != NULL) {
		for (i = 0; i < n; ++i) {
			free (args[i]);
		}
		free (args);
	}
	return 0;
}


static int
parse_opt (int k, char *arg, struct argp_state *state)
{
	char **args = NULL;
	int pos, n = 0, i;
	char *longKey = NULL;

	/*
	 * state is a struct that cointains argc and argv
	 */
	/*
	 * now find the key inside the option array, then iterate until you find the option or short option in argv
	 */
	i = 0;
	while (k != 0 && options[i].key != 0 && options[i].key != k) {
		// needed to find the long_option related to key
		++i;
	}
	if (options[i].key != 0) {
		longKey = strdup (options[i].name);
		 /*iterate argv till you find arg*/
		 /*arg is the first argument*/
		 /*iterate till you find the next option and save the arguments meanwhile*/
		for (i = 0; i < state->argc; ++i) {
			if ((strlen (state->argv[i]) > 2 && strcmp (longKey, state->argv[i] + 2) == 0)
					|| (strlen (state->argv[i]) > 1 && state->argv[i][1] == k)) {

				break;
				// found!
			}
		}
		// now count the arguments so that you can allocate args
		pos = i + 1;
		for (i = pos; i < state->argc; ++i)  {
			if (state->argv[i][0] == '-') {
				// it maybe an option (starts with '-') so stop here
				break;
			}
		}
		n = i - pos;
		args = malloc (n * sizeof (char *));
		for (i = pos; i < state->argc; ++i) {
			// now allocate arguments into arg
			if (state->argv[i][0] == '-') {
				break;
			}
			args[i - pos] = strdup (state->argv[i]);
		}
	}
	if (longKey != NULL) {
		free (longKey);
	}
	// now you have args and n

	/* now iterate function table till you find the key and
	 * call the related function
	 */
	i = 0;
	while (functions[i].key != 0 && functions[i].key != k) {
		i++;
	}
	if (functions[i].key != 0){
		functions[i].functionPtr (args, n);
	}
	// else the key was not found

	if (args != NULL) {
		for (i = 0; i < n; ++i) {
			free (args[i]);
		}
		free (args);
	}
	return 0;
}


bool version ()
{
	fprintf (stdout, "kpd 0.0.1\nLicense GPLV3+ <http://gnu.org/licenses/gpl.html>.\nThis is free software, you are welcome to contribute to the development.\n");
	return true;
}


int main (int argc, char *argv[])
{
	int ret = EXIT_SUCCESS;
	
	import_var_from_settings (); // import DBlocation, host, port to util.h
	/*struct argp argpHostPort = {optionsHOSTandPORT, parse_opt_host_port, 0, 0};*/
	/*argp_parse (&argpHostPort, argc, argv, 0, 0, 0);*/

	// first parse the filters if any	
	struct argp argpFilters = {options, parse_opt_filters, 0, 0};
	argp_parse (&argpFilters, argc, argv, ARGP_IN_ORDER |ARGP_NO_HELP |  ARGP_NO_EXIT, 0, 0);
	/*process_cli (argc, argv, hostANDport, 2, 0, 0);  // check if host and port is issued and use them, last zero because you don't want to check the sanity of argv now*/

	/*[>check if no arguments -> display current status and exit<]*/
	if(argc == 1){
		print_current_status(get_current_status ());
		exit(EXIT_SUCCESS);
	}
	/*} else {*/
		/*if (!process_cli (argc, argv, filters, NOPTIONS, 0, 0)) // filters strings must be parsed at the begin so when search is issued has the filter Strings*/
			/*ret = 64;*/

		/*// from this point on the remaining args are parsed in the order they appear in argv*/
		/*if (!process_cli (argc, argv, functions, NOPTIONS, 1, 1))*/
			/*ret =  64;*/
	/*}*/

	struct argp argp = {options, parse_opt, 0, 0};
	return argp_parse (&argp, argc, argv, ARGP_IN_ORDER, 0, 0);
	
	destroy_search_results (); // filtersStrings as well, if any
	free_var_from_settings ();

	if (ret == 1) {
		ret = 64; // EX_USAGE	64	/* command line usage error */
	}
	return ret;
}
