import std.string;
import std.concurrency;
import std.exception;
import std.conv;
import std.range;

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
        bool isDummy = false; // last element, dummy

    	private void load_song(mpd_song* song) {
			assert(song !is null);
			static foreach(string ltag; ["title","artist","album"]) {
				mixin("this."~ltag~"= mpd_song_get_tag(song,mpd_tag_type.MPD_TAG_"~ltag.toUpper~",0).fromStringz.to!string;");
			}
			auto duration_tot = mpd_song_get_duration(song);
			duration_min = duration_tot / 60;
			duration_sec = duration_tot % 60;
			position = mpd_song_get_pos(song);
			uri = mpd_song_get_uri(song).fromStringz.to!string;
		}


    	this(ref Connection conn) {
			mpd_song* song = mpd_run_current_song(conn.c);
            // check to avoid assertion failure on empty playlist
            if (song !is null) load_song(song);
            else this.isDummy = true;
    	}

    	this(mpd_song *song) {
			load_song(song);
    	}

    	@property string toString(bool uris)
    	{
    		if (uris || this.title == "")
        		return this.uri;
    		else
            	return this.artist ~ " - " ~ this.album ~ " - " ~ this.title;
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

	immutable string host;
	immutable short port;
	immutable uint timeout;


	public:
	this(string host, short port, uint timeout = 1000)
	{
		this.host = host;
		this.port = port;
		this.timeout = timeout;
	}

    @property void update()
	{
		auto conn = Connection(host, port, timeout);
        // TODO avoid printing MPD_ERROR_SUCCESS on no error
        // null is path to update (means update all db)
		mpd_run_update(conn.c, null);
	}

    @property void pause()
	{
		auto conn = Connection(host, port, timeout);
		enforce(mpd_run_toggle_pause(conn.c), new MPDException(mpd_connection_get_error(conn.c)));
	}

	@property void play()
	{
		auto conn = Connection(host, port, timeout);
		if (status.state == mpd_state.MPD_STATE_STOP)
			enforce(mpd_run_play_pos(conn.c, 0), new MPDException(mpd_connection_get_error(conn.c)));
		else
			enforce(mpd_run_toggle_pause(conn.c), new MPDException(mpd_connection_get_error(conn.c)));
	}

	@property void play(ulong pos)
    {
		auto conn = Connection(host, port, timeout);
        pos = pos == 0 ? 0 : pos - 1;
		enforce(mpd_run_play_pos(conn.c, pos), new MPDException(mpd_connection_get_error(conn.c)));
	}

	@property void del(uint pos)
    {
		auto conn = Connection(host, port, timeout);
        pos = pos == 0 ? 0 : pos - 1;
		enforce(mpd_run_delete(conn.c, pos), new MPDException(mpd_connection_get_error(conn.c)));
	}

    @property Status status()
    {
		auto conn = Connection(host, port, timeout);
		return Status(conn);
	}

    @property Song song()
    {
		auto conn = Connection(host, port, timeout);
		return Song(conn);
	}

	static foreach(fun; ["consume", "repeat", "random", "single"])
    {
		mixin("@property void " ~ fun ~ "(string s){
                auto conn=Connection(host, port, timeout);
                bool st;
                try {
                    st = to!bool(s);
                } catch (ConvException e) {
                    throw new MPDException(\"--"~fun~" only accepts 'true' or 'false' as arguments.\");
                }
				enforce(mpd_run_" ~ fun ~ "(conn.c, st), new MPDException(mpd_connection_get_error(conn.c)));}");
	}

	static foreach(fun; ["next", "previous", "clear", "stop", "shuffle"])
    {
		mixin("@property void " ~ fun ~ "(){auto conn=Connection(host, port, timeout);
				enforce(mpd_run_" ~ fun ~ "(conn.c), new MPDException(mpd_connection_get_error(conn.c)));}");
	}

    @property void add(string uri)
    {
		auto conn = Connection(host, port, timeout);
        if(!mpd_run_add(conn.c, uri.toStringz)) {
            throw new MPDException(mpd_connection_get_error(conn.c));
        }
    }

    @property string statusString()
    {
    	auto st = this.status;
    	auto s = this.song;
    	string ret;
        if (s.isDummy) return "";

    	if(s.title != "") {
            ret = s.artist ~ " - " ~ s.title ~ "\n" ~ s.album;
        } else {
        	ret ~= s.uri;
        }
        ret ~= "\n";
		switch(st.state){
			case mpd_state.MPD_STATE_PLAY:
				ret ~= "(play)";
				break;
			case mpd_state.MPD_STATE_PAUSE:
				ret ~= "(pause)";
				break;
			case mpd_state.MPD_STATE_STOP:
				ret ~= "(stop)";
				break;
			default:
				assert(false);
		}

		ret ~= " #" ~ (s.position+1).to!string ~ "/" ~ st.queueLenght.to!string;
		ret ~= "\t" ~ st.elapsedTimeMin.to!string ~ ":" ~ st.elapsedTimeSec.to!string ~ "/" ~ s.duration_min.to!string ~ ":" ~ s.duration_sec.to!string;
		auto last = "";
		static foreach(r; ["random", "consume", "repeat", "single", "crossfade"]){
			mixin("if(st."~r~") last ~= \""~r~":on \";");
		}
		if(last != "") ret ~= "\n" ~ last;

        return ret;
    }

    @property Song get_song_by_pos(ulong pos)
    {
		auto conn = Connection(host, port, timeout);
		return Song(mpd_run_get_queue_song_pos(conn.c, pos));
	}


	@property Generator!Song playlist()
    {
		return new Generator!Song(
				{
    			auto st = this.status;
                auto len = status.queueLenght;
                if(!len) return;
    			foreach(i; iota(len)){
    				auto song = get_song_by_pos(i);
    				yield(song);
    			}
    		}
    	);
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
	bool mpd_run_clear(mpd_connection *);
	bool mpd_run_stop(mpd_connection *);
	bool mpd_run_next(mpd_connection *);
	bool mpd_run_previous(mpd_connection *);
	bool mpd_run_toggle_pause(mpd_connection *);
	bool mpd_run_play_pos(mpd_connection *, ulong pos);
	bool mpd_run_delete(mpd_connection *, uint pos);
	bool mpd_status_get_random(mpd_status*);
	bool mpd_status_get_repeat(mpd_status*);
	bool mpd_status_get_single(mpd_status*);
 	bool mpd_status_get_consume(mpd_status*);
	bool mpd_status_get_update_id(mpd_status*); 
	bool mpd_status_get_crossfade(mpd_status*);
	uint mpd_status_get_elapsed_time(mpd_status*);
	uint mpd_song_get_duration(mpd_song*);
	uint mpd_song_get_pos(mpd_song*);
	mpd_state mpd_status_get_state(mpd_status*);
	mpd_song* mpd_run_current_song(mpd_connection*);
	char* mpd_song_get_tag(mpd_song*, mpd_tag_type, int);
	uint mpd_status_get_queue_length(mpd_status*);
	bool mpd_run_consume(mpd_connection*, bool);
	bool mpd_run_repeat(mpd_connection*, bool);
	bool mpd_run_random(mpd_connection*, bool);
	bool mpd_run_single(mpd_connection*, bool);
	bool mpd_run_shuffle(mpd_connection*);
	bool mpd_run_update(mpd_connection*, const char*);
	bool mpd_run_pause(mpd_connection*);
    bool mpd_run_add(mpd_connection*, const char*);
    mpd_song* mpd_run_get_queue_song_pos(mpd_connection *, ulong);
    char* mpd_song_get_uri(mpd_song *) 	;
