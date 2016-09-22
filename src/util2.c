#include "util2.h"

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

/* queries the server for the current song, inserts it into a structure
 * arguments: Connection
 * returns a NULL structure if no current song / error,
 * returns a pointer to a SONG structure if successful
 */
SONG*
get_current_song(struct mpd_connection *mpdConnection)
{
	struct mpd_song* mpdSong = NULL;
	SONG* song = NULL;
	int duration;

	mpdSong = mpd_run_current_song(mpdConnection);
	if(mpdSong == NULL){
		return NULL;
	}
	
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
	/*fprintf(stdout, "\n");*/
	return;
}














