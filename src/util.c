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
	bool check;	
	unsigned pos='0'-args[0][0];

	//check args elements, must be at max 1
	if(n>=2)
	{
		fprintf(stdrin,"Too many elements!\n");
		return false;
	}
	
	//zero elements, reproduce current song
	if(n==0)
	{
		check = mpd_send_current_song(mpdServer);
		return check;
	}
	
	//reproduce the specified song
	if(n==1)
	{
		check = mpd_send_play_pos(mpdServer, pos-1);
		return check;
	}	

	return true;
}




