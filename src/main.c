#include <stdio.h> // printf
#include <getopt.h> // long_option struct
#include"parse_args.h" // function table struct
#include "util.h" // handle for libmpdclient
#include "gc_util.h" // gc handler
#include <gc.h> // gc_init
/*#include <stdlib.h> // free*/

bool funct ()
{
	return true;
}

static struct option long_options[] = {    // args neeed:
	{"help",			no_argument,       0, 'h'}, // 0
	{"play",			optional_argument, 0, 'p'}, // 0+
	{"pause",			no_argument,	   0, 'P'}, // 2+
	{"next",			no_argument,       0, 'n'}, // 1+
	{"previous",		no_argument,       0, 'b'}, // 0
	{"prev",			no_argument,       0, '0'}, // 0
    {"stop",			no_argument,       0, 's'}, // 1+	
	{"random",			optional_argument, 0, 'r'}, // 1+
	{"shuffle", 	    no_argument,       0, 'S'},
	{"update",			no_argument,       0, 'u'}, // 1+
	{"add",				required_argument, 0, 'a'}, // 1+
	{"search",			required_argument, 0, 's'}, // 1+
	{"filter",			required_argument, 0, 'f'}, // 1+
	{"v-filter",		required_argument, 0, 'v'}, // 1+
	{"list",			no_argument, 	   0, 'l'}, // 1+
	{"seek",			required_argument, 0, '0'}, // 1+
	{"consume",			no_argument,	   0, '0'}, // 0+
	{"repeat",			no_argument,	   0, '0'}, // 0+
	{"single",			no_argument,	   0, '0'}, // 1+
	{"swap",			required_argument, 0, 'w'}, // 2+
	{"format",			required_argument, 0, 'f'}, // 1+
	{"output-enable",	required_argument, 0, '0'}, // 1+
	{"clear", 			optional_argument, 0, 'c'}, // 0+
	{"delete", 			required_argument, 0, 'd'}, // 1+
	{"delete_range", 	required_argument, 0, 'D'},// 2+
};

static functionTable functions[] = {
	{"help",			'h',	(void *) &funct},
	{"play",			'p', 	(void *) &play},
	{"pause",			'P', 	(void *) &pause},
	{"next",			'n',	(void *) &next},
	{"previous",		'b', 	(void *) &previous},
	{"prev",			'b', 	(void *) &previous},
	{"stop",			'S', 	(void *) &stop},
	{"random",			'r', 	(void *) &random_kpd},
	{"shuffle",			's',	(void *) &funct},
	{"update",			'u', 	(void *) &funct},
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
	{"clear", 			'c', 	(void *) &clear},
	{"delete", 			'd', 	(void *) &delete},
	{"delete_range", 	'D', 	(void *) &delete_range},
};    
#define NOPTIONS 25 

int main (int argc, char *argv[])
{
	STATUS* currentStatus = NULL;
	struct mpd_connection *mpdSession = NULL;

	GC_INIT ();
	
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
//		exit(EXIT_SUCCESS);
	}
	
	process_cli (argc, argv, long_options, functions, NOPTIONS, mpdSession, 1);

	return 0;
}

