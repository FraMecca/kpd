#include <stdio.h> // printf
#include <getopt.h> // long_option struct
#include"parse_args.h" // function table struct
#include "util.h" // handle for libmpdclient
#include "gc_util.h" // gc handler
#include "kpd_search.h"

bool funct ()
{
	return true;
}


static functionTable hostANDport[] = {
	{"host",			'0',	(void *) &change_host},
	{"port",			'0',	(void *) &change_host},
};

static functionTable functions[] = {
	{"help",			'h',	(void *) &funct},
	{"play",			'p', 	(void *) &play},
	{"pause",			'P', 	(void *) &pause},
	{"next",			'n',	(void *) &next},
	{"previous",		'b', 	(void *) &previous},
	{"prev",			'b', 	(void *) &previous},
	{"stop",			'S', 	(void *) &stop},
	{"clear",			'c',	(void *) &clear},
	{"random",			'r', 	(void *) &random_kpd},
	{"shuffle",			's',	(void *) &funct},
	{"update",			'u', 	(void *) &update},
	{"add",				'a', 	(void *) &funct},
	{"search",			's', 	(void *) &search_util},
	{"filter",			'f', 	(void *) &funct},
	{"v-filter",		'v', 	(void *) &funct},
	{"list",			'l', 	(void *) &list},
	{"seek",			'0', 	(void *) &seek},
	{"consume",			'0', 	(void *) &consume},
	{"repeat",			'0', 	(void *) &repeat},
	{"single",			'0', 	(void *) &single},
	{"swap",			'w', 	(void *) &funct},
	{"format",			'f', 	(void *) &funct},
	{"output-enable",   '0', 	(void *) &output_enable},
	{"delete", 			'd', 	(void *) &delete},
	{"delete-range", 	'D', 	(void *) &delete_range},
};    
#define NOPTIONS 25 

int main (int argc, char *argv[])
{
	STATUS* currentStatus = NULL;
	struct mpd_connection *mpdSession = NULL;
	int ret = 0;
	
	import_var_from_settings (); // import DBlocation, host, port to util.h
	process_cli (argc, argv, hostANDport, 2, NULL, 0); 

	// attach client to mpd server
	// should specify the host as config or as command line argument
	mpdSession = open_connection (_host, _port);  
	if (mpdSession == NULL) {
		// didn't get a connection successfully
		return 2;
	}
	
	/*check if no arguments -> display current status and exit*/
	if(argc == 1){
		currentStatus = get_current_status(mpdSession);
		print_current_status(currentStatus);
		exit(EXIT_SUCCESS);
	} else {
		ret = (process_cli (argc, argv, functions, NOPTIONS, mpdSession, 1) == 0);
	}
	if (mpdSession != NULL && should_close (mpdSession)) {
		close_connection (mpdSession);
	}
	
	free_var_from_settings ();

	return ret;
}

