#ifndef KPD_UTIL2_H
#define KPD_UTIL2_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpd/client.h>
#include <string.h>

#define MAXFEAT 4

typedef struct so {
	char *title;
	char *artist;
	char *album;
	float duration;
	unsigned int position;
} SONG;

typedef struct st {
	char *state;
	bool random;
	bool repeat;
	bool single;
	bool crossfade;
	float elapsedTime;
	int queueLenght;
	SONG* song;
} STATUS;

struct mpd_connection *open_connection(const char *host, unsigned port);
void close_connection(struct mpd_connection *mpdConnection);
SONG* get_current_song(struct mpd_connection *mpdConnection);
char* get_current_state(struct mpd_status* mpdStatus);
STATUS* get_current_status(struct mpd_connection *mpdConnection);
void print_current_status(STATUS* status);

#endif
