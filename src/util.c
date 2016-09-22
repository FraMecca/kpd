#include <mpd/client.h> // libmpdclient
#include <stdio.h> // fprintf
#include <gc.h> // garbage collector
#include <stdbool.h> // true false
#include "util2.h" // structure STATUS
#include <string.h> // strcmp

/* SHOULD 
 * IMPLEMENT
 * TOOMANYEL ST ERROR
 */

#define STANDARD_USAGE_ERROR(commandname) fprintf (stderr,"kpd: incorrect usage of %s\nTry 'kpd --help' for more information.", commandname); 

bool 
pause (struct mpd_connection *mpdServer, char **args, int n)
{
	if (n != 0) {
		STANDARD_USAGE_ERROR(pause);
		return false;
	} else {
		return mpd_send_toggle_pause (mpdServer);
	}
}

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
	STATUS *status = NULL;

	if (n != 0) {
		pos=args[0][0] - '0';
	}

	//check args elements, must be at max 1
	if(n>=2)
	{
		fprintf(stdout,"Too many elements!\n");
		return false;
	}
	
	//zero elements, reproduce current song or the first
	if(n==0)
	{
		status = get_current_status(mpdServer);
		//if state == stop reproduce the first song
		if(strcmp(status->state,"stop")==0)
		{
			check = mpd_send_play_pos(mpdServer, 0);
		}
		else
		{	
			if (strcmp (status->state, "play")==0)
			{
				check = pause (mpdServer, NULL, 0);
			} 
			else
			{
			check = mpd_send_play (mpdServer);
			}
		}
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
next(struct mpd_connection *mpdServer, char **args, int n)
{
	if(n != 0){
		STANDARD_USAGE_ERROR("next");
	}
	return (mpd_send_next(mpdServer));
}

bool
previous(struct mpd_connection *mpdServer, char **args, int n)
{
	if(n != 0){
		STANDARD_USAGE_ERROR("previous");
	}
	
	return(mpd_send_previous(mpdServer));
}


bool
stop(struct mpd_connection *mpdServer)
{	
	return(mpd_send_stop(mpdServer));
}


bool
random_kpd(struct mpd_connection *mpdServer, char **args, int n)
{	
	STATUS *status = NULL; 	
	
	//check the number of argument
	if(n>1)
	{
		fprintf(stdout,"Too many arguments!\n");
	}

	//zero argument =  switch
	if(n==0)
	{
		status = get_current_status(mpdServer);
		return(mpd_send_random(mpdServer,!status->random));
	}

	//if there is an attivation char random_kpd switch on
	if(strcasecmp(args[0],"on")==0 || strcasecmp(args[0],"True")==0 || (args[0][0]-'0')==1)
	{
		return(mpd_send_random(mpdServer,1));
	}
	
	if(strcasecmp(args[0],"off")==0 || strcasecmp(args[0],"False")==0 || (args[0][0]-'0')==0)
   	{
		return(mpd_send_random(mpdServer,0));
	}	
	
	fprintf(stdout,"The command is not valid\n");

	return false;
}



