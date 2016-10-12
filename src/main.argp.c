#include <stdio.h> // printf
#include <getopt.h> // long_option struct
#include"parse_args.h" // function table struct
#include "util.h" // handle for libmpdclient
#include "kpd_search.h"
#include <argp.h>

void parse_ar (char *ar)
{
	return;
}

static int
parse_opt (int k, char *arg, struct argp_state *state)
{



	char **args = NULL;
	int pos, n = 0, i;

	/*
	 * state is a struct that cointains argc and argv
	 */
	/*if (arg != NULL) {*/
		/* iterate argv till you find arg
		 * arg is the first argument
		 * iterate till you find the next option and save the arguments meanwhile
		 */
		/*for (i = 0; i < state->argc; ++i) {*/
			/*if (strcmp (arg, state->argv[i]) == 0) {*/
				/*break;*/
				/*// found!*/
			/*}*/
		/*}*/
		/*// now count the arguments so that you can allocate args*/
		/*pos = i;*/
		/*for (i = pos; i < state->argc; ++i) {*/
			/*if (state->argv[i][0] == '-') {*/
				/*// it maybe an option (starts with '-') so stop here*/
				/*break;*/
			/*}*/
		/*}*/
		/*n = i - pos;*/
		/*args = malloc (n * sizeof (char *));*/
		/*for (i = pos; i < state->argc; ++i) {*/
			/*// now allocate arguments into arg*/
			/*if (state->argv[i][0] == '-') {*/
				/*break;*/
			/*}*/
			/*args[i - pos] = strdup (state->argv[i]);*/
		/*}*/
	/*}*/

	switch (k) {
		case 'p':
		printf ("args: ");
		for (i = 0; i < n; ++i) {
			printf ("%s ", args[i]);
		}
		printf ("\n");
		printf ("%s\n", arg);
		break;
	}
	return 0;
}

bool funct ()
{
	return true;
}

bool version ()
{
	fprintf (stdout, "kpd 0.0.1\nLicense GPLV3+ <http://gnu.org/licenses/gpl.html>.\nThis is free software, you are welcome to contribute to the development.\n");
	return true;
}


static functionTable hostANDport[] = {
	{"host",			'0',	(void *) &change_host},
	{"port",			'0',	(void *) &change_host},
};

static functionTable filters[] = {
	{"filter",			'f', 	(void *) &filter_helper},
	{"v-filter",		'v', 	(void *) &vfilter_helper},
	{"help",			'h',	(void *) &funct},
	{"play",			'p', 	(void *) &funct},
	{"pause",			'P', 	(void *) &funct},
	{"next",			'n',	(void *) &funct},
	{"previous",		'b', 	(void *) &funct},
	{"prev",			'0', 	(void *) &funct},
	{"stop",			'0', 	(void *) &funct},
	{"clear",			'c',	(void *) &funct},
	{"random",			'r', 	(void *) &funct},
	{"shuffle",			't',	(void *) &funct},
	{"update",			'u', 	(void *) &funct},
	{"add",				'a', 	(void *) &funct},
	{"search",			's', 	(void *) &funct},
	{"list",			'l', 	(void *) &funct},
	{"seek",			'S', 	(void *) &funct},
	{"forward",			'F',	(void *) &funct},
	{"backward",		'B',	(void *) &funct},
	{"consume",			'0', 	(void *) &funct},
	{"repeat",			'0', 	(void *) &funct},
	{"single",			'0', 	(void *) &funct},
	{"swap",			'w', 	(void *) &funct},
	{"verbose",		    'v', 	(void *) &funct},
	{"delete", 			'd', 	(void *) &funct},
	{"delete-range", 	'D', 	(void *) &funct},
	{"version",			'V',    (void *) &funct},
	{"host",			'0',	(void *) &funct},
	{"port",			'0',	(void *) &funct},
}; // if filters functiontable doesn't have all the other options, they will be parsed as part of the filterString

static functionTable functions[] = {
	{"help",			'h',	(void *) &funct},
	{"play",			'p', 	(void *) &play},
	{"pause",			'P', 	(void *) &pause},
	{"next",			'n',	(void *) &next},
	{"previous",		'b', 	(void *) &previous},
	{"prev",			'0', 	(void *) &previous},
	{"stop",			'0', 	(void *) &stop},
	{"clear",			'c',	(void *) &clear},
	{"random",			'r', 	(void *) &random_kpd},
	{"shuffle",			't',	(void *) &shuffle},
	{"shuffle-range",	'0',	(void *) &shuffle_range},
	{"update",			'u', 	(void *) &update},
	{"add",				'a', 	(void *) &add},
	{"search",			's', 	(void *) &search_util},
	{"filter",			'f', 	(void *) &funct},
	{"v-filter",		'v', 	(void *) &funct},
	{"list",			'l', 	(void *) &list},
	{"seek",			'S', 	(void *) &seek},
	{"forward",			'F',	(void *) &forward},
	{"backward",		'B',	(void *) &backward},
	{"consume",			'0', 	(void *) &consume},
	{"repeat",			'0', 	(void *) &repeat},
	{"single",			'0', 	(void *) &single},
	{"swap",			'w', 	(void *) &swap},
	{"move",			'm', 	(void *) &move},
	{"verbose",		    'v', 	(void *) &output_enable},
	{"delete", 			'd', 	(void *) &delete},
	{"delete-range", 	'D', 	(void *) &delete_range},
	{"version",			'V',    (void *) &version},
};    
#define NOPTIONS 29 

int main (int argc, char *argv[])
{
	int ret = EXIT_SUCCESS;
	
	/*import_var_from_settings (); // import DBlocation, host, port to util.h*/
	/*process_cli (argc, argv, hostANDport, 2, 0, 0);  // check if host and port is issued and use them, last zero because you don't want to check the sanity of argv now*/

	/*[>check if no arguments -> display current status and exit<]*/
	/*if(argc == 1){*/
		/*print_current_status(get_current_status ());*/
		/*exit(EXIT_SUCCESS);*/
	/*} else {*/
		/*if (!process_cli (argc, argv, filters, NOPTIONS, 0, 0)) // filters strings must be parsed at the begin so when search is issued has the filter Strings*/
			/*ret = 64;*/

		/*// from this point on the remaining args are parsed in the order they appear in argv*/
		/*if (!process_cli (argc, argv, functions, NOPTIONS, 1, 1))*/
			/*ret =  64;*/
	/*}*/

	struct argp_option options[] = {
		{"play", 'p', "NUM", 2, "show p"},
		{0}
	};
	struct argp argp = {options, parse_opt, 0, 0};
	return argp_parse (&argp, argc, argv, 0, 0, 0);
	
	destroy_search_results (); // filtersStrings as well, if any
	free_var_from_settings ();

	if (ret == 1) {
		ret = 64; // EX_USAGE	64	/* command line usage error */
	}
	return ret;
}

