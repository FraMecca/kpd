#include "util.h" 
#include "gc_util.h" // malloc, free def
#include <mpd/client.h> // libmpdclient
#include <stdio.h> // fprintf
#include <gc.h> // garbage collector
#include <stdbool.h> // true false
#include <string.h> // strcmp
#include "settings.h"

/* This macro is a first step towards a sort of try except macro system
 * the objective of this macro is to have y = f(x) non NULL
 * but if it fails it trashes x and generates a new X using newX
 * and starts again
 */
#define TRY(x, delX, newX, y, f) do {\
		if ((y = f (x)) == NULL) {\
		delX (x); doNotClose = false;\
		}\
	} while (y == NULL && (x = newX () ) != NULL)

/* You may wonder... why this mess?
 * libmpdclient seems to corrupt the mpd_connection_struct when you issue commands in a short span of time.
 * Given that my assumptions are correct, I get a sigabort everytime I free the mpd_connection_struct if it is opened a second time
 * by the TRY macro.
 * For this reason I use a static bool to take track of that.
 * I hate myself for this
 */
static bool doNotClose = true;

bool should_close ()
{
	return doNotClose;
}
/* this is ugly */

void
import_var_from_settings ()
{
	// import variables from settings.h to be passed to other functions
	_host = strdup (defHost);
	_port = defPort;
	_DBlocation = strdup (defDBlocation);
}

bool change_host (void * p, char **args, int n)
{ 
	_host = strdup (args[0]);
	return true;
}
bool change_port (void *p, char **args, int n)
{
	_port = atoi (args[0]);
	return true;
}

/* opens a new connection to the mpd server
 * arguments: host, port (timeout defined)
 * returns a pointer to the connection structure if successful
 * returns a null pointer if error occours
 */
struct mpd_connection*
open_connection()
{
	struct mpd_connection *newConn = NULL;
	enum mpd_error connErr;
	
	newConn = mpd_connection_new(_host, _port, TIMEOUT);
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
	
	song->title = strdup (mpd_song_get_tag(mpdSong, MPD_TAG_TITLE, 0));
	song->artist = strdup (mpd_song_get_tag(mpdSong, MPD_TAG_ARTIST, 0));
	song->album = strdup (mpd_song_get_tag(mpdSong, MPD_TAG_ALBUM, 0));
	duration = mpd_song_get_duration(mpdSong);
	song->duration_min = duration/60;
	song->duration_sec = duration%60;
	song->position = mpd_song_get_pos(mpdSong);
	song->uri = strdup (mpd_song_get_uri (mpdSong));

	mpd_song_free (mpdSong);
	return song;
}

/* function to free song_struct */
void free_song_st (SONG *s)
{
	if (s!=NULL) {
		free (s->title);
		free (s->artist);
		free (s->album);
		free (s->uri);
		free (s);
	}
}

/* function to free status_struct */
void free_status_st (STATUS *s)
{
	if (s != NULL) {
		free (s->state);
		free_song_st (s->song);
		free (s);
	}
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
	
		/*mpdSong = mpd_run_current_song(mpdConnection);*/
	TRY (mpdConnection, close_connection, open_connection, mpdSong, mpd_run_current_song);
		if(mpdSong == NULL){
			fprintf (stderr, "can't get running song\n");
			return NULL;
		}
	return parse_mpd_song(mpdSong);	
}

/* queries the server for the current state (play, pause, stop, unknown)
 * arguments: status (struct mpd_status*)
 * returns NULL if error,
 * returns a pointer to a string with the status if successful
 */
static char* 
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
			state = strdup ("stop"); 
			break;
		case MPD_STATE_PLAY:
			state = strdup ("play"); 
			break;
		case MPD_STATE_PAUSE:
			state = strdup ("pause");
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

	/*mpdStatus = mpd_run_status(mpdConnection);*/
	TRY (mpdConnection, close_connection, open_connection, mpdStatus, mpd_run_status);

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
	status->update = mpd_status_get_update_id (mpdStatus); 
	status->crossfade = mpd_status_get_crossfade(mpdStatus);
	status->song = get_current_song(mpdConnection);
	status->state = get_current_state(mpdStatus);			
	eltime = (float)mpd_status_get_elapsed_time(mpdStatus);
	status->elapsedTime_min = eltime/60;
	status->elapsedTime_sec = eltime%60;	
	status->queueLenght = mpd_status_get_queue_length(mpdStatus);

	mpd_status_free (mpdStatus);	
	return status;	
}

/* add a new element to the playlist queue
 * returns 1 if error
 * returns 0 if successful
 * */
bool
enqueue(QUEUE *q, SONG *s)
{
	QUEUE *t;
	if((t = (QUEUE*)malloc(sizeof(QUEUE))) == NULL){
		STANDARD_USAGE_ERROR("calloc");
		return false;
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
	return true;
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

void destroy_queue (QUEUE *q)
{
	if (q != NULL) {
		destroy_queue (q->next);
		/*free_song_st (q->song);*/
		free (q);
	}
}

/* receives the songs in the playlist, one by one
 * after the get_current_playlist function
 * returns NULL if error or empty playlist
 * returns a queue of songs if successful
 */
static QUEUE* 
retrieve_songs(struct mpd_connection *mpdConnection, QUEUE *q)
{
	SONG* song = NULL;
	struct mpd_song* mpdSong = NULL;

	if((mpdSong = mpd_recv_song(mpdConnection)) == NULL){		
		return q;
	}	
		
	song = parse_mpd_song(mpdSong);
	if(enqueue(q, song) == 0){
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

