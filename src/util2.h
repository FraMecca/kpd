#ifndef KPD_UTIL2_H
#define KPD_UTIL2_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpd/client.h>
#include <string.h>

#define MAXFEAT 4

typedef struct so {
	const char *title;
	const char *artist;
	const char *album;
	int duration_min;
	int duration_sec;
	unsigned int position;
} SONG;

typedef struct st {
	char *state;
	bool random;
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
int enqueue(QUEUE* q, SONG* s);
SONG* dequeue(QUEUE* q);
QUEUE* retrieve_songs(struct mpd_connection* mpdConnection, QUEUE* q);
QUEUE* get_current_playlist(struct mpd_connection* mpdConnection);
void print_current_playlist(QUEUE *q);
#endif
