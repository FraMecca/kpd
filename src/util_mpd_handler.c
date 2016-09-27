#include "util.h" 
#include "gc_util.h" // malloc, free def
#include <mpd/client.h> // libmpdclient
#include <stdio.h> // frintf
#include <gc.h> // garbage collector
#include <stdbool.h> // true false
#define _GNU_SOURCE
#include <string.h> // strcmp
#include <math.h> // pow
#include "kpd_search.h"

static char *filterSt = NULL, *revFilterSt = NULL; 

/* prints a STATUS structure to stdout */
	void 
print_current_status(STATUS* status)
{
	SONG* song = NULL;
	bool CRflag = false, nullFlag = false;

	if(status == NULL){
		return;
	}
	song = status->song;

	if(song != NULL){
		if(song->title != NULL){
			fprintf(stdout, "%s - ", song->title);
			nullFlag = true;
		}
		if(song->artist != NULL){
			fprintf(stdout, "%s\n", song->artist);
			nullFlag = true;
		}
		if(song->album != NULL){
			fprintf(stdout, "%s\n", song->album);
			nullFlag = true;
		} else {
			nullFlag = false;
			// even if only album field is missing, the filesystem name will be printed
		}
		if (nullFlag == false) {
			// title, artist, album fields are missing, will print filesystem name
			fprintf (stdout, "%s\n", song->uri);
		}
		if(status->state != NULL){
			fprintf(stdout, "(%s)\t", status->state);
		}
		fprintf(stdout, "#%d/%d\t", 1+song->position, status->queueLenght);
		fprintf(stdout, "%d:%.2d/%d:%.2d\n", status->elapsedTime_min, status->elapsedTime_sec, song->duration_min, song->duration_sec);
	}
	if(status->random){
		fprintf(stdout, "random: on ");
		CRflag = true;
	}
	if(status->repeat){
		fprintf(stdout, "repeat: on ");
		CRflag = true;
	}
	if (status->consume) {
		fprintf (stdout, "consume: on ");
		CRflag = true;
	}
	if(status->single){
		fprintf(stdout, "single: on ");
		CRflag = true;
	}
	if(status->crossfade){
		fprintf(stdout, "crossfade: on ");
		CRflag = true;
	}
	if (CRflag == true) {
		fprintf(stdout, "\n");
	}
	if (status->update) {
		fprintf (stdout, "Updating database...\n");
	}
	return;
}

/* prints the playlist queue on stdout */
	void 
print_current_playlist(QUEUE* q, struct mpd_connection *mpdConnection)
{
	int i = 0;	
	SONG* song = dequeue(q);

	SONG *cur = get_current_song (mpdConnection);
	if(cur == NULL){
		return;		
	}
	while(i < cur->position){
		i++;
		if (song != NULL) {
			if (song->artist != NULL && song->title != NULL) {
				// some songs have null title or/and artist field, so the filesystem name will be used
				fprintf(stdout, "%d. %s - %s\n", i, song->artist, song->title);
			} else {
				fprintf (stdout, "%d. %s\n", i, song->uri);
			}
			free_song_st (song);
			song = dequeue(q);
		}
	}
	// now we got the current playing song,
	// will be printed bold
	{
		char color[5] = "[0m";
		STATUS *status = get_current_status (mpdConnection);
		if (strcmp (status->state, "play") == 0) {
			strncpy (color, "[31m", 4);
		} else {
			if (strcmp (status->state, "pause") == 0) {
				strncpy (color, "[33m", 4);
			}
		}
		free_status_st (status);

		fprintf (stdout, "\x1b%s", color);
		i++;
		if (song->artist != NULL && song->title != NULL) {
			fprintf(stdout, "%d. %s - %s\n", i, song->artist, song->title);
		} else {
			fprintf (stdout, "%d. %s\n", i, song->uri);
		}
		free_song_st (song);
		song = dequeue(q);
		fprintf (stdout, "\x1b[0m"); // reset ansi_escape_code
	}	
	while(song != NULL){
		i++;
		if (song->artist != NULL && song->title != NULL) {
			fprintf(stdout, "%d. %s - %s\n", i, song->artist, song->title);
		} else {
			fprintf (stdout, "%d. %s\n", i, song->uri);
		}
		free_song_st (song);
		song = dequeue(q);
	}
	free_song_st (cur);

	return;
}

