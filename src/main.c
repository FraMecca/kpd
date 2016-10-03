#include <stdio.h> // printf
#include <getopt.h> // long_option struct
#include"parse_args.h" // function table struct
#include "util.h" // handle for libmpdclient
#include "kpd_search.h"

bool funct ()
{
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
	{"stop",			'S', 	(void *) &funct},
	{"clear",			'c',	(void *) &funct},
	{"random",			'r', 	(void *) &funct},
	{"shuffle",			't',	(void *) &funct},
	{"update",			'u', 	(void *) &funct},
	{"add",				'a', 	(void *) &funct},
	{"search",			's', 	(void *) &funct},
	{"list",			'l', 	(void *) &funct},
	{"seek",			'k', 	(void *) &funct},
	{"consume",			'0', 	(void *) &funct},
	{"repeat",			'0', 	(void *) &funct},
	{"single",			'0', 	(void *) &funct},
	{"swap",			'w', 	(void *) &funct},
	{"verbose",		    'V', 	(void *) &funct},
	{"delete", 			'd', 	(void *) &funct},
	{"delete-range", 	'D', 	(void *) &funct},
	{"version",			'0',    (void *) &funct},
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
	{"stop",			'S', 	(void *) &stop},
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
	{"seek",			'k', 	(void *) &seek},
	{"consume",			'0', 	(void *) &consume},
	{"repeat",			'0', 	(void *) &repeat},
	{"single",			'0', 	(void *) &single},
	{"swap",			'w', 	(void *) &swap},
	{"move",			'm', 	(void *) &move},
	{"verbose",		    'V', 	(void *) &output_enable},
	{"delete", 			'd', 	(void *) &delete},
	{"delete-range", 	'D', 	(void *) &delete_range},
	{"version",			'0',    (void *) &funct},
};    
#define NOPTIONS 27 

int main (int argc, char *argv[])
{
	int ret = EXIT_SUCCESS;
	
	import_var_from_settings (); // import DBlocation, host, port to util.h
	process_cli (argc, argv, hostANDport, 2, 0, 0);  // check if host and port is issued and use them, last zero because you don't want to check the sanity of argv now

	/*check if no arguments -> display current status and exit*/
	if(argc == 1){
		print_current_status(get_current_status ());
		exit(EXIT_SUCCESS);
	} else {
		if (!process_cli (argc, argv, filters, NOPTIONS, 0, 0)) // filters strings must be parsed at the begin so when search is issued has the filter Strings
			ret = 64;

		// from this point on the remaining args are parsed in the order they appear in argv
		if (!process_cli (argc, argv, functions, NOPTIONS, 1, 1))
			ret =  64;
	}
	
	destroy_search_results (); // filtersStrings as well, if any
	free_var_from_settings ();

	if (ret == 1) {
		ret = 64; // EX_USAGE	64	/* command line usage error */
	}
	return ret;
}

