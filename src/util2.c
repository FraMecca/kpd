#include "util2.h"
#include <mpd/connection.h>
#include <mpd/client.h>

#define TIMEOUT 5000

/* opens a new connection to the mpd server
 * arguments: host, port (timeout defined)
 * returns a pointer to the connection structure if successful
 * returns a null pointer if error occours
 */
struct mpd_connection*
open_connection(const char *host, unsigned port)
{
	struct mpd_connection *newConn = NULL;
	enum mpd_error connErr;
	
	newConn = mpd_connection_new(host, port, TIMEOUT);
	connErr = mpd_connection_get_error(newConn);
	
	/*if error occours*/
	if(connErr != MPD_ERROR_SUCCESS){

		fprintf(stderr, "MPD connection error: ");

		/*error codes are defined in libmpdclient, file error.h*/
		switch(connErr){
			case MPD_ERROR_OOM:
				fprintf(stderr, "Out of memory\n");
				break;
		
			case MPD_ERROR_ARGUMENT:
				fprintf(stderr, "Unrecognized argument\n");
				break;	
		
			case MPD_ERROR_STATE:
				fprintf(stderr, "State\n");
				break;
		
			case MPD_ERROR_TIMEOUT:
				fprintf(stderr, "Timeout was reached\n");
				break;
		
			case MPD_ERROR_SYSTEM:
				fprintf(stderr, "System error\n");
				break;
		
			case MPD_ERROR_RESOLVER:
				fprintf(stderr, "Unknown host\n");
				break;
		
			case MPD_ERROR_MALFORMED:
				fprintf(stderr, "Malformed response received from MPD\n");
				break;
		
			case MPD_ERROR_CLOSED:
				fprintf(stderr, "Connection closed\n");
				break;
		
			case MPD_ERROR_SERVER:
				fprintf(stderr, "Server error\n");
				break;
		}

		return NULL;
	}

	fprintf(stdout, "Connection to %s successful\n", host);; 
	return newConn;
}

/* closes connection and frees all memory
 * arguments: connection (struct mpd_connection *)
 */
void
close_connection(struct mpd_connection *mpdConnection)
{
	fprintf(stdout, "Closing connection...\n");
	mpd_connection_free(mpdConnection);
}