bool list (struct mpd_connection *mpdSession, char **argv, int n)
{
	QUEUE* playlist = NULL;

	if (n != 0) {
		STANDARD_USAGE_ERROR ("output-enable");
		return false;
	}
	if((playlist = get_current_playlist(mpdSession)) != NULL){
		print_current_playlist(playlist, mpdSession);
	} else {
		fprintf(stdout, "Empty playlist.\n");
	}
	destroy_queue (playlist);
	return true;
}

	bool 
pause (struct mpd_connection *mpdSession, char **args, int n)
{
	if (n != 0) {
		STANDARD_USAGE_ERROR("pause");
		return false;
	} else {
		return mpd_send_toggle_pause (mpdSession);
	}
}

/* start play song
 * can accept zero or one arguments
 * if zero args, play current song or song zero
 * if arg, play the song specified by the number (-1 because mpd start from 0)
 * prototype is standard because it is called from parse_args
 */
	bool
play (struct mpd_connection *mpdSession, char **args, int n)
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
		STANDARD_USAGE_ERROR ("play");
		return false;
	}

	//zero elements, reproduce current song or the first
	if(n==0)
	{
		status = get_current_status(mpdSession);
		//if state == stop reproduce the first song
		if(strcmp(status->state,"stop")==0)
		{
			check = mpd_send_play_pos(mpdSession, 0);
		}
		else
		{	
			if (strcmp (status->state, "play")==0)
			{
				check = pause (mpdSession, NULL, 0);

			} 
			else
			{
				check = mpd_send_play (mpdSession);
				free_status_st (status);
				status = NULL;
				status = get_current_status(mpdSession);
				print_current_status (status);
			}
		}
		free_status_st (status);
		return check;
	} else { // n == 1
		//reproduce the specified song
		check = mpd_send_play_pos(mpdSession, pos-1);
		status = get_current_status(mpdSession);
		print_current_status (status);
		free_status_st (status);
		return check;
	}
}

	bool
next(struct mpd_connection *mpdSession, char **args, int n)
{
	if(n != 0){
		STANDARD_USAGE_ERROR("next");
	}
	return (mpd_send_next(mpdSession));
}

	bool
previous(struct mpd_connection *mpdSession, char **args, int n)
{
	if(n != 0){
		STANDARD_USAGE_ERROR("previous");
	}

	return(mpd_send_previous(mpdSession));
}


	bool
stop(struct mpd_connection *mpdSession)
{	
	return(mpd_send_stop(mpdSession));
}

/* a wrapper to mpd_run_delete */
	bool 
delete_song(struct mpd_connection *mpdSession, int pos)
{	
	return mpd_run_delete(mpdSession, pos);
}

/* the function used by qsort to compare integers */
	int 
compare_pos(const void *pos1, const void *pos2)
{
	return (pos1 - pos2);
}

/* deletes a range of songs from the playlist
 * usage: kpd -D 11 12 21 22 ...
 * where the first range is 11-12 and the second 21-22 (and so on)
 * returns false if error
 * returns true if successful
 */
	bool 
delete_range(struct mpd_connection *mpdSession, char **args, int n)
{
	int **ranges = NULL;
	int i;
	int cnt = 0;
	int n_ranges = n/2;

	ranges = (int**)malloc(n_ranges*sizeof(int*));
	if(!ranges){
		fprintf(stderr, "Memory allocation error.\n");
		return false;	
	}

	for(i=0; i<n_ranges; i++){
		ranges[i] = (int*)malloc(2*sizeof(int));
		if(!ranges[i]){
			fprintf(stderr, "Memory allocation error.\n");
			return false;	
		}
		ranges[i][0] = convert_to_int(args[cnt]) - 1;
		ranges[i][1] = convert_to_int(args[cnt+1]);
		cnt = cnt+2;
	}

	for(i=0; i<n_ranges; i++){
		if(!mpd_run_delete_range(mpdSession, ranges[i][0], ranges[i][1])){
			return false;
		}
	}
	return true;

}

/* converts a string of char to an integer
 * returns it when done
 */
	int 
convert_to_int(char *arg)
{
	int len = strlen(arg);
	int i;
	int result = 0;
	int arg_int;

	for(i=0; i<len; i++){
		arg_int = arg[i] - '0';
		result+=((pow(10, (len-i-1)))*(arg_int));
	}
	return result;
}

