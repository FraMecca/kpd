#ifndef KPD_SETTINGS_H_H
#define KPD_SETTINGS_H_H

static char defHost[] = "localhost"; // the ip of mpd
static unsigned int defPort = 6600;// the port as defined in mpd configs
static char defDBlocation[] = "/home/user/.mpd/database"; // the location of the database to be used with the custom search
#define TIMEOUT 1000 // timeout for connection, define at your own risk. Good values range between 1000 and 5000

#endif
