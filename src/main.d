import std.stdio;
import std.range;
import std.string;
import std.getopt;
import std.typecons;
import std.algorithm;
import std.conv;
import std.exception;
import std.traits;
import std.file;
import std.path;
import sdlang;

import search;
import database;
import libmpdclient;

extern (C){
	int isatty(const int fd);
}

uint asUInt (string st){
    import core.stdc.stdlib : exit;
    try{
        return st.to!uint;
    }catch(Exception e){
        stderr.writeln("Error: '" ~ st ~"' can't be parsed as integer");
        exit(1);
        assert(false);
    }
}

/* TODO: some options here can be left to config file
 *  other must be moved to cli only.
 * Tipical kprc file is only tree lines long and contains:
 * host "localhost" \n port 6600 \n db "/home/user/.mpd/database"
 * Doesn't need much more and I don't care at the moment for selecting a different host on the cli
 * In this way we can remove the indirection of parsing a first structure (programArgs) 
 * Then copy programArgs to ParseArgs that contains all the values of programArgs + configfile
 * Maybe we can allow to specify a second config file
 */ 

struct ParseArgs{
    bool quiet;
    bool add;
    bool previous;
    bool clear;
    bool list;
    bool listall;
    bool next;
    bool uris; 
    bool pause;
    string consume;
    string random;
    string repeat;
    string single;
    bool stop;
    bool shuffle;
    bool update;
    bool play;
    ulong playN;
    Nullable!uint delN;
    //TODO: not implemented
    Nullable!(Tuple!(uint, uint)) delR;
    Nullable!(Tuple!(uint, uint)) shuffleR;
    Nullable!(Tuple!(uint, uint)) swapR;
    Nullable!(Tuple!(uint, uint)) move;
    string[] searchTermsR;
    string[] filterTermsR;
    string host = "localhost";
	int port = 6600;
	string dblocation = "~/.mpd/database";

    import commandr;
    this(ProgramArgs program){
        arraySep = ",";
    	auto configs = ["/etc/kpd.conf", "~/.kpdrc", "~/.config/kpd.conf"];
    	foreach(cfile; configs){
    		cfile = expandTilde(cfile);
    		if(cfile.exists){
    			sdlang.Tag root;
    			root = parseSource(readText(cfile));
    			host = root.getTagValue!string("host", host);
    			port = root.getTagValue!int("port", port);
    			dblocation = root.getTagValue!string("db");
    		}
    	}

        // now handle program from commandr
        static foreach(m; __traits(allMembers, ParseArgs)) {
                mixin("static if (is(typeof(ParseArgs."~m~") == bool)){ this."~m~" = program.flag(\""~m~"\");}");
        }

        auto lastArg = program.arg("");

        if(lastArg) this.playN = lastArg.asUInt;
        if(program.flag("del")) this.delN = lastArg.asUInt;

        this.searchTermsR = program.option("search").split(",");
        this.filterTermsR = program.option("filter").split(",");

        this.consume = program.option("consume");
        this.consume = program.option("random");
        this.consume = program.option("repeat");
        this.consume = program.option("single");

        if(program.option("shufflerange"))
            this.shuffleR = tuple(program.option("shufflerange").asUInt, lastArg.asUInt);
        if(program.option("delrange"))
            this.delR = tuple(program.option("delrange").asUInt, lastArg.asUInt);
        if(program.option("swap"))
            this.swapR = tuple(program.option("swap").asUInt, lastArg.asUInt);
        if(program.option("move"))
            this.move = tuple(program.arg("move").asUInt, lastArg.asUInt);
    }
}

auto buildCliArgs(string[] args){
    import commandr;
    auto kargs = new Program("kpd", "0.0.1")
        .summary("kpd is a client for MPD")
        .author("Francesco Gallà and Francesco Mecca")

        .add(new Flag("a", "add", "add elements to current playlist"))
        .add(new Flag("l", "list", "display current playlist"))
        .add(new Flag(null, "listall", "display whole database"))
        .add(new Flag("c", "clear", "clear current playlist"))

        .add(new Flag("b", "prev", "play previous song"))
        .add(new Flag(null, "previous", "play previous song"))
        .add(new Flag("n", "next", "play next song"))
        .add(new Flag("p", "play", "play [n] or pause mpd"))
        .add(new Flag("P", "pause", "pause mpd"))
        .add(new Flag(null, "stop", "stop mpd"))

        .add(new Flag("d", "del", "remove specified item from playlist"))
        .add(new Option(null, "delrange", "remove specified range of elements from playlist"))
        .add(new Flag("t", "shuffle", "shuffle current playlist"))
        .add(new Option(null, "shufflerange", "shuffle given range"))
        .add(new Option(null, "swap", "swap specified playlist elements"))
        .add(new Option(null, "move", "move specified playlist elements"))

        .add(new Option(null, "consume", "toggle consume")) // option because accepts only "true" or "false"
        .add(new Option(null, "random", "toggle random")) // TODO: move to flag that toggles opposite
        .add(new Option(null, "repeat", "toggle repeat")) // TODO: support on, off
        .add(new Option(null, "single", "toggle single"))

        .add(new Flag("u", "update", "send update request to mpd"))

        .add(new Flag("q", "quiet", "disable output"))
        .add(new Flag("U", "displayuri", "display uri of elements"))

        .add(new Option("s", "search", "search given comma-separated key(s)"))
        .add(new Option("f", "filter", "filter given comma-separated key(s) from search results"))

        .add(new Argument("", "Optional argument (depending on command)")
             .optional)
        .parse(args);
    return kargs;
}

