#ifndef KPD_UTIL_H
#define KPD_UTIL_H
#include <stdbool.h>
#include <mpd/client.h>

bool play (struct mpd_connection *, char **args, int n);
bool pause (struct mpd_connection *, char **args, int n);
bool next (struct mpd_connection *mpdServer);
bool previous (struct mpd_connection *mpdServer);
bool stop(struct mpd_connection *mpdServer);
bool random_kpd(struct mpd_connection *mpdServer, char **args, int n);
bool output_enable(struct mpd_connection *mpdServer, char **args, int n);

#endif
