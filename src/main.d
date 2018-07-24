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

struct ParseArgs{
    bool quiet;
    bool add; //
    bool previous;
    string backward; //
    string forward; //
    bool clear;
    bool list;
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
    bool listall;
    bool play;
    ulong playN;
    ulong delN;
    Tuple!(ulong, ulong) delR;
    Tuple!(ulong, ulong) shuffleR;
    Tuple!(ulong, ulong) swapR;
    string[] searchTermsR;
	string host = "localhost";
	int port = 6600;
	string dblocation = "~/.mpd/database";

    
    immutable string[] oargs;
    GetoptResult rlst;

    void opt_handler(string opt)
    {
        // split long and short option and get original occurrence
        auto div = opt.indexOf("|");
        assert(div > 0);
        auto lng = "--" ~ opt[0 .. div]; auto sht = "-" ~ opt[div+1 .. $];
        assert(sht.length < lng.length);
        auto idx = oargs.countUntil(sht);
        long eqIdx = -1;
        if(idx == -1) idx = oargs.countUntil(lng);
        // manage options such as --play=2
        if(idx == -1) {
            foreach(i, param; oargs.enumerate){
                if(param.canFind(sht~"=") || param.canFind(lng~"=")){
                    idx = i;
                    eqIdx = oargs[idx].indexOf("=");
                    break;
                }
            }
        }
        enforce(idx > 0, "Error in parsing command line arguments");
        assert(eqIdx >= -1);

        if(oargs.length == idx + 1 && eqIdx == -1) {
            switch(opt){
                case "play|p":
                	play = true;
                    playN = 0;
                    break;
                default:
                    throw new GetOptException("Missing value for argument " ~ lng ~ ".");
            }
        } else {
            string arg1 = eqIdx != -1 ? oargs[idx][eqIdx+1 .. $] : oargs[idx+1];
            string arg2;
            switch(opt){
                case "play|p":
                	play = true;
                    playN = to!ulong(arg1);
                    break;
                case "del|d":
                    delN = to!ulong(arg1);
                    break;
                case "del-range|D":
                    enforce(oargs.length >= idx+2, 
                            new GetOptException("Missing second value for argument " ~ lng ~ "."));
                    arg2 = oargs[idx+2];
                    delR = tuple(to!ulong(arg1), to!ulong(arg2));
                    break;
                case "swap":
                    enforce(oargs.length >= idx+2, 
                            new GetOptException("Missing second value for argument " ~ lng ~ "."));
                    arg2 = oargs[idx+2];
                    swapR = tuple(to!ulong(arg1), to!ulong(arg2));
                    break;
                case "shuffle-range":
                    enforce(oargs.length >= idx+2, 
                            new GetOptException("Missing second value for argument " ~ lng ~ "."));
                    arg2 = oargs[idx+2];
                    shuffleR = tuple(to!ulong(arg1), to!ulong(arg2));
                    break;
                default:
                    throw new GetOptException("Missing value for argument " ~ lng ~ ".");
            }
        }
    }

    this(string[] args){
        arraySep = ",";
        oargs = args.idup;
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
        rlst = getopt(args, config.caseSensitive,
                "add|a",  "add to current playlist", &add,
                "previous|b|prev",  "play previous song", &previous,
                "backward|B",  "seek backwards of nsec or %song", &backward,
                "forward|",  "seek forward of nsec or %song", &forward,
                "clear|c",  "clear current playlist", &clear,
                "consume",  "toggle consume", &consume,
                "del|d",  "del specified item from playlist", &opt_handler,
                "del-range|D",  "del specified range from playlist", &opt_handler,
                "list|l",  "display current playlist", &list,
                "list-all|L", "display the whole database", &listall,
                "next|n",  "play next song", &next,
                "display-uri|U",  "display URIs", &uris,
                "play|p",  "toggle play or play specified item", &opt_handler,
                "pause|P", "pause", &pause,
                "random|r",  "toggle random", &random,
                "repeat",  "toggle repeat", &repeat,
                "single",  "toggle single", &single,
                "stop",  "toggle stop", &stop,
                "search|s",  "search given key(s)", &searchTermsR,
                "shuffle|t",  "shuffle current playlist", &shuffle,
                "shuffle-range",  "shuffle given range", &opt_handler,
                "update|u",  "send update request to MPD", &update,
                "swap",  "swap specified items", &opt_handler,
                "quiet|q", "disable output", &quiet,
        );

    }
}

void main(string[] args)
{
    try {

        auto pargs = ParseArgs(args); // constructor does parsing

        if(pargs.rlst.helpWanted) {
            defaultGetoptPrinter("kpd is a client for MPD.", pargs.rlst.options);
            return;
        }

		auto conn = MPDConnection(pargs.host, to!short(pargs.port));

        static foreach(m; __traits(allMembers, ParseArgs)) {
            static if (m != "listall" && m != "add" && m != "quiet" && m != "list" && m != "uris" &&
            		m != "host" && m != "port" && m != "dblocation" && m != "play") {
                mixin("static if (is(typeof(ParseArgs."~m~") == bool)){ if(pargs."~m~") conn."~m~";}");
                mixin("static if (is(typeof(ParseArgs."~m~") == string)){ if(pargs."~m~")
                        conn."~m~"(pargs."~m~");}");
            }
        }
        
        if (pargs.listall || pargs.add || pargs.searchTermsR.length > 0) {
        	auto gen = new DBParser(pargs.dblocation);
            gen.all
                .tee!((a){if (pargs.listall) pretty_print(a, pargs.uris);}) // print db
                .filter!(a => search_queries(a, parseQueries(pargs.searchTermsR))) // search
                .tee!((a){if (!pargs.quiet) pretty_print(a, pargs.uris);}) // print search results
                .each!((a){if (pargs.add) conn.add(a.uri);}); // add 
        }

        if (pargs.play){
        	if(pargs.playN == 0) conn.play;
        	else conn.play(pargs.playN);
        }

        if(pargs.list){
        	auto highlight = conn.song.position;
            foreach(i, r; conn.playlist.enumerate){
				auto prt = (i+1).to!string ~ ". " ~ r.toString(pargs.uris);
            	if(i == highlight)
            		prt = "\033[95m" ~ prt ~ "\x1b[0m";
            	writeln(prt);
            }
        }

		if(!pargs.quiet && pargs.searchTermsR.length == 0 && !pargs.clear && !pargs.list && !pargs.listall){
			writeln(conn.statusString);
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
            auto prt = s.artist ~ " - " ~ s.album ~ " - " ~ s.title;
            writeln(prt);
        } else {
            writeln(s.title);
        }
    }
}