/* deletes one or more track(s) from the playlist
 * arguments required: at least one position 
 * returns false if error,
 * returns true if done, and prints the playlist
 */
	bool 
delete(struct mpd_connection *mpdSession, char **args, int n)
{
	int i;
	int pos;
	int *positions = NULL;

	if(n == 0){
		STANDARD_USAGE_ERROR("delete");
		return false;
	}

	if(n == 1){
		pos = convert_to_int(args[0]);
		if(!delete_song(mpdSession, pos - 1)){
			STANDARD_USAGE_ERROR("delete_song");
			return false;
		}
		return true;
	}

	/* n > 1 */
	positions = (int*)malloc(n*sizeof(int));
	if(!positions){
		fprintf(stderr, "Memory allocation error.\n");
		return false;
	}		

	for(i=0; i<n; i++){
		positions[i] = convert_to_int(args[i]);
	}
	qsort(positions, n, sizeof(int), compare_pos);

	for(i=0; i<n; i++){
		pos = positions[i] - i - 1;	
		if(!delete_song(mpdSession, pos)){
			STANDARD_USAGE_ERROR("delete_song");
			return false;
		}
	}
	list(mpdSession, args, n);
	return true;
}

	bool
clear(struct mpd_connection *mpdSession)
{	
	return mpd_send_clear(mpdSession);
} 

	bool
random_kpd(struct mpd_connection *mpdSession, char **args, int n)
{	
	STATUS *status = NULL; 	

	//check the number of argument
	if(n>1)
	{
		STANDARD_USAGE_ERROR ("random");
		return false;
	}

	//zero argument =  toggle
	if(n==0)
	{
		bool value;
		status = get_current_status(mpdSession);
		value = !status->random;
		free_status_st (status);
		return(mpd_send_random(mpdSession, value));
	}

	//if there is an attivation char random_kpd switch on
	if(strcasecmp(args[0],"on")==0 || strcasecmp(args[0],"True")==0 || (args[0][0]-'0')==1)
	{
		return(mpd_send_random(mpdSession,1));
	}

	if(strcasecmp(args[0],"off")==0 || strcasecmp(args[0],"False")==0 || (args[0][0]-'0')==0)
   	{
		return(mpd_send_random(mpdSession,0));
	}	

	STANDARD_USAGE_ERROR ("random");

	return false;
}

/* in consume mode the song is removed from playlist after reproduction
 * accepts standard args
 * the user can pass as parameters: on, off, true, false, 0, 1 case insensitive
 */
	bool
consume(struct mpd_connection *mpdSession, char **args, int n)
{	
	STATUS *status = NULL; 	

	//check the number of argument
	if(n>1)
	{
		STANDARD_USAGE_ERROR ("consume");
		return false;
	}

	//zero argument =  toggle
	if(n==0)
	{
		bool value;
		status = get_current_status(mpdSession);
		status = get_current_status(mpdSession);
		value = !status->consume;
		free_status_st (status);
		return(mpd_send_consume(mpdSession, value));
	}

	//if there is an attivation char consume_kpd switch on
	if(strcasecmp(args[0],"on")==0 || strcasecmp(args[0],"True")==0 || (args[0][0]-'0')==1)
	{
		return(mpd_send_consume(mpdSession,1));
	}

	if(strcasecmp(args[0],"off")==0 || strcasecmp(args[0],"False")==0 || (args[0][0]-'0')==0)
   	{
		return(mpd_send_consume(mpdSession,0));
	}	

	STANDARD_USAGE_ERROR ("consume");

	return false;
}

/* in repeat mode the client repeats the song after playing it
 * this function accepts standard args
 * the user can pass as parameters: on, off, true, false, 0, 1 case insensitive
 */ 
	bool
