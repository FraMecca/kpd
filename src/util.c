#include <mpd/client.h> // libmpdclient
#include <stdio.h> // fprintf
#include <gc.h> // garbage collector
#include <stdbool.h> // true false

/* start play song
 * can accept zero or one arguments
 * if zero args, play current song or song zero
 * if arg, play the song specified by the number (-1 because mpd start from 0)
 * prototype is standard because it is called from parse_args
 */
bool
play (struct mpd_connection *mpdServer, char **args, int n)
{
	int i;

	for(i=0; i<n; i++){
		puts(args[i]);
		printf ("%d\n", n);
		
		
	}
	

	
	
	
	return true;
}




