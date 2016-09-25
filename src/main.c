#include <stdio.h> // printf
#include <getopt.h> // long_option struct
#include"parse_args.h" // function table struct
#include "util.h" // handle for libmpdclient
#include "gc_util.h" // gc handler
#include <gc.h> // gc_init

bool funct ()
{
	return true;
}

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
	{"search",			's', 	(void *) &funct},
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
	int ret;

	/*GC_INIT ();*/
	
	// attach client to mpd server
	// should specify the host as config or as command line argument
	mpdSession = open_connection ("localhost", 6600); 
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
	return ret;
}