repeat(struct mpd_connection *mpdSession, char **args, int n)
{	
	STATUS *status = NULL; 	

	//check the number of argument
	if(n>1)
	{
		STANDARD_USAGE_ERROR ("repeat");
		return false;
	}

	//zero argument =  toggle
	if(n==0)
	{
		bool value;
		status = get_current_status(mpdSession);
		value = !status->repeat;
		free_status_st (status);
		return(mpd_send_repeat(mpdSession, value));
	}

	//if there is an attivation char repeat_kpd switch on
	if(strcasecmp(args[0],"on")==0 || strcasecmp(args[0],"True")==0 || (args[0][0]-'0')==1)
	{
		return(mpd_send_repeat(mpdSession,1));
	}

	if(strcasecmp(args[0],"off")==0 || strcasecmp(args[0],"False")==0 || (args[0][0]-'0')==0)
   	{
		return(mpd_send_repeat(mpdSession,0));
	}	

	STANDARD_USAGE_ERROR ("repeat");

	return false;
}

/* accepts standard args,
 * toggle the single mode for mpd
 * accepts as args the standard args
 * the user can pass as parameters: on, off, true, false, 0, 1 case insensitive
 */
	bool
single(struct mpd_connection *mpdSession, char **args, int n)
{	
	STATUS *status = NULL; 	

	//check the number of argument
	if(n>1)
	{
		STANDARD_USAGE_ERROR ("single");
		return false;
	}

	//zero argument =  toggle
	if(n==0)
	{
		bool value;
		status = get_current_status(mpdSession);
		value = !status->single;
		free_status_st (status);
		return(mpd_send_single(mpdSession, value));
	}

	//if there is an attivation char single_kpd switch on
	if(strcasecmp(args[0],"on")==0 || strcasecmp(args[0],"True")==0 || (args[0][0]-'0')==1)
	{
		return(mpd_send_single(mpdSession,1));
	}

	if(strcasecmp(args[0],"off")==0 || strcasecmp(args[0],"False")==0 || (args[0][0]-'0')==0)
   	{
		return(mpd_send_single(mpdSession,0));
	}	

	STANDARD_USAGE_ERROR ("single");

	return false;
}

//verify if the seek function goes over the edges
	bool
check_limit(STATUS *status, int final_val)
{
	if(final_val<0 && (status->elapsedTime_sec + final_val<0))
	{
		return false;
	}
	if(final_val>0 && (status->elapsedTime_sec + final_val > (status->song->duration_sec +
					status->song->duration_min * 60)))
	{
		return false;
	}

	return true;
}

	bool
