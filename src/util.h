#ifndef KPD_UTIL_H
#define KPD_UTIL_H
#include <stdbool.h>
#include <mpd/client.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAXFEAT 4
#define STANDARD_USAGE_ERROR(commandname) fprintf (stderr,"kpd: incorrect usage of %s\nTry 'kpd --help' for more information.\n", commandname); 

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

char *_host;
char *_DBlocation;
unsigned int _port;

bool change_port ();
bool change_host ();
void import_var_from_settings ();
void free_var_from_settings ();

struct mpd_connection *open_connection();
void close_connection(struct mpd_connection *mpdConnection);
bool should_close (); // shame
SONG* get_current_song(struct mpd_connection *mpdConnection);
void free_song_st (SONG *s);
void free_status_st (STATUS *s);
STATUS* get_current_status(struct mpd_connection *mpdConnection);
void print_current_status(STATUS* status);
QUEUE* get_current_playlist(struct mpd_connection* mpdConnection);
bool enqueue(QUEUE* q, SONG* s);
SONG* dequeue(QUEUE* q);
void destroy_queue (QUEUE *q);
void print_current_playlist(QUEUE *q, struct mpd_connection *mpdSession);
bool list (struct mpd_connection *mpdSession, char **argv, int n);

bool play (struct mpd_connection *, char **args, int n);
bool pause (struct mpd_connection *, char **args, int n);
bool next (struct mpd_connection *, char **args, int n);
bool previous (struct mpd_connection *, char **args, int n);
bool stop(struct mpd_connection *mpdServer);
bool clear(struct mpd_connection *mpdServer);
bool delete_song(struct mpd_connection *mpdServer, int pos);
int convert_to_int(char *arg);
bool delete(struct mpd_connection *mpdServer, char **args, int n);
bool delete_range(struct mpd_connection *mpdServer, char **args, int n);
int compare_pos(const void *pos1, const void *pos2);
bool random_kpd(struct mpd_connection *mpdServer, char **args, int n);
bool consume(struct mpd_connection *mpdServer, char **args, int n);
bool single(struct mpd_connection *mpdServer, char **args, int n);
bool repeat(struct mpd_connection *mpdServer, char **args, int n);
bool seek(struct mpd_connection *mpdServer, char **args, int n);
bool update(struct mpd_connection *mpdSession, char **args, int n);
bool output_enable(struct mpd_connection *mpdServer, char **args, int n);

bool search_util (struct mpd_connection *mpdSession, char **args, int n);
void destroy_search_results ();
void print_search_results (char **results, int size);
bool filter_helper (struct mpd_connection *m, char **args, int n);
bool vfilter_helper (struct mpd_connection *m, char **args, int n);

#endif
