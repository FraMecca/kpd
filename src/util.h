#ifndef KPD_UTIL_H
#define KPD_UTIL_H
#include <stdbool.h>
#include <mpd/client.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAXFEAT 4
#define STANDARD_USAGE_ERROR(commandname) fprintf (stderr,"kpd: incorrect usage of %s\nTry 'kpd --help' for more information.\n", commandname); 
#define TIMEOUT 5000

typedef struct so {
	const char *title;
	const char *artist;
	const char *album;
	const char *uri; // name on the filesystem
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
	int elapsedTime_min;
	int elapsedTime_sec;
	int queueLenght;
	SONG* song;
} STATUS;

typedef struct q {
	SONG* song;
	struct q *next;	
} QUEUE;

struct mpd_connection *open_connection(const char *host, unsigned port);
void close_connection(struct mpd_connection *mpdConnection);
SONG* get_current_song(struct mpd_connection *mpdConnection);
char* get_current_state(struct mpd_status* mpdStatus);
STATUS* get_current_status(struct mpd_connection *mpdConnection);
void print_current_status(STATUS* status);
QUEUE* get_current_playlist(struct mpd_connection* mpdConnection);
bool enqueue(QUEUE* q, SONG* s);
SONG* dequeue(QUEUE* q);
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
bool output_enable(struct mpd_connection *mpdServer, char **args, int n);

#endif