seek(struct mpd_connection *mpdSession, char **args, int n)
{
	int num = 0, l = 0;
	char time_val='\0', sign='\0';	
	unsigned final_value = 0;
	STATUS *status = NULL;

	status = get_current_status(mpdSession);

	//check number of arguments
	if(n>1)
	{
		fprintf(stdout,"Too many arguments!\n");
		return false;
	}

	//check if the song is in play o in pause
	if(strcmp(status->state,"stop")==0) 
	{
		STANDARD_USAGE_ERROR("No song is in play or pause\n");
 	   	return false;
	}

	l = strlen(args[0]);

	//check if the number is of the type num%s/m/h
	if(args[0][l-2]=='%')
	{
		fprintf(stdout,"Command not valid\n");
		return false;
	}

	//check if the argument is %
	if(args[0][l-1]=='%')
	{
		sscanf(args[0],"%d%*c", &num);

		//check the case -num%
		if(num<0)
		{
			num *= (-1);
		}

		if(num>100)
		{
			STANDARD_USAGE_ERROR("Percentage is more than 100%\n");
			return false;
		}

		int totalTime = status->song->duration_sec + status->song->duration_min * 60;
		final_value = (unsigned) (totalTime / 100 ) * num;
		//printf ("FIN = %d\n", final_value);

		return(mpd_send_seek_pos(mpdSession, status->song->position, final_value));
	}
	else
	{
		sscanf(args[0],"%c%d%c", &sign, &num, &time_val);

		if(sign>='0' || sign<='9')
		{
			if(time_val=='\0')
			{
				num += ((pow(10,l-1))*(sign-'0'));
			}
			else
			{
				num += ((pow(10,l-2))*(sign-'0'));
			}

			switch(time_val)
			{
				case '\0':
				case 's':	
					if(!check_limit(status, num))
					{
						STANDARD_USAGE_ERROR("Edge not respected\n");
						return false;
					}
					return(mpd_send_seek_pos(mpdSession, status->song->position, (unsigned) num));

				case 'm':
					num *= 60;
					if(!check_limit(status, num))
					{
						STANDARD_USAGE_ERROR("Edge not respected\n");
						return false;
					}	
					return(mpd_send_seek_pos(mpdSession, status->song->position, (unsigned) num));

				case 'h':
					num *= (60*60);
					if(!check_limit(status, status->elapsedTime_sec + num))
					{
						STANDARD_USAGE_ERROR("Edge not respected\n");
						return false;
					}
					return(mpd_send_seek_pos(mpdSession, status->song->position, (unsigned) num));

				default:
					STANDARD_USAGE_ERROR("Command is not valid\n");
					return false;
			}
		}

		switch(time_val)
		{
			case '\0':	
			case 's':
				final_value = (unsigned)  status->elapsedTime_sec + num;
				//printf ("%d + %d\n", status->elapsedTime_sec + status->elapsedTime_min * 60 + num);
				//printf ("%d + %d\n", status->elapsedTime_sec + num);

				if(!check_limit(status, status->elapsedTime_sec + num))
				{
					STANDARD_USAGE_ERROR("Edge not respected\n");
					return false;
				}	

				//fprintf(stdout,"%d", final_value);
				return(mpd_send_seek_pos(mpdSession, status->song->position, final_value));
				break;

			case 'm':
				num *= 60;
				final_value = (unsigned)  status->elapsedTime_sec + num;

				if(!check_limit(status, status->elapsedTime_sec + num))
				{
					STANDARD_USAGE_ERROR("Edge not respected\n");
					return false;
				}	

				//fprintf(stdout,"%d", final_value);
				return(mpd_send_seek_pos(mpdSession, status->song->position, final_value));
				break;

			case 'h':
				num *= (60*60);
				//printf ("%d + %d\n", status->elapsedTime_sec + status->elapsedTime_min * 60 + num);
				//printf ("%d + %d\n", status->elapsedTime_sec + num);
				final_value = (unsigned)  status->elapsedTime_sec + num;

				if(!check_limit(status, status->elapsedTime_sec + num))
				{
					STANDARD_USAGE_ERROR("Edge not respected\n");
					return false;
				}	

				//fprintf(stdout,"%d", final_value);
				return(mpd_send_seek_pos(mpdSession, status->song->position, final_value));
				break;

			default:
				STANDARD_USAGE_ERROR("Command is not valid\n");
				break;
		}
	}	
	return false;	
}

/* this function redirects stdout to /dev/null or reset stdout to original state.
 * it is used to suppress or enable output by the user and kpd
 * accepts standard arguments
 * accepts from the user: on, off, true, false, 0, 1, case insensivite
 */
	bool
output_enable (struct mpd_connection *m, char **args, int n)
{
	if (n != 1) {
		STANDARD_USAGE_ERROR ("output-enable");
		return false;
	} else {
		if(strcasecmp(args[0],"on")==0 || strcasecmp(args[0],"True")==0 || (args[0][0]-'0')==1) {
			freopen ("/dev/stdout/", "w", stdout);
			return true;
		} else {
			if(strcasecmp(args[0],"off")==0 || strcasecmp(args[0],"False")==0 || (args[0][0]-'0')==0) {
				freopen ("/dev/null/", "w", stdout);
				return true;
			} else {
				// at this point, the user didn't input on or off or true or false or 0 or 1 and standard usage error should be issued
				STANDARD_USAGE_ERROR ("output-enable");
				return false;
			}
		}
	}
}

bool
swap(struct mpd_connection *mpdConnection, char **args, int n){
	int x, y, t, i=0;
	QUEUE *q, *prevx, *nextx, *prevy, *nexty, *node, *nodex, *nodey;

	// control argument
	if(n != 2){
		STANDARD_USAGE_ERROR("swap");
	}

	sscanf(args[0], "%d %d", &x, &y);

	// put the lower in x
	if(x > y){
		t = x;
		x = y;
		y = t;
	}

	q = get_current_playlist(mpdConnection);

	// take the nodes and the prevs and nexts
	for(node=q; node != NULL; node=node->next, i++){

		if(x == 0){
			prevx = NULL;
			nodex = node;
			nextx = node->next;
		}

		if(i == x-1 && x != 0){
			prevx = node;
			nodex = node->next;
			nextx = node->next->next;
		}else{
			prevx = NULL;
		}

		if(i == y-1){
			prevy = node;
			nodey = node->next;
			nexty = node->next->next;
		}

	}

	// switch the nodes
	prevx->next = nodey;
	nodey->next = nextx;

	prevy->next = nodex;
	nodex->next = nexty;

	// NEED TO PUT HERE ADD!	

	return true;
}

	bool 
