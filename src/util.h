#ifndef KPD_UTIL_H
#define KPD_UTIL_H
#include <stdbool.h>
#include <mpd/client.h>

bool play (struct mpd_connection *, char **args, int n);
bool pause (struct mpd_connection *, char **args, int n);



#endif
