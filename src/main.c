#include <stdlib.h> // exit 
#include <argp.h> // argp
#include "kpd_search.h"
#include "util.h" // handle for libmpdclient
#include "include/argparse.h"

bool version ()
{
	fprintf (stdout, "kpd 0.0.1\nLicense GPLV3+ <http://gnu.org/licenses/gpl.html>.\nThis is free software, you are welcome to contribute to the development.\n");
	return true;
}

char **ar = NULL;
int in;
char *searchSt;
char *tmpSt; // "single shot" string used for function that need arguments 

struct argparse_option options[] = {
	OPT_HELP (),
	/*OPT_ARRAY ('f',		"filter",			 NULL,	   "Filter the search using multiple keywords"),*/
	/*OPT_ARRAY ('v',		"vfilter",			 NULL,	   "Same as filter but excludes"),*/
	OPT_INTEGER ('p',		"play",				 &in,	   "Play a song from the playlist",   &play ),
	OPT_BOOLEAN ('P',		"pause",			 NULL,	   "Toggle pause",                    &pause),
	OPT_BOOLEAN ('n',		"next",				 NULL,	   "play next track in playlist",     &next),
	OPT_BOOLEAN ('b',		"previous",			 NULL,	   "play previous track in playlist", &previous),
	OPT_BOOLEAN ( 0 ,		"stop",				 NULL,	   "Stop playback",                   &stop),
	OPT_BOOLEAN ('c',		"clear",			 NULL,	   "Clear playlist",                  &clear),
	OPT_BOOLEAN ('t',		"shuffle",			 NULL,	   "Shuffle track order in playlist", &shuffle),
	OPT_ARRAY   ( 0 ,		"shuffle-range",	 &tmpSt,   "Same as shuffle, for a subset",   &shuffle_range),
	OPT_BOOLEAN ('u',		"update",			 NULL,	   "Update MPD database",             &update),
	OPT_BOOLEAN ('a',		"add",				 NULL,	   "add tracks to playlist",          &add),
	OPT_STRING  ('s',		"search",			 &searchSt,"Search tracks in MPD database",   &search_util),
	OPT_BOOLEAN ('l',		"list",				 NULL,	   "Print playlist",                  &list),
	OPT_ARRAY   ('S',		"seek",				 NULL,	   "Seek [+-]track by duration or %", &seek),
	OPT_STRING  ('r',		"random",			 NULL,	   "Toggle mpd random mode",          &random_kpd),
	OPT_STRING  ( 0 ,		"consume",			 &tmpSt,   "Toggle consume mode in MPD",      &consume), 
	OPT_STRING  ( 0 ,		"repeat",			 &tmpSt,   "Toggle repeat mode in MPD",       &repeat),
	OPT_STRING  ( 0 ,		"single",			 &tmpSt,   "Toggle single mode in MPD",       &single),
	OPT_STRING  ( 0 ,		"enable-output",	 &tmpSt,   "Verbose mode",                    &output_enable),
	OPT_ARRAY   ('d',		"delete",			 &tmpSt,   "Delete a track from playlist",    &delete),
	OPT_ARRAY   ('D',		"delete-range",		 &tmpSt,   "Delete tracks from playlist",     &delete_range),
	OPT_ARRAY   ('w',		"move",				 &tmpSt,   "Move a track in the playlist",    &move),
	OPT_ARRAY   ('m',		"swap",				 &tmpSt,   "Swap two tracks in playlist",     &swap),
	OPT_STRING  ('N',		"full-names",		 NULL,	   "Print full filename of tracks",   &print_full_names),
    OPT_END (),
};

static const char *const usage [] = {
	"kpd [[--] args [--]]",
	NULL,
};

int 
main (int argc, char *argv[])
{
	int ret = EXIT_SUCCESS;
	printf ("echo search con zero args, seek con + e - \ne anche argopt e uint invece di char\n");

	// load structures into parse_args library
	import_var_from_settings (); // import DBlocation, host, port to util.h

	/*check if no arguments -> display current status and exit*/
	if(argc == 1){
		print_current_status(get_current_status ());
		exit(EXIT_SUCCESS); 
	} else if (argc == 2 && strcmp(argv[1],"-V") == 0){
		version();
		exit(EXIT_SUCCESS);
	}

	struct argparse argparse;
	argparse_init (&argparse, options, usage, 0);
	argparse_describe (&argparse, "\nKPD client for MPD\n", "");
	argparse_parse (&argparse, argc, argv);

	return ret;
}
