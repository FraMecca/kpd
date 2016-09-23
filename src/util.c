#include "util.h" 
#include "util2.h" 
#include "gc_util.h" // malloc, free def
#include <mpd/client.h> // libmpdclient
#include <stdio.h> // fprintf
#include <gc.h> // garbage collector
#include <stdbool.h> // true false
#include <string.h> // strcmp

#define STANDARD_USAGE_ERROR(commandname) fprintf (stderr,"kpd: incorrect usage of %s\nTry 'kpd --help' for more information.\n", commandname); 
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
			default:
				break;
		}

		return NULL;
	}

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

/* converts a (struct mpd_song*) defined in libmpdclient
 * into a SONG* structure defined in util2.h
 * returns NULL if error
 * returns a SONG* element if successful
 */
SONG*
parse_mpd_song(struct mpd_song* mpdSong)
{
	SONG *song = NULL;
	int duration;

	song = (SONG*)malloc(sizeof(SONG));
	if(song == NULL){
		fprintf(stderr, "Memory allocation error.\n");
		return NULL;
	}
	
	song->title = mpd_song_get_tag(mpdSong, MPD_TAG_TITLE, 0);
	song->artist = mpd_song_get_tag(mpdSong, MPD_TAG_ARTIST, 0);
	song->album = mpd_song_get_tag(mpdSong, MPD_TAG_ALBUM, 0);
	duration = mpd_song_get_duration(mpdSong);;
	song->duration_min = duration/60;
	song->duration_sec = duration%60;
	song->position = mpd_song_get_pos(mpdSong);
	return song;
}

/* queries the server for the current song, inserts it into a structure
 * arguments: Connection
 * returns a NULL structure if no current song / error,
 * returns a pointer to a SONG structure if successful
 */
SONG*
get_current_song(struct mpd_connection *mpdConnection)
{
	struct mpd_song* mpdSong = NULL;

	mpdSong = mpd_run_current_song(mpdConnection);
	if(mpdSong == NULL){
		return NULL;
	}
	return parse_mpd_song(mpdSong);	
}

/* queries the server for the current state (play, pause, stop, unknown)
 * arguments: status (struct mpd_status*)
 * returns NULL if error,
 * returns a pointer to a string with the status if successful
 */
char* 
get_current_state(struct mpd_status* mpdStatus)
{
	int mpdState;
	char *state;
	mpdState = mpd_status_get_state(mpdStatus);
	if(!mpdState){
		return NULL;
	}

	switch(mpdState){
		case MPD_STATE_STOP:
			state = (char*)malloc(4*sizeof(char));
			strcpy(state, "stop");
			break;
		case MPD_STATE_PLAY:
			state = (char*)malloc(4*sizeof(char));
			strcpy(state, "play");
			break;
		case MPD_STATE_PAUSE:
			state = (char*)malloc(5*sizeof(char));
			strcpy(state, "pause");
			break;
		default:
			state = NULL;
			break;
	}
	return state;	
}

/* queries the server for the current status structure, 
 * parses it and inserts it into another structure
 * arguments: Connection
 * returns NULL if error,
 * returns a pointer to a STATUS structure if successful
 */
STATUS* 
get_current_status(struct mpd_connection *mpdConnection)
{
	struct mpd_status* mpdStatus = NULL;
	STATUS *status = NULL;
	int eltime;

	mpdStatus = mpd_run_status(mpdConnection);
	if(mpdStatus == NULL){
		fprintf(stderr, "Unable to retrieve status. Connection error.\n");
		return NULL;
	}
	status = (STATUS*)malloc(sizeof(STATUS));
	if(status == NULL){
		fprintf(stderr, "Memory allocation error.\n");
		return NULL;
	}
	status->random = mpd_status_get_random(mpdStatus);
	status->repeat = mpd_status_get_repeat(mpdStatus);
	status->single = mpd_status_get_single(mpdStatus);
	status->consume = mpd_status_get_consume(mpdStatus);
	status->crossfade = mpd_status_get_crossfade(mpdStatus);
	status->song = get_current_song(mpdConnection);
	status->state = get_current_state(mpdStatus);			
	eltime = (float)mpd_status_get_elapsed_time(mpdStatus);
	status->elapsedTime_min = eltime/60;
	status->elapsedTime_sec = eltime%60;	
	status->queueLenght = mpd_status_get_queue_length(mpdStatus);
	
	return status;	
}

