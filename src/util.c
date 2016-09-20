#ifndef KPD_UTIL_H
#define KPD_UTIL_H

#include <mpd/client.h> // libmpdclient
#include <gc.h> // garbage collector
#include <bool.h> // true false

/* start play song
 * can accept zero or one arguments
 * if zero args, play current song or song zero
 * if arg, play the song specified by the number (-1 because mpd start from 0)
 * prototype is standard because it is called from parse_args
 */
bool
play (struct *mpd_connection, char **args, int n)
{
	int i;

	for(i=0; i<n; i++){
		puts(args[i]);
		
		
	}
	

	
	
	
	return bool;
}

struct mpd_connection*
open_connection (const char *host, unsigned port, unsigned timeout_ms)
{
	struct mpd_connection *new_conn;
	mpd_error conn_err;

	new_conn = mpd_connection_new(host, port, timeout_ms);

}



#endif
