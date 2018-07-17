import std.stdio;
import std.range;
import std.string;
import std.getopt;
import std.typecons;
import std.algorithm;
import std.conv;
import std.exception;
import std.traits;

import kpd_search;
import database;
import libmpdclient_extern;

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
    ulong playN = -1;
    ulong delN = -1;
    Tuple!(ulong, ulong) delR;
    Tuple!(ulong, ulong) shuffleR;
    Tuple!(ulong, ulong) swapR;
    string[] searchTermsR;

    
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
        assert(idx > 0, to!string(idx));
        assert(eqIdx >= -1);

        if(oargs.length == idx + 1 && eqIdx == -1) {
            switch(opt){
                case "play|p":
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
        oargs = args.idup;
        arraySep = ",";
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
    // flags for cli options


    // TODO config file
    try {
    	string backward;
    	string forward;
        bool list;
        bool uris;
        string seek;

        //string host = "192.168.1.131";
		string host = "localhost";
		short port = 6600;
    	auto conn = MPDConnection(host, port); 

        auto parseArgs = ParseArgs(args);

        if(parseArgs.rlst.helpWanted) {
            defaultGetoptPrinter("kpd is a client for MPD.", parseArgs.rlst.options);
            return;
        }
        auto gen = new DBParser("/home/francesco/.mpd/mpd.db");

        static foreach(m; __traits(allMembers, ParseArgs)) {
            static if (m != "listall" && m != "add" && m != "quiet" && m != "list" && m != "uris") {
                mixin("static if (is(typeof(ParseArgs."~m~") == bool)){ if(parseArgs."~m~") conn."~m~";}");
                mixin("static if (is(typeof(ParseArgs."~m~") == string)){ if(parseArgs."~m~")
                        conn."~m~"(parseArgs."~m~");}");
            }
        }
        
        if (parseArgs.listall || parseArgs.add || parseArgs.searchTermsR.length > 0) {
            gen.all
                .tee!((a){if (parseArgs.listall) pretty_print(a, parseArgs.uris);}) // print db
                .filter!(a => search(a, parseQueries(parseArgs.searchTermsR))) // search
                .tee!((a){if (!parseArgs.quiet) pretty_print(a, parseArgs.uris);}) // print search results
                .each!((a){if (parseArgs.add) conn.add(a.uri);});// add 
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