/* prints a STATUS structure to stdout */
void 
print_current_status(STATUS* status)
{
	SONG* song = NULL;
	int CRflag = false;

	if(status == NULL){
		return;
	}
	song = status->song;
	
	if(song != NULL){
		if(song->title != NULL){
			fprintf(stdout, "%s - ", song->title);
		}
		if(song->artist != NULL){
			fprintf(stdout, "%s\n", song->artist);
		}
		if(song->album != NULL){
			fprintf(stdout, "%s\n", song->album);
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
		fprintf (stdout, "consume: on");
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
	return;
}

/* add a new element to the playlist queue
 * returns 1 if error
 * returns 0 if successful
 * */
int
enqueue(QUEUE *q, SONG *s)
{
	QUEUE *t;
	if((t = (QUEUE*)malloc(sizeof(QUEUE))) == NULL){
		STANDARD_USAGE_ERROR("calloc");
		return 1;
	}
	t->song = s;
	if(q->next == NULL){
		q->next = t;
		q->next->next = t;
	} else {
		t->next = q->next->next;
		q->next->next = t;
		q->next = t;
	}
	return 0;
}

/* receives the songs in the playlist, one by one
 * after the get_current_playlist function
 * returns NULL if error or empty playlist
 * returns a queue of songs if successful
 */
QUEUE* 
retrieve_songs(struct mpd_connection *mpdConnection, QUEUE *q)
{
	SONG* song = NULL;
	struct mpd_song* mpdSong = NULL;

	if((mpdSong = mpd_recv_song(mpdConnection)) == NULL){		
		return q;
	}	
		
	song = parse_mpd_song(mpdSong);
	if(enqueue(q, song) == 1){
		STANDARD_USAGE_ERROR("enqueue");
		return NULL;
	}
	
	retrieve_songs(mpdConnection, q);
	return q;
}

/* sends to the server the request for a playlist queue,
 * calls retrieve_songs and builds q
 * returns NULL if error
 * returns a playlist queue if successful
 */
QUEUE* 
get_current_playlist(struct mpd_connection* mpdConnection)
{
	QUEUE *q = NULL;	
	
	mpd_send_list_queue_meta(mpdConnection);
	
	if((q = (QUEUE*)malloc(sizeof(QUEUE))) == NULL){
		STANDARD_USAGE_ERROR("calloc");		
		return NULL;
	}
	q->next = NULL;
	retrieve_songs(mpdConnection, q);
	return q;
}

/* delete an element from the queue and save the linked song 
 * returns NULL if q empty 
 * returns SONG* element if successful
 */
SONG* 
dequeue(QUEUE* q)
{
	QUEUE *t;
	SONG* song = NULL;
	if(q->next == NULL){
		return NULL;
	}
	if(q->next != q->next->next){
		t = q->next->next;
		q->next->next = t->next;
		song = t->song;
		free(t);
	} else {
		song = q->next->song;
		free(q->next);
		q->next = NULL;
	}
	return song;
}

/* prints the playlist queue on stdout */
void 
print_current_playlist(QUEUE* q, struct mpd_connection *mpdConnection)
{
	int i = 0;	
	SONG* song = dequeue(q);

	SONG *cur = get_current_song (mpdConnection);

	{
		while(i < cur->position){
			i++;
			fprintf(stdout, "%d. %s - %s\n", i, song->artist, song->title);
			song = dequeue(q);
		}
		// now we got the current playing song,
		// will be printed bold
		{
			fprintf (stdout, "\033[31m");
			i++;
			fprintf(stdout, "%d. %s - %s\n", i, song->artist, song->title);
			song = dequeue(q);
			fprintf (stdout, "\033[0m");
		}	
		while(song != NULL){
			i++;
			fprintf(stdout, "%d. %s - %s\n", i, song->artist, song->title);
			song = dequeue(q);
		}
	}

	return;
}

bool list (struct mpd_connection *mpdSession, char **argv, int n)
{
	QUEUE* playlist = NULL;

	if (n != 0) {
		STANDARD_USAGE_ERROR ("output-enable");
		return false;
	}
	playlist = get_current_playlist(mpdSession);
	print_current_playlist(playlist, mpdSession);
	return true;
}

bool 
pause (struct mpd_connection *mpdServer, char **args, int n)
{
	if (n != 0) {
		STANDARD_USAGE_ERROR("pause");
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
		STANDARD_USAGE_ERROR ("play");
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
				status = get_current_status(mpdServer);
				print_current_status (status);
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
		STANDARD_USAGE_ERROR ("random");
		return false;
	}

	//zero argument =  toggle
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

	STANDARD_USAGE_ERROR ("random");

	return false;
}

/* in consume mode the song is removed from playlist after reproduction
 * accepts standard args
 * the user can pass as parameters: on, off, true, false, 0, 1 case insensitive
 */
bool
consume(struct mpd_connection *mpdServer, char **args, int n)
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
		status = get_current_status(mpdServer);
		return(mpd_send_consume(mpdServer,!status->consume));
	}

	//if there is an attivation char consume_kpd switch on
	if(strcasecmp(args[0],"on")==0 || strcasecmp(args[0],"True")==0 || (args[0][0]-'0')==1)
	{
		return(mpd_send_consume(mpdServer,1));
	}
	
	if(strcasecmp(args[0],"off")==0 || strcasecmp(args[0],"False")==0 || (args[0][0]-'0')==0)
   	{
		return(mpd_send_consume(mpdServer,0));
	}	

	STANDARD_USAGE_ERROR ("consume");

	return false;
}

