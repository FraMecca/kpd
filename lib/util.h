#pragma once
#include <stdbool.h>
#include <mpd/client.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "include/argparse.h"

#define MAXFEAT 4
#define STANDARD_USAGE_ERROR(commandname) \
	do {                    \
		fprintf (stderr,"kpd: incorrect usage of %s\nTry 'kpd --help' for more information.\n", commandname); \
		exit (EXIT_FAILURE); \
	} while (0)

typedef struct so {
	char *title;
	char *artist;
	char *album;
	char *uri; // name on the filesystem
	int duration_min;
	int duration_sec;
	unsigned int position;
} SONG;

typedef struct st {
	char *state;
	bool random;
	bool consume;
	bool repeat;
	bool single;
	bool crossfade;
	bool update;
	int elapsedTime_min;
	int elapsedTime_sec;
	int queueLenght;
	SONG* song;
} STATUS;


typedef struct q {
	SONG* song;
	struct q *next;	
} QUEUE;

extern char *_host;
extern char *_DBlocation;
extern unsigned int _port;
size_t count_args(char **a);
void import_var_from_settings();
void free_var_from_settings();
bool change_host(char **args,int n);
bool change_port(char **args,int n);
struct mpd_connection *open_connection();
void close_connection(struct mpd_connection *mpdConnection);
SONG *parse_mpd_song(struct mpd_song *mpdSong);
void free_song_st(SONG *s);
void free_status_st(STATUS *s);
SONG *get_current_song();
STATUS *get_current_status();
bool enqueue(QUEUE *q,SONG *s);
SONG *dequeue(QUEUE *q);
void destroy_queue(QUEUE *q);
QUEUE *get_current_playlist();
int count_playlist_elements(QUEUE *q);
bool print_full_names(struct argparse *self,const struct argparse_option *opt);
void print_current_status(STATUS *status);
void print_current_playlist(QUEUE *q);
bool list(struct argparse *self,const struct argparse_option *opt);
bool pause(struct argparse *self,const struct argparse_option *opt);
bool play(struct argparse *self,const struct argparse_option *opt);
bool next(struct argparse *self,const struct argparse_option *opt);
bool previous(struct argparse *self,const struct argparse_option *opt);
bool stop(struct argparse *self,const struct argparse_option *opt);
bool delete_song(int pos);
int compare_pos(const void *pos1,const void *pos2);
bool delete_range(struct argparse *self,const struct argparse_option *opt);
int convert_to_int(char *arg);
bool delete(struct argparse *self,const struct argparse_option *opt);
bool clear(struct argparse *self,const struct argparse_option *opt);
bool random_kpd(struct argparse *self,const struct argparse_option *opt);
bool consume(struct argparse *self,const struct argparse_option *opt);
bool repeat(struct argparse *self,const struct argparse_option *opt);
bool single(struct argparse *self,const struct argparse_option *opt);
bool check_limit(STATUS *status,int final_val);
bool seek(struct argparse *self,const struct argparse_option *opt);
bool output_enable(struct argparse *self,const struct argparse_option *opt);
bool swap(struct argparse *self,const struct argparse_option *opt);
bool move(struct argparse *self,const struct argparse_option *opt);
bool update(struct argparse *self,const struct argparse_option *opt);
void destroy_search_results();
void print_search_results(char **results,int size);
bool filter_helper(struct argparse *self,const struct argparse_option *opt);
bool add(struct argparse *self,const struct argparse_option *opt);
bool shuffle(struct argparse *self,const struct argparse_option *opt);
bool shuffle_range(struct argparse *self,const struct argparse_option *opt);
