#include "util2.h"
#include "util.h"

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
	song->position = 1+mpd_song_get_pos(mpdSong);
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
		fprintf(stdout, "#%d/%d\t", song->position, status->queueLenght);
		fprintf(stdout, "%d:%.2d/%d:%.2d\n", status->elapsedTime_min, status->elapsedTime_sec, song->duration_min, song->duration_sec);
	}
	if(status->random){
		fprintf(stdout, "random: on ");
	}
	if(status->repeat){
		fprintf(stdout, "repeat: on ");
	}
	if(status->single){
		fprintf(stdout, "single: on ");
	}
	if(status->crossfade){
		fprintf(stdout, "crossfade: on ");
	}
	fprintf(stdout, "\n");
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
print_current_playlist(QUEUE* q)
{
	int i = 0;	
	SONG* song = dequeue(q);

	while(song != NULL){
		i++;
		fprintf(stdout, "%d. %s - %s\n", i, song->artist, song->title);
		song = dequeue(q);
	}
	return;
}












