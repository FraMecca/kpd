#include <mpd/client.h> // libmpdclient
#include <stdio.h> // fprintf
#include <gc.h> // garbage collector
#include <stdbool.h> // true false

/* SHOULD 
 * IMPLEMENT
 * TOOMANYEL ST ERROR
 */

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
	unsigned pos;
	if (n != 0) {
		pos=args[0][0] - '0';
	}

	//check args elements, must be at max 1
	if(n>=2)
	{
		fprintf(stdout,"Too many elements!\n");
		return false;
	}
	
	//zero elements, reproduce current song
	if(n==0)
	{
		check = mpd_send_play (mpdServer);
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

bool 
pause (struct mpd_connection *mpdServer, char **args, int n)
{
	if (n != 0) {
		fprintf(stdout,"Too many elements!\n");
		return false;
	} else {
		return mpd_send_toggle_pause (mpdServer);
	}
}

bool
next(struct mpd_connection *mpdServer)
{

}


