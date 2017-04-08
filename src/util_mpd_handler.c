#include "util.h" 
#include <mpd/client.h> // libmpdclient
/*#include <mpd/run.h> // run_check*/
#include <stdio.h> // fprintf
#include <stdbool.h> // true false
#define _GNU_SOURCE
#include <string.h> // strcmp
#include <math.h> // pow
#include "kpd_search.h"
#include "include/argparse.h"

static char *filterSt = NULL, *revFilterSt = NULL; 
static char **results = NULL;
static int resultsSize = 0;
static bool printFullNames = false;

int
print_full_names (struct argparse *self, const struct argparse_option *opt)
{	
	char *args = * (char **) opt->value;
	/* 
	 * in print functions, print full names instead of artist title and album
	 */
	if(args != NULL) {
		printFullNames = !printFullNames;
	} else if (strcasecmp(args,"on")==0 || strcasecmp(args,"True")==0 || (args[0]-'0')==1) {
			printFullNames = true;
	} else if (strcasecmp(args,"off")==0 || strcasecmp(args,"False")==0 || (args[0]-'0')==0) {
   		printFullNames = false;
	} else {
		// toggle
		printFullNames = !printFullNames;
	}
	return true;
}

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
		if (!printFullNames) {
			if(song->artist != NULL){
				fprintf(stdout, "%s - ", song->artist);
				nullFlag = true;
			}
			if(song->title != NULL){
				fprintf(stdout, "%s\n", song->title);
				nullFlag = true;
			}
			if(song->album != NULL){
				fprintf(stdout, "%s\n", song->album);
				nullFlag = true;
			} else {
				nullFlag = false;
				// even if only album field is missing, the filesystem name will be printed
			}
		}
		if (nullFlag == false || printFullNames == true) {
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
print_current_playlist(QUEUE* q) 
{
	int i = 0;	
	SONG* song = dequeue(q);

	SONG *cur = get_current_song ();
	if(cur == NULL){
		// in case there is no playing song
		while(song!= NULL){
			i++;
			if (song->artist != NULL && song->title != NULL && !printFullNames) {
				// some songs have null title or/and artist field, so the filesystem name will be used
				fprintf(stdout, "%d. %s - %s\n", i, song->artist, song->title);
			} else {
				fprintf (stdout, "%d. %s\n", i, song->uri);
			}
			free_song_st (song);
			song = dequeue(q);
		}
		return;		
	}
	while(i < cur->position){
		i++;
		if (song != NULL) {
			if ((song->artist != NULL && song->title != NULL) && !printFullNames) {
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
		if (song != NULL){  // not really needed
			char color[5] = "[0m";
			STATUS *status = get_current_status ();
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
			if (song->artist != NULL && song->title != NULL && !printFullNames) {
				fprintf(stdout, "%d. %s - %s\n", i, song->artist, song->title);
			} else {
				fprintf (stdout, "%d. %s\n", i, song->uri);
			}
			free_song_st (song);
			song = dequeue(q);
			fprintf (stdout, "\x1b[0m"); // reset ansi_escape_code
		}
	}	
	while(song != NULL){
		i++;
		if (song->artist != NULL && song->title != NULL && printFullNames) {
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

int 
list (struct argparse *self, const struct argparse_option *opt)
{
	QUEUE* playlist = NULL;

	if((playlist = get_current_playlist()) != NULL){
		print_current_playlist(playlist);
	} else {
		fprintf(stdout, "Empty playlist.\n");
	}
	destroy_queue (playlist);
	return true;
}

int 
pause (struct argparse *self, const struct argparse_option *opt)
{
	struct mpd_connection *mpdSession = NULL;
	bool f;
		mpdSession = open_connection ();
		f =  mpd_send_toggle_pause (mpdSession);
		close_connection (mpdSession);
		return f;
}

/* start play song
 * can accept zero or one arguments
 * if zero args, play current song or song zero
 * if arg, play the song specified by the number (-1 because mpd start from 0)
 * prototype is standard because it is called from parse_args
 */
int
play (struct argparse *self, const struct argparse_option *opt)
{
	/*char **args = *(char ***) opt->value;*/
	size_t n = -1;
	if (opt->value) {
		 n = * (int *) opt->value;
	}
	struct mpd_connection *mpdSession = NULL;
	bool check;	
	STATUS *status = NULL;

	//user input is without arguments,  reproduce current song or the first
	if(n == 0) {
		mpdSession = open_connection ();
		status = get_current_status(mpdSession);
		close_connection (mpdSession);
		//if state == stop reproduce the first song
		if (strcmp(status->state,"stop")==0) {
			mpdSession = open_connection ();
			check = mpd_send_play_pos(mpdSession, 0);
			close_connection (mpdSession);
		}
		else {	
			// horror
			// TODO pls
			if (strcmp (status->state, "play")==0) {
				check = pause ( NULL, 0);

			} 
			else {
				mpdSession = open_connection ();
				check = mpd_send_play (mpdSession);
				close_connection (mpdSession);
				free_status_st (status);
				status = NULL;
				mpdSession = open_connection ();
				status = get_current_status(mpdSession);
				close_connection (mpdSession);
				print_current_status (status);
			}
		}
		free_status_st (status);
		return check;
	} else { // user input contains arguments
		//reproduce the specified song
		mpdSession = open_connection ();
		check = mpd_send_play_pos(mpdSession, n - 1);
		close_connection (mpdSession);

		mpdSession = open_connection ();
		status = get_current_status(mpdSession);
		close_connection (mpdSession);
		print_current_status (status);
		free_status_st (status);

		return check;
	}
}

int
next(struct argparse *self, const struct argparse_option *opt)
{
	struct mpd_connection *mpdSession = NULL;
	STATUS *status;
	mpdSession = open_connection ();
	mpd_send_next(mpdSession);
	close_connection (mpdSession);

	mpdSession = open_connection ();
	status = get_current_status(mpdSession);
	close_connection (mpdSession);
	print_current_status (status);
	free_status_st (status);
	return true;
}

int
previous (struct argparse *self, const struct argparse_option *opt)
{
	struct mpd_connection *mpdSession = NULL;
	STATUS *status;
	mpdSession = open_connection ();
	mpd_send_previous(mpdSession);
	close_connection (mpdSession);

	mpdSession = open_connection ();
	status = get_current_status(mpdSession);
	close_connection (mpdSession);
	print_current_status (status);
	free_status_st (status);
	return true;
}


int
stop (struct argparse *self, const struct argparse_option *opt)
{	
	struct mpd_connection *mpdSession = NULL;

	mpdSession = open_connection ();
	mpd_send_stop(mpdSession);
	close_connection (mpdSession);
	return true;
}

/* a wrapper to mpd_run_delete */
bool 
delete_song(int pos)
{	
	struct mpd_connection *mpdSession = NULL;
	bool f;
	mpdSession = open_connection ();
	f = mpd_run_delete(mpdSession, pos);
	close_connection (mpdSession);
	return f;
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
int
delete_range (struct argparse *self, const struct argparse_option *opt)
{
	struct mpd_connection *mpdSession = NULL;
	int **ranges = NULL;
	char **args = * (char ***) opt->value;
	int n = count_args (args);
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
		mpdSession = open_connection ();
		if(!mpd_run_delete_range(mpdSession, ranges[i][0], ranges[i][1])){
			close_connection (mpdSession);
			return false;
		}
		close_connection (mpdSession);
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
int
delete (struct argparse *self, const struct argparse_option *opt)
{
	// TODO: for brevity i leave the old logic with arrays
	// should be integers
	int i;
	int pos;
	int *positions = NULL;
	char **args = * (char ***) opt->value;
	int n = count_args (args);

	if(n == 0){
		STANDARD_USAGE_ERROR("delete");
		return false;
	}

	if(n == 1){
		pos = convert_to_int(args[0]);
		if(!delete_song(pos - 1)){
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
		if(!delete_song(pos)){
			STANDARD_USAGE_ERROR("delete_song");
			return false;
		}
	}
	list(NULL, NULL);
	return true;
}

int
clear (struct argparse *self, const struct argparse_option *opt)
{	
	struct mpd_connection *mpdSession = NULL;
	bool f;
	mpdSession = open_connection ();
	f =  mpd_send_clear(mpdSession);
	close_connection (mpdSession);
	return f;
} 

int
random_kpd (struct argparse *self, const struct argparse_option *opt)
{	
	struct mpd_connection *mpdSession = NULL;
	STATUS *status = NULL; 	
	bool f;
	char *args = * (char **) opt->value;
	//zero argument =  toggle
	if(!args)
	{
		bool value;
		mpdSession = open_connection ();
		status = get_current_status(mpdSession);
		close_connection (mpdSession);
		value = !status->random;
		free_status_st (status);
		mpdSession = open_connection ();
		f = mpd_send_random(mpdSession, value);
		close_connection (mpdSession);
		return f;
	}

	//if there is an attivation char random_kpd switch on
	if(strcasecmp(args,"on")==0 || strcasecmp(args,"True")==0 || (args[0] -'0')==1)
	{
		mpdSession = open_connection ();
		f = mpd_send_random(mpdSession,1);
		close_connection (mpdSession);
		return f;
	}

	if(strcasecmp(args,"off")==0 || strcasecmp(args,"False")==0 || (args[0] - '0')==0)
   	{
		mpdSession = open_connection ();
		f = mpd_send_random(mpdSession,0);
		close_connection (mpdSession);
		return f;
	}	

	STANDARD_USAGE_ERROR ("random");

	return false;
}

/* in consume mode the song is removed from playlist after reproduction
 * accepts standard args
 * the user can pass as parameters: on, off, true, false, 0, 1 case insensitive
 */
int
consume (struct argparse *self, const struct argparse_option *opt)
{	
	struct mpd_connection *mpdSession = NULL;
	bool f;
	STATUS *status = NULL; 	
	char *args = * (char **) opt->value;

	//zero argument =  toggle
	if(args)
	{
		bool value;
		mpdSession = open_connection ();
		status = get_current_status(mpdSession);
		close_connection (mpdSession);
		value = !status->consume;
		free_status_st (status);
		mpdSession = open_connection ();
		f = mpd_send_consume(mpdSession, value);
		close_connection (mpdSession);
		return f;
	}

	//if there is an attivation char consume_kpd switch on
	if(strcasecmp(args,"on")==0 || strcasecmp(args,"True")==0 || (args[0] -'0')==1)
	{
		mpdSession = open_connection ();
		f = mpd_send_consume(mpdSession,1);
		close_connection (mpdSession);
		return f;
	}

	if(strcasecmp(args,"off")==0 || strcasecmp(args,"False")==0 || (args[0] -'0')==0)
   	{
		mpdSession = open_connection ();
		f = mpd_send_consume(mpdSession,0);
		close_connection (mpdSession);
		return f;
	}	

	STANDARD_USAGE_ERROR ("consume");

	return false;
}

/* in repeat mode the client repeats the song after playing it
 * this function accepts standard args
 * the user can pass as parameters: on, off, true, false, 0, 1 case insensitive
 */ 
int
repeat (struct argparse *self, const struct argparse_option *opt)
{	
	struct mpd_connection *mpdSession = NULL;
	STATUS *status = NULL; 	
	bool f;
	char *args = * (char **) opt->value;

	//zero argument =  toggle
	if(args)
	{
		bool value;
		mpdSession = open_connection ();
		status = get_current_status(mpdSession);
		close_connection (mpdSession);
		value = !status->repeat;
		free_status_st (status);
		mpdSession = open_connection ();
		f = mpd_send_repeat (mpdSession, value);
		close_connection (mpdSession);
		return f;
	}

	//if there is an attivation char repeat_kpd switch on
	if (strcasecmp(args,"on")==0 || strcasecmp(args,"True")==0 || (args[0] - '0')==1)
	{
		mpdSession = open_connection ();
		f = mpd_send_repeat(mpdSession,1);
		close_connection (mpdSession);
		return f;
	}

	if(strcasecmp(args,"off")==0 || strcasecmp(args,"False")==0 || (args-'0')==0)
   	{
		mpdSession = open_connection ();
		f = mpd_send_repeat(mpdSession,0);
		close_connection (mpdSession);
		return f;
	}	

	STANDARD_USAGE_ERROR ("repeat");

	return false;
}

/* accepts standard args,
 * toggle the single mode for mpd
 * accepts as args the standard args
 * the user can pass as parameters: on, off, true, false, 0, 1 case insensitive
 */
int
single (struct argparse *self, const struct argparse_option *opt)
{	
	struct mpd_connection *mpdSession = NULL;
	STATUS *status = NULL; 	
	bool f;
	char *args = * (char **) opt->value;

	//check the number of argument
	//zero argument =  toggle
	if (args) {
		bool value;
		mpdSession = open_connection ();
		status = get_current_status(mpdSession);
		value = !status->single;
		close_connection (mpdSession);
		free_status_st (status);
		mpdSession = open_connection ();
		f = mpd_send_single(mpdSession, value);
		close_connection (mpdSession);
	}

	//if there is an attivation char single_kpd switch on
	if(strcasecmp(args,"on")==0 || strcasecmp(args,"True")==0 || (args[0]-'0')==1) {
		mpdSession = open_connection ();
		f = mpd_send_single(mpdSession,1);
		close_connection (mpdSession);
		return f;
	}

	if(strcasecmp(args,"off")==0 || strcasecmp(args,"False")==0 || (args[0]-'0')==0)
   	{
		mpdSession = open_connection ();
		f = mpd_send_single(mpdSession,0);
		close_connection (mpdSession);
		return f;
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


//go to a specific point of the song expressed in hours (h), minutes (m), seconds (s) or percentage
int 
seek (struct argparse *self, const struct argparse_option *opt)
{
	return 0;
	/*struct mpd_connection *mpdSession = NULL;*/
	/*int num = 0, l = 0;*/
	/*char time_val='\0';	*/
	/*unsigned final_value = 0;*/
	/*bool f;*/
	/*STATUS *status = NULL;*/

	/*mpdSession = open_connection ();*/
	/*status = get_current_status(mpdSession);*/
	/*close_connection (mpdSession);*/
	
	/*//check if the song is in play o in pause*/
	/*if(strcmp(status->state,"stop")==0) */
	/*{*/
		/*STANDARD_USAGE_ERROR("No song is being played\n");*/
            /*return false;*/
	/*}*/
	
	/*char args = * (char **) opt->value;*/
	/*l = strlen(args[0]);*/

	/*//check if the number is of the type num%s/m/h*/
	/*if(args[0][l-2]=='%')*/
	/*{*/
		/*fprintf(stdout,"Command not valid\n");*/
		/*return false;*/
	/*}*/

	/*//check if the argument is %*/
	/*if(args[0][l-1]=='%')*/
	/*{*/
		/*sscanf(args[0],"%d%*c", &num);*/

		/*//check the case -num%*/
		/*if(num<0)*/
		/*{*/
			/*num *= (-1);*/
		/*}*/

		/*if(num>100)*/
		/*{*/
			/*STANDARD_USAGE_ERROR("Percentage is more than 100%\n");*/
			/*return false;*/
		/*}*/
		
		/*//calculate the total time of the song and the respective percentage*/
		/*int totalTime = status->song->duration_sec + status->song->duration_min * 60;*/
		/*final_value = (unsigned) (totalTime / 100 ) * num;*/
		/*//printf ("FIN = %d\n", final_value);*/

		/*mpdSession = open_connection ();*/
		/*f = mpd_send_seek_pos(mpdSession, status->song->position, final_value);*/
		/*close_connection (mpdSession);*/
		/*free_status_st (status);*/
		/*return f;*/
	/*}*/
	/*else*/
	/*{*/
		/*sscanf(args[0],"%d%c", &num, &time_val);*/

		/*switch(time_val)*/
		/*{*/
			/*case '\0':*/
			/*case 's':	*/
				/*if(!check_limit(status, num))*/
				/*{*/
					/*STANDARD_USAGE_ERROR("Edge not respected\n");*/
					/*return false;*/
				/*}*/
				/*mpdSession = open_connection ();*/
				/*f = mpd_send_seek_pos(mpdSession, status->song->position, (unsigned) num);*/
				/*close_connection (mpdSession);*/
				/*free_status_st (status);*/
				/*return f;*/

			/*case 'm':*/
				/*num *= 60;*/
				/*if(!check_limit(status, num))*/
				/*{*/
					/*STANDARD_USAGE_ERROR("Edge not respected\n");*/
					/*return false;*/
				/*}	*/
				/*mpdSession = open_connection ();*/
				/*f = mpd_send_seek_pos(mpdSession, status->song->position, (unsigned) num);*/
				/*free_status_st (status);*/
				/*close_connection (mpdSession);*/
				/*return f;*/

			/*case 'h':*/
				/*num *= (60*60);*/
				/*if(!check_limit(status, status->elapsedTime_sec + num))*/
				/*{*/
					/*STANDARD_USAGE_ERROR("Edge not respected\n");*/
					/*return false;*/
				/*}*/
				/*mpdSession = open_connection ();*/
				/*f = mpd_send_seek_pos(mpdSession, status->song->position, (unsigned) num);*/
				/*close_connection (mpdSession);*/
				/*free_status_st (status);*/
				/*return f;*/

			/*default:*/
				/*STANDARD_USAGE_ERROR("Command is not valid\n");*/
				/*return false;*/
		/*}*/
		
	/*}	*/
	/*return false;	*/
}

/* this function redirects stdout to /dev/null or reset stdout to original state.
 * it is used to suppress or enable output by the user and kpd
 * accepts standard arguments
 * accepts from the user: on, off, true, false, 0, 1, case insensivite
 */
int
output_enable (struct argparse *self, const struct argparse_option *opt)
{
	char *args = * (char **) opt->value;

	if(strcasecmp(args,"on")==0 || strcasecmp(args,"True")==0 || (args[0]-'0')==1) {
		freopen ("/dev/stdout/", "w", stdout);
		return true;
	} else if(strcasecmp(args,"off")==0 || strcasecmp (args,"False") == 0 || (args[0] - '0') == 0) {
		freopen ("/dev/null/", "w", stdout);
		return true;
	} else {
		// at this point, the user didn't input on or off or true or false or 0 or 1 and standard usage error should be issued
		STANDARD_USAGE_ERROR ("output-enable");
		return false;
	}
}

int 
swap (struct argparse *self, const struct argparse_option *opt)
{
	char **args = * (char ***) opt->value;
	int n = count_args (args);
	/*struct mpd_connection *mpdSession = NULL;*/
	int x, y,i=0;
	STATUS *st = get_current_status ();

	i = st->queueLenght;
	free_status_st (st);
	if (i == 0) {
		fprintf (stderr, "No songs in playlist\n");
		return false;
	}

	// control argument
	if(n != 2){
		STANDARD_USAGE_ERROR("swap");
	}

	sscanf(args[0], "%d", &x);
	sscanf(args[1], "%d", &y);
	if (x < 0 || y < 0) {
		fprintf (stderr, "numbers can't be negative\n");
		STANDARD_USAGE_ERROR ("swap");
	}
	if (x > i || y > i) {
		fprintf (stderr, "numbers can't be out of range\n");
		STANDARD_USAGE_ERROR ("swap");
	}

	struct mpd_connection *mpdSession;
	mpdSession = open_connection ();
	mpd_send_swap (mpdSession, x - 1, y - 1);
		mpd_response_finish (mpdSession);
	close_connection (mpdSession);
	return true;
}

int
move (struct argparse *self, const struct argparse_option *opt)
{
	char **args = * (char ***) opt->value;
	int n = count_args (args);
	/*struct mpd_connection *mpdSession = NULL;*/
	int x, y, i=0;
	STATUS *st = get_current_status ();

	i = st->queueLenght;
	free_status_st (st);
	if (i == 0) {
		fprintf (stderr, "No songs in playlist\n");
		return false;
	}

	// control argument
	if(n != 2){
		STANDARD_USAGE_ERROR("move");
	}

	sscanf(args[0], "%d", &x);
	sscanf(args[1], "%d", &y);
	if (x < 0 || y < 0) {
		fprintf (stderr, "numbers can't be negative\n");
		STANDARD_USAGE_ERROR ("move");
	}
	if (x > i || y > i) {
		fprintf (stderr, "numbers can't be out of range\n");
		STANDARD_USAGE_ERROR ("move");
	}

	struct mpd_connection *mpdSession = NULL;
	mpdSession = open_connection ();
	mpd_send_move (mpdSession, x - 1, y - 1);
	mpd_response_finish (mpdSession);
	close_connection (mpdSession);
	return true;
}

int 
update (struct argparse *self, const struct argparse_option *opt)
{
	struct mpd_connection *mpdSession = NULL;
	bool f;
	mpdSession = open_connection ();
	f = (mpd_run_update (mpdSession, NULL) > 0);
	close_connection (mpdSession);
	return f;
}


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

int
search_util (struct argparse *self, const struct argparse_option *opt)
{
	char *arg = * (char **) opt->value;
	/*printf ("%s\n%s\n", filterSt, revFilterSt);*/
	results = search_handler (arg, &resultsSize, _DBlocation, filterSt, revFilterSt);

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
filter_helper (char **args, int n)
{
	/*
	 * this function:
	 * 1. checks the correctness of the filter arguments and count args size
	 * 2. if 1 cleared, parse the filterString
	 */
	int i, size = 0;
	char sp[] = " ";
	if (n == 0) {
		STANDARD_USAGE_ERROR ("filter");
		return false;
	}

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
vfilter_helper (char **args, int n)
{
	/*
	 * this function:
	 * 1. checks the correctness of the filter arguments and count args size
	 * 2. if 1 cleared, parse the filterString
	 */
	int i, size = 0;
	char sp[] = " ";

	if (n == 0) {
		STANDARD_USAGE_ERROR ("vfilter");
		return false;
	}

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

int
add (struct argparse *self, const struct argparse_option *opt)
{
	struct mpd_connection *mpdSession = NULL;
	int i = 0;

	for (i = 0; i < resultsSize; ++i) {
		mpdSession = open_connection ();
		// send to mpd the uri of a song to be added to the playlist
		mpd_send_add (mpdSession, results[i]);
		mpd_response_finish (mpdSession);
		/*sometimes the connection decades, so better open a new one*/
		close_connection (mpdSession);
		}
	return true;
}

int 
shuffle (struct argparse *self, const struct argparse_option *opt)
{
	struct mpd_connection *mpdSession = NULL;
	bool f;
	mpdSession = open_connection ();
	f= mpd_run_shuffle (mpdSession);
	close_connection (mpdSession);
	return f;
}

int 
shuffle_range (struct argparse *self, const struct argparse_option *opt)
{
	/*struct mpd_connection *mpdSession = NULL;*/
	int x, y, i = 0;
	STATUS *st = get_current_status ();

	i = st->queueLenght;
	free_status_st (st);
	if (i == 0) {
		fprintf (stderr, "No songs in playlist\n");
		return false;
	}

	if (! *(char ***)opt->value) STANDARD_USAGE_ERROR ("shuffle-range");
	char **args = *(char ***) opt->value;
	// control argument
	if(count_args (args) != 2){
		STANDARD_USAGE_ERROR("shuffle-range");
	}

	sscanf(args[0], "%d", &x);
	sscanf(args[1], "%d", &y);
	if (x < 0 || y < 0) {
		fprintf (stderr, "numbers can't be negative\n");
		STANDARD_USAGE_ERROR ("shuffle-range");
	}
	if (x > i || y > i) {
		fprintf (stderr, "numbers can't be out of range\n");
		STANDARD_USAGE_ERROR ("shuffle-range");
	}

	struct mpd_connection *mpdSession;
	mpdSession = open_connection ();
	mpd_send_shuffle_range (mpdSession, x - 1, y - 1);
	mpd_response_finish (mpdSession);
	close_connection (mpdSession);
	return true;
}
