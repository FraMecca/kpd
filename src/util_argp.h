#include <stdio.h> // printf
#include "kpd_search.h"
#include <getopt.h> // long_option struct
#include "util.h" // handle for libmpdclient
#include <argp.h>

void funct ();
bool version ();
int parse_opt_filters (int k, char *arg, struct argp_state *state);
int parse_opt (int k, char *arg, struct argp_state *state);

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