void main(string[] args)
{
    try {
        auto pargs = ParseArgs(buildCliArgs(args));

		auto conn = MPDConnection(pargs.host, to!short(pargs.port));

        static foreach(m; __traits(allMembers, ParseArgs)) {
            static if (m != "listall" && m != "add" && m != "quiet" && m != "list" && m != "uris" &&
            		m != "host" && m != "port" && m != "dblocation" && m != "play") {
                mixin("static if (is(typeof(ParseArgs."~m~") == bool)){ if(pargs."~m~") conn."~m~";}");
                mixin("static if (is(typeof(ParseArgs."~m~") == string)){ if(pargs."~m~")
                        conn."~m~"(pargs."~m~");}");
            }
        }

		if ((pargs.listall || pargs.add || pargs.searchTermsR.length > 0)) {
			auto queries = parseQueries(pargs.searchTermsR);
			auto negate = parseQueries(pargs.filterTermsR);
			if(isatty(stdin.fileno) == 1){ // search on db only if interactive
				auto gen = new DBParser(pargs.dblocation);
				gen.all
					.tee!((a){
						if (pargs.listall) pretty_print(a, pargs.uris);
					}) // print db
                .filter!(a => search_queries(a, queries))
                .filter!(a => !search_queries(a, negate))
                .tee!((a){
						if (!pargs.quiet) pretty_print(a, pargs.uris);
					}) // print search results
                .each!((a){
						if (pargs.add) conn.add(a.uri);
					}); // add 
			} else { // we are in a pipe
				foreach(a; stdin.byLineCopy()){
					if (pargs.add)
						conn.add(a); // TODO use each!
				}
			}
		}

        if (pargs.play){
        	if(pargs.playN == 0) conn.play;
        	else conn.play(pargs.playN);
        }

		if (!pargs.delN.isNull)	conn.del(pargs.delN.get);
        // TODO: write as static foreach
        /*
		if (!pargs.move.isNull) conn.move(pargs.move.get);
		if (!pargs.swapR.isNull) conn.swap(pargs.delR.get);
		if (!pargs.delR.isNull)	conn.del_range(pargs.delR.get);
		if (!pargs.shuffleR.isNull)	conn.shuffle_range(pargs.delR.get);
        */

        if(pargs.list){
        	auto highlight = conn.song.position;
            foreach(i, r; conn.playlist.enumerate){
				auto prt = (i+1).to!string ~ ". " ~ r.toString(pargs.uris);
            	if(i == highlight){
					switch(conn.status.state){
						case mpd_state.MPD_STATE_PLAY:
							prt = "\033[95m" ~ prt ~ "\x1b[0m"; break;
						case mpd_state.MPD_STATE_PAUSE:
							prt = "\033[93m" ~ prt ~ "\x1b[0m"; break;
						case mpd_state.MPD_STATE_STOP:
							prt = "\033[91m" ~ prt ~ "\x1b[0m"; break;
						default:
							assert(0);
					}
				}
            	writeln(prt);
            }
        }

		if(!pargs.quiet && pargs.searchTermsR.length == 0 && !pargs.clear && !pargs.list && !pargs.listall){
            auto outstr = conn.statusString;
            if (outstr != "") writeln(conn.statusString);
		}

    } catch (Exception e){
		stderr.writeln(e.msg);
    }
}

void pretty_print(DBUnion s, bool uris)
{
    if (uris || s.title == "") {
        writeln(s.uri);
    } else {
        if(s.kind is DBUnion.Kind.song) {
            writeln(s.artist, " - ", s.album, " - ", s.title);
        } else {
            writeln(s.title);
        }
    }
}
