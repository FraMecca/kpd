import std.string;
import std.exception;
import std.conv;

import std.stdio; // TODO: remove

class MPDException : Exception
{
	this(mpd_error err, string file = __FILE__, size_t line = __LINE__)
	{
		string msg = "LIBMPDCLIENT: " ~ err.to!string;
		super(msg, file, line);
    }

	this(string msg, string file = __FILE__, size_t line = __LINE__)
	{
		super(msg, file, line);
    }
}

struct MPDConnection
{
	private:
	struct Connection
	{
		mpd_connection* c = null;
		this(string host, short port, uint timeout)
		{
			this.c = mpd_connection_new(host.toStringz, port, timeout);
			auto err = mpd_connection_get_error(c);
			enforce(err == 0, new MPDException("Connection failed"));
		}
		~this()
		{
			if (c != null) mpd_connection_free(c);
		}
	}

	struct Song
	{
		string title;
    	string artist;
    	string album;
    	string uri; // name on the filesystem
    	uint duration_min;
    	uint duration_sec;
    	uint position;

    	this(ref Connection conn, int pos) {
			mpd_song* song = mpd_run_current_song(conn.c);
			static foreach(string ltag; ["title","artist","album"]) {
				mixin("this."~ltag~"= mpd_song_get_tag(song,mpd_tag_type.MPD_TAG_"~ltag.toUpper~",0);");
			}
    	}	
	}

	struct Status
	{
		mpd_state state;
		bool random;                                             
		bool consume;
		bool repeat;
		bool single;
		bool crossfade;                                          
		bool update;                                             
		int elapsedTimeMin;                                     
		int elapsedTimeSec;
		int queueLenght;

		this(ref Connection conn)
		{
			mpd_status *status = mpd_run_status(conn.c);
			auto eltime = mpd_status_get_elapsed_time(status).to!float;
			random = mpd_status_get_random(status);
    		repeat = mpd_status_get_repeat(status);
    		single = mpd_status_get_single(status);
    		consume = mpd_status_get_consume(status);
    		update = mpd_status_get_update_id(status); 
    		crossfade = mpd_status_get_crossfade(status);
            state = mpd_status_get_state(status);               
			elapsedTimeMin = (eltime / 60).to!int;
			elapsedTimeSec = (eltime % 60).to!int;        
			queueLenght = mpd_status_get_queue_length(status);
    	}
	}

	string host;
	short port;
	uint timeout;

	public:
	this(string host, short port, uint timeout = 1000)
	{
		this.host = host;
		this.port = port;
		this.timeout = timeout;
	}

	@property void pause()
	{
		auto conn = Connection(host, port, timeout);
		enforce(mpd_send_toggle_pause(conn.c), new MPDException(mpd_connection_get_error(conn.c)));
	}

	@property void play(int pos = -1)
	in{
		assert(pos >= -1);
	}
	body {
		auto conn = Connection(host, port, timeout);
		if (pos == -1 && status.state == mpd_state.MPD_STATE_PLAY) this.pause();
		else enforce(mpd_send_play_pos(conn.c, pos), new MPDException(mpd_connection_get_error(conn.c)));
	}

    @property Status status()
    {
		auto conn = Connection(host, port, timeout);
		return Status(conn);
	}

    @property Song song(int pos = -1)
    in {
    	assert(pos >= -1);
    } body {
		auto conn = Connection(host, port, timeout);
		return Song(conn, pos);
	}

	static foreach(fun; ["consume", "repeat", "random", "single"]){
		mixin("@property void " ~ fun ~ "(bool st){auto conn=Connection(host, port, timeout);
				enforce(mpd_send_" ~ fun ~ "(conn.c, st), new MPDException(mpd_connection_get_error(conn.c)));}");
	}

	static foreach(fun; ["next", "previous", "clear", "stop", "shuffle"]){
		mixin("@property void " ~ fun ~ "(){auto conn=Connection(host, port, timeout);
				enforce(mpd_send_" ~ fun ~ "(conn.c), new MPDException(mpd_connection_get_error(conn.c)));}");
	}
}

private:
extern (C):
	struct mpd_connection;
	struct mpd_status;
	struct mpd_song;
	void mpd_connection_free(mpd_connection *);
	mpd_connection* mpd_connection_new(const char*, short, uint);
	enum mpd_state 
	{
		MPD_STATE_UNKNOWN = 0, MPD_STATE_STOP = 1, MPD_STATE_PLAY = 2, MPD_STATE_PAUSE = 3
	}
	enum mpd_error 
	{
		MPD_ERROR_SUCCESS = 0, MPD_ERROR_OOM, MPD_ERROR_ARGUMENT, MPD_ERROR_STATE,
  		MPD_ERROR_TIMEOUT, MPD_ERROR_SYSTEM, MPD_ERROR_RESOLVER, MPD_ERROR_MALFORMED,
		MPD_ERROR_CLOSED, MPD_ERROR_SERVER,
	}
	enum  mpd_tag_type {
		MPD_TAG_UNKNOWN = -1, MPD_TAG_ARTIST, MPD_TAG_ALBUM, MPD_TAG_ALBUM_ARTIST,
		MPD_TAG_TITLE, MPD_TAG_TRACK, MPD_TAG_NAME, MPD_TAG_GENRE,
		MPD_TAG_DATE, MPD_TAG_COMPOSER, MPD_TAG_PERFORMER, MPD_TAG_COMMENT,
		MPD_TAG_DISC, MPD_TAG_MUSICBRAINZ_ARTISTID, MPD_TAG_MUSICBRAINZ_ALBUMID, MPD_TAG_MUSICBRAINZ_ALBUMARTISTID,
		MPD_TAG_MUSICBRAINZ_TRACKID, MPD_TAG_MUSICBRAINZ_RELEASETRACKID, MPD_TAG_ORIGINAL_DATE, MPD_TAG_ARTIST_SORT,
		MPD_TAG_ALBUM_ARTIST_SORT, MPD_TAG_ALBUM_SORT, MPD_TAG_COUNT
	}
	mpd_error mpd_connection_get_error(mpd_connection *);
    mpd_status* mpd_run_status(mpd_connection *connection);
	bool mpd_send_clear(mpd_connection *);
	bool mpd_send_stop(mpd_connection *);
	bool mpd_send_next(mpd_connection *);
	bool mpd_send_previous(mpd_connection *);
	bool mpd_send_toggle_pause(mpd_connection *);
	bool mpd_send_play_pos(mpd_connection *, uint pos);
	bool mpd_status_get_random(mpd_status*);
	bool mpd_status_get_repeat(mpd_status*);
	bool mpd_status_get_single(mpd_status*);
 	bool mpd_status_get_consume(mpd_status*);
	bool mpd_status_get_update_id(mpd_status*); 
	bool mpd_status_get_crossfade(mpd_status*);
	uint mpd_status_get_elapsed_time(mpd_status*);
	mpd_state mpd_status_get_state(mpd_status*);
	mpd_song* mpd_run_current_song(mpd_connection*);
	string mpd_song_get_tag(mpd_song*, mpd_tag_type, int);
	uint mpd_status_get_queue_length(mpd_status*);
	bool mpd_send_consume(mpd_connection*, bool);
	bool mpd_send_repeat(mpd_connection*, bool);
	bool mpd_send_random(mpd_connection*, bool);
	bool mpd_send_single(mpd_connection*, bool);
	bool mpd_send_shuffle(mpd_connection*);
	bool mpd_send_update(mpd_connection*);