/* in repeat mode the client repeats the song after playing it
 * this function accepts standard args
 * the user can pass as parameters: on, off, true, false, 0, 1 case insensitive
 */ 
bool
repeat(struct mpd_connection *mpdServer, char **args, int n)
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
		status = get_current_status(mpdServer);
		return(mpd_send_repeat(mpdServer,!status->repeat));
	}

	//if there is an attivation char repeat_kpd switch on
	if(strcasecmp(args[0],"on")==0 || strcasecmp(args[0],"True")==0 || (args[0][0]-'0')==1)
	{
		return(mpd_send_repeat(mpdServer,1));
	}
	
	if(strcasecmp(args[0],"off")==0 || strcasecmp(args[0],"False")==0 || (args[0][0]-'0')==0)
   	{
		return(mpd_send_repeat(mpdServer,0));
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
single(struct mpd_connection *mpdServer, char **args, int n)
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
		status = get_current_status(mpdServer);
		return(mpd_send_single(mpdServer,!status->single));
	}

	//if there is an attivation char single_kpd switch on
	if(strcasecmp(args[0],"on")==0 || strcasecmp(args[0],"True")==0 || (args[0][0]-'0')==1)
	{
		return(mpd_send_single(mpdServer,1));
	}
	
	if(strcasecmp(args[0],"off")==0 || strcasecmp(args[0],"False")==0 || (args[0][0]-'0')==0)
   	{
		return(mpd_send_single(mpdServer,0));
	}	

	STANDARD_USAGE_ERROR ("single");

	return false;
}

bool
seek(struct mpd_connection *mpdServer, char **args, int n)
{
	int num = 0, l = 0;
	char time_val;	
	unsigned final_value = 0;
	STATUS *status = NULL;
	
	status = get_current_status(mpdServer);

	//check number of arguments
	if(n>1)
	{
		fprintf(stdout,"Too many arguments!\n");
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
		
		final_value = (unsigned) (status->song->duration_sec / 100 ) * num;

		return(mpd_send_seek_pos(mpdServer, status->song->position, final_value));
	}
	else
	{
		sscanf(args[0],"%d%c", &num, &time_val);
		switch(time_val)
		{	
			case 's':
				final_value = (unsigned)  status->song->duration_sec + num;
				return(mpd_send_seek_pos(mpdServer, status->song->position, final_value));
				break;
			
			case 'm':
				num *= 60;
				final_value = (unsigned)  status->song->duration_sec + num;
				return(mpd_send_seek_pos(mpdServer, status->song->position, final_value));
				break;
			
			case 'h':
				num *= (60*60);
				final_value = (unsigned)  status->song->duration_sec + num;
				return(mpd_send_seek_pos(mpdServer, status->song->position, final_value));
				break;

			default:
				fprintf(stdout,"Command not valid\n");
				return false;
		}
	}	
	
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