update (struct mpd_connection *mpdSession, char **args, int n)
{
	if(n != 0){
		STANDARD_USAGE_ERROR("next");
	}
	return ((mpd_run_update (mpdSession, NULL) > 0));
}

static char **results = NULL;
static int resultsSize = 0;

	void
destroy_search_results ()
{
	if (filterSt != NULL) {
		free (filterSt);
	}
	if (revFilterSt != NULL) {
		free (revFilterSt);
	}
	destroy_results (results, resultsSize);
}

	bool
search_util (struct mpd_connection *mpdSession, char **args, int n)
{
	if (n != 1) {
		STANDARD_USAGE_ERROR ("search");
		return false;
	}
	// else number of arguments is correct
	/*printf ("%s\n%s\n", filterSt, revFilterSt);*/
	results = search_handler (args[0], &resultsSize, _DBlocation, filterSt, revFilterSt);

	/*	 search_handler does:
 	 *	 1. loads the db in memory
 	 *	 2. searches the key in the list that contains the db
 	 *	 3. filters the results for filterSt and revFiltersSt
 	 */
 	print_search_results (results, resultsSize);
 	return true;
}

	void
print_search_results (char **results, int size)
{
	int i;
	for (i = 0; i < size; ++i) {
		fprintf (stdout, "%s\n", results[i]);
	}
}

	static bool
check_for_type_words (char *w)
{
	// this function checks if w is: Artist, Album, Title
	if (strcmp (w, "Artist") == 0 ||
			strcmp (w, "Album") == 0 ||
			strcmp (w, "Directory") == 0 ||
			strcmp (w, "Title") == 0) {
		return true;
	} else {
		return false;
	}
}

bool
filter_helper (struct mpd_connection *m, char **args, int n)
{
	/*
	 * this function:
	 * 1. checks the correctness of the filter arguments and count args size
	 * 2. if 1 cleared, parse the filterString
	 */
	int i, size = 0;
	char sp[] = " ";

	for (i = 0; i < n; ++i) {
		size += strlen (args[i]);
		if (check_for_type_words (args[i]) && i > 0 && check_for_type_words (args[i - 1])) {
			STANDARD_USAGE_ERROR ("filter");
	 		return false; // there is something like Artist Album, so wrong usage
		}
	}
	filterSt = calloc (size + n + 1, sizeof (char)); // +n is for spaces
	// the whole filterSt situation could have been improved. Old python code was used where filterSt was a string
	strncpy (filterSt, args[0], strlen (args[0]));
	strncat (filterSt, sp, strlen (sp));
	for (i = 1; i < n; ++i) {
		strncat (filterSt, args[i], strlen (args[i]));
		strncat (filterSt, sp, strlen (sp));
	}
	filterSt[strlen(filterSt) - 1] = '\0';
	return true;
}

bool
vfilter_helper (struct mpd_connection *m, char **args, int n)
{
	/*
	 * this function:
	 * 1. checks the correctness of the filter arguments and count args size
	 * 2. if 1 cleared, parse the filterString
	 */
	int i, size = 0;
	char sp[] = " ";

	for (i = 0; i < n; ++i) {
		size += strlen (args[i]);
		if (check_for_type_words (args[i]) && i > 0 && check_for_type_words (args[i - 1])) {
			STANDARD_USAGE_ERROR ("filter");
	 		return false; // there is something like Artist Album, so wrong usage
		}
	}
	revFilterSt = calloc (size + n + 1, sizeof (char)); // +n is for spaces
	// the whole filterSt situation could have been improved. Old python code was used where filterSt was a string
	strncpy (revFilterSt, args[0], strlen (args[0]));
	strncat (revFilterSt, sp, strlen (sp));
	for (i = 1; i < n; ++i) {
		strncat (revFilterSt, args[i], strlen (args[i]));
		strncat (revFilterSt, sp, strlen (sp));
	}
	revFilterSt[strlen(revFilterSt) - 1] = '\0';
	return true;
}
