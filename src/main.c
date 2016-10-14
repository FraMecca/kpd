#include "util_argp.h"
#include <stdio.h> // printf

int main (int argc, char *argv[])
{
	int ret = EXIT_SUCCESS;
	
	import_var_from_settings (); // import DBlocation, host, port to util.h

	// first parse the filters if any	
	struct argp argpFilters = {options, parse_opt_filters, 0, 0};
	argp_parse (&argpFilters, argc, argv, ARGP_IN_ORDER | ARGP_SILENT | ARGP_NO_HELP | ARGP_NO_EXIT, 0, 0);

	/*[>check if no arguments -> display current status and exit<]*/
	if(argc == 1){
		print_current_status(get_current_status ());
		exit(EXIT_SUCCESS); 
	} else if (argc == 2 && strcmp(argv[1],"-V") == 0){
		version();
		exit(EXIT_SUCCESS);
	}

	struct argp argp = {options, parse_opt, 0, 0};
	return argp_parse (&argp, argc, argv, ARGP_IN_ORDER, 0, 0);
}
