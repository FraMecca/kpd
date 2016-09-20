#ifndef KPD_UTIL2_H
#define KPD_UTIL2_H

#include <stdbool.h>
#include <stdio.h>
#include <mpd/client.h>

struct mpd_connection *open_connection(const char *host, unsigned port);
void close_connection(struct mpd_connection *mpdConnection);

#endif
