#include <stdio.h> // printf
#include <gc.h> // boehm GC
#include <getopt.h> // long_option struct
#include"parse_args.h" // function table struct
#include "util.h" // handle for libmpdclient
/*#include <stdlib.h> // free*/

bool funct ()
{
	return true;
}

static struct option long_options[] = {    // args neeed:
	{"play", 		optional_argument, 0, 'p'}, // 0+
	{"pause",  		no_argument,	   0, 'P'}, // 2+
	{"next",		no_argument,       0, 'n'}, // 1+
	{"previous",	no_argument,       0, 'b'}, // 0
    {"stop",   		no_argument,       0, 's'}, // 1+	
	{"random",	    required_argument, 0, 'r'}, // 1+
	{"update",	    no_argument,       0, 'u'}, // 1+
	{"add",	        required_argument, 0, 'a'}, // 1+
	{"search",      required_argument, 0, 's'}, // 1+
	{"filter",	    required_argument, 0, 'f'}, // 1+
	{"v-filter",    required_argument, 0, 'v'}, // 1+
	{"list",        no_argument, 	   0, 'l'}, // 1+
	{"seek",	    required_argument, 0, '0'}, // 1+
	{"consume",	    required_argument, 0, '0'}, // 1+
	{"single",	    required_argument, 0, '0'}, // 1+
	{"swap",	    required_argument, 0, 'w'}, // 2+
	{"format",	    required_argument, 0, 'f'}, // 1+
	{"output",	    no_argument,       0, '0'}, // 1+
	{"no-output",   no_argument,       0, '0'}, // 1+
};

static functionTable functions[] = {
	{"play", 		'p', 	(void *) &play},
	{"pause",  		'P', 	(void *) &funct},
	{"next",	 	'n',	(void *) &funct},
	{"previous", 	'b', 	(void *) &funct},
	{"stop",     	's', 	(void *) &funct},
	{"random",	 	'r', 	(void *) &funct},
	{"update",	 	'u', 	(void *) &funct},
	{"add",	    	'a', 	(void *) &funct},
	{"search",	 	's', 	(void *) &funct},
	{"filter",	 	'f', 	(void *) &funct},
	{"v-filter", 	'v', 	(void *) &funct},
	{"playlist", 	'l', 	(void *) &funct},
	{"seek",	 	'0', 	(void *) &funct},
	{"consume", 	'0', 	(void *) &funct},
	{"single",	 	'0', 	(void *) &funct},
	{"swap",	 	'w', 	(void *) &funct},
	{"format",	 	'f', 	(void *) &funct},
	{"output",	 	'0', 	(void *) &funct},
	{"no-output", 	'0', 	(void *) &funct},
};    
#define NOPTIONS 19

int main (int argc, char **argv)
{
	struct mpd_connection *mpd = NULL;
	process_cli (argc, argv, long_options, functions, NOPTIONS, mpd, 1);
	return 0;
}